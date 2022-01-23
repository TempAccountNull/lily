#pragma once
#include "common/util.h"

#include <Windows.h>
#include <string>
#include <stdio.h>
#include <intrin.h>
#pragma intrinsic(_enable)

#include "common/dbvm.h"
#include "common/encrypt_string.h"
#include "msrnames.h"
#include "physicalmemory.h"
#include "common/exception.h"
#include "patternscan.h"
#include "function_ref.hpp"

using tReadProcessMemory = tl::function<bool(uintptr_t Address, void* Buffer, size_t Size)>;
using tWriteProcessMemory = tl::function<bool(uintptr_t Address, const void* Buffer, size_t Size)>;

extern "C" void RunWithKernelStack(void* pFunc, void* pThis);

class Kernel {
private:
	INITIALIZER_INCLASS(CommitPages) {
		const bool bSuccess = [&] {
			MEMORY_BASIC_INFORMATION MemInfo;
			if (!VirtualQuery(RunWithKernelStack, &MemInfo, sizeof(MemInfo)))
				return false;

			const uintptr_t Base = (uintptr_t)MemInfo.AllocationBase & ~0xFFF;
			const uintptr_t Size = (uintptr_t)MemInfo.BaseAddress - (uintptr_t)MemInfo.AllocationBase + MemInfo.RegionSize;
			for (size_t i = 0; i < Size; i += 0x1000) {
				void* const Page = (void*)(Base + i);
				uint8_t byte;
				if (!ReadProcessMemory((HANDLE)-1, Page, &byte, sizeof(byte), 0))
					return false;
			}

			return true;
		}();

		if (!bSuccess)
			error("CommitPages"e);
	};

public:
	const DBVM& dbvm;
	Kernel(const DBVM& _dbvm) : dbvm(_dbvm) {}
	~Kernel() {
		VirtualFree(DirectoryTableBase, 0, MEM_RELEASE);
		VirtualFree(MapPhysicalPDPTE, 0, MEM_RELEASE);
	}

	Kernel() = delete;
	Kernel(const Kernel&) = delete;
	Kernel& operator=(const Kernel&) = delete;

	const tReadProcessMemory ReadProcessMemoryWinAPI = [&](uintptr_t Address, void* Buffer, size_t Size) {
		return ::ReadProcessMemory((HANDLE)-1, (const void*)Address, Buffer, Size, 0);
	};
	const tWriteProcessMemory WriteProcessMemoryWinAPI = [&](uintptr_t Address, const void* Buffer, size_t Size) {
		return ::WriteProcessMemory((HANDLE)-1, (void*)Address, Buffer, Size, 0);
	};

private:
	PML4E* const DirectoryTableBase = (PML4E*)VirtualAllocVerified(0x1000, PAGE_READWRITE);
	PDPTE* const MapPhysicalPDPTE = (PDPTE*)VirtualAllocVerified(0x1000, PAGE_READWRITE);
	const PML4E CustomPML4E = (uintptr_t)dbvm.GetPhysicalAddress((uintptr_t)MapPhysicalPDPTE, dbvm.GetCR3());
	const CR3 CustomCR3 = [&] {
		verify(CustomPML4E);
		ResetCustomCR3();
		CR3 cr3 = (uintptr_t)dbvm.GetPhysicalAddress((uintptr_t)DirectoryTableBase, dbvm.GetCR3());
		verify(cr3);
		return cr3;
	}();

public:
	const tReadProcessMemory ReadProcessMemoryDBVM = [&](uintptr_t Address, void* Buffer, size_t Size) {
		return dbvm.RPM(Address, Buffer, Size, CustomCR3);
	};

	const tWriteProcessMemory WriteProcessMemoryDBVM = [&](uintptr_t Address, const void* Buffer, size_t Size) {
		return dbvm.WPM(Address, Buffer, Size, CustomCR3);
	};

private:
	class EProcess {
	public:
		DWORD GetPid(const Kernel& kernel) const {
			DWORD Pid;
			if (!kernel.ReadProcessMemoryDBVM((uintptr_t)this + kernel.Offset_UniqueProcessId, &Pid, sizeof(Pid)))
				return 0;

			return Pid;
		}
		EProcess* GetNextProcess(const Kernel& kernel) const {
			LIST_ENTRY ListEntry;
			if (!kernel.ReadProcessMemoryDBVM((uintptr_t)this + kernel.Offset_ActiveProcessLinks, &ListEntry, sizeof(ListEntry)))
				return 0;

			return (EProcess*)((uintptr_t)ListEntry.Flink - kernel.Offset_ActiveProcessLinks);
		}
		CR3 GetKernelCR3(const Kernel& kernel) const {
			CR3 cr3;
			if (!kernel.ReadProcessMemoryDBVM((uintptr_t)this + kernel.Offset_DirectoryTableBase, &cr3, sizeof(cr3)))
				return 0;

			return cr3;
		}
		uintptr_t GetPebAddress(const Kernel& kernel) const {
			uintptr_t PebAddress;
			if (!kernel.ReadProcessMemoryDBVM((uintptr_t)this + kernel.Offset_Peb, &PebAddress, sizeof(PebAddress)))
				return 0;

			return PebAddress;
		}
	};

	//EPROCESS Offset
	EProcess* const SystemProcess = [&] {
		const uintptr_t PsInitialSystemProcess = GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsInitialSystemProcess"e);
		ReadProcessMemoryDBVM(PsInitialSystemProcess, (void*)&SystemProcess, sizeof(SystemProcess));
		verify(SystemProcess);
		return SystemProcess;
	}();

	const unsigned Offset_DirectoryTableBase = 0x28;

	const unsigned Offset_UniqueProcessId = [&] {
		uintptr_t PsGetProcessId = (uintptr_t)GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsGetProcessId"e);
		ReadProcessMemoryDBVM(PsGetProcessId + 0x3, (void*)&Offset_UniqueProcessId, sizeof(Offset_UniqueProcessId));
		verify(Offset_UniqueProcessId);
		return Offset_UniqueProcessId;
	}();

	const unsigned Offset_ActiveProcessLinks = Offset_UniqueProcessId + 0x8;

	const unsigned Offset_Peb = [&] {
		uintptr_t PsGetProcessPeb = (uintptr_t)GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsGetProcessPeb"e);
		ReadProcessMemoryDBVM(PsGetProcessPeb + 0x3, (void*)&Offset_Peb, sizeof(Offset_Peb));
		verify(Offset_Peb);
		return Offset_Peb;
	}();

	//Map 512GB(few computer that has ram over 512GB)
	const PhysicalAddress MaxPhysicalAddress = 0x40000000i64 * 0x200;

	void MapPhysicalAddressSpaceToCustomCR3() {
		//Map 512GB(few computer that has ram over 512GB)
		for (uintptr_t i = 0; i < 0x200; i++) {
			PDPTE pdpte = i * 0x40000000;
			pdpte.Present = 1;
			pdpte.ReadWrite = 1;
			pdpte.UserSupervisor = 1;
			pdpte.PageSize = 1;
			MapPhysicalPDPTE[i] = pdpte;
		}

		for (mapLinkPhysical = 0x80; DirectoryTableBase[mapLinkPhysical] && mapLinkPhysical < 0xFF; mapLinkPhysical++);
		verify(mapLinkPhysical < 0xFF);

		PML4E pml4e = CustomPML4E;
		pml4e.Present = 1;
		pml4e.ReadWrite = 1;
		pml4e.UserSupervisor = 1;
		DirectoryTableBase[mapLinkPhysical] = pml4e;
	}

	void ResetCustomCR3() {
		const bool bSuccess = [&] {
			const uintptr_t GsBase = dbvm.ReadMSR(IA32_KERNEL_GS_BASE_MSR);
			const CR3 userCR3 = dbvm.GetCR3();
			CR3 krnlCR3 = 0;

			//The process is not kva-shadowed.
			if (dbvm.GetPhysicalAddress(GsBase, userCR3))
				return dbvm.ReadPhysicalMemory(userCR3.PageFrameNumber * 0x1000, &DirectoryTableBase[0x000], 0x1000);

			//Finding KernelDirectoryBase...
			for (size_t Offset = 0x1000; Offset < 0x10000 &&
				!dbvm.RPM(GsBase + Offset, &krnlCR3, sizeof(krnlCR3), userCR3); Offset += 0x1000);

			if (!krnlCR3)
				return false;

			return
				dbvm.ReadPhysicalMemory(userCR3.PageFrameNumber * 0x1000 + 0x000, &DirectoryTableBase[0x000], 0x800) &&
				dbvm.ReadPhysicalMemory(krnlCR3.PageFrameNumber * 0x1000 + 0x800, &DirectoryTableBase[0x100], 0x800);
		}();

		verify(bSuccess);
		MapPhysicalAddressSpaceToCustomCR3();
	}

	CR3 mapCR3 = { 0 };
	DWORD mapPid = 0;
	uint8_t mapLinkPhysical = 0;
	uint8_t mapLink[0x100] = { 0 };

public:
	template<class R>
	R KernelExecute(bool bSetInterrupt, tl::function<R(void)> CallBack) const {
		R Result;
		KernelExecute<void>(bSetInterrupt, [&] { Result = CallBack(); });
		return Result;
	}

	template<>
	void KernelExecute(bool bSetInterrupt, tl::function<void(void)> CallBack) const {
		dbvm.SwitchToKernelMode(0x10);
		_stac();
		if (bSetInterrupt) _enable();
		__writecr3(CustomCR3);
		RunWithKernelStack(CallBack.callback(), CallBack.obj());
		dbvm.ReturnToUserMode();
	}

#ifdef TL_IGNORE_GUARD
	template <class F>
	class UserFunction;
	template <class R, class... Args>
	class UserFunction<R(Args...)> : public tl::function<R(Args...)> {};

	template <class F>
	class KernelFunction;
	template <class R, class... Args>
	class KernelFunction<R(Args...)> : public tl::function<R(Args...)> {
		const Kernel& kernel;
		const bool bSetInterrupt;
	public:
		using super = tl::function<R(Args...)>;

		template<class FN>
		KernelFunction(FN&& f, const Kernel& kernel, const bool bSetInterrupt = false) :
			super(f), kernel(kernel), bSetInterrupt(bSetInterrupt) {}

		R operator()(Args... args) const {
			return kernel.KernelExecute<R>(bSetInterrupt, [&] { return super::operator()(args...); });
		}
	};
#endif

	void SetCustomCR3() const { dbvm.SetCR3(CustomCR3); }
	CR3 GetMapCR3() const { return mapCR3; }

	static bool MemcpyWithExceptionHandler(void* Dst, const void* Src, size_t Size) {
		return ExceptionHandler::TryExcept([&]() { memcpy(Dst, Src, Size); });
	}

private:
	const tReadPhysicalMemory ReadPhysicalMemory =
		[&](PhysicalAddress Address, void* Buffer, size_t Size) {
		uintptr_t MappedAddress = GetMappedPhysicalAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler(Buffer, (void*)MappedAddress, Size))
			return true;

		SetCustomCR3();
		return dbvm.ReadPhysicalMemory(Address, Buffer, Size);
	};

	const tWritePhysicalMemory WritePhysicalMemory =
		[&](PhysicalAddress Address, const void* Buffer, size_t Size) {
		uintptr_t MappedAddress = GetMappedPhysicalAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler((void*)MappedAddress, Buffer, Size))
			return true;

		SetCustomCR3();
		return dbvm.WritePhysicalMemory(Address, Buffer, Size);
	};
		
	PhysicalAddress GetPhysicalAddress(uintptr_t VirtualAddress, CR3 cr3) const {
		return PhysicalMemory::GetPhysicalAddress(VirtualAddress, cr3, ReadPhysicalMemory);
	}

	bool WriteProcessMemoryCR3(uintptr_t Address, const void* Buffer, size_t Size, CR3 cr3) const {
		return PhysicalMemory::WriteProcessMemory(Address, Buffer, Size, cr3, ReadPhysicalMemory, WritePhysicalMemory);
	}

	bool ReadProcessMemoryCR3(uintptr_t Address, void* Buffer, size_t Size, CR3 cr3) const {
		return PhysicalMemory::ReadProcessMemory(Address, Buffer, Size, cr3, ReadPhysicalMemory);
	}

public:
	//Be carefull with system calls, exceptions, interrupts and memory commit when KVA shadowed.(CR3 change)
	//no CR3 Change when the program run with "administrator privileges".
	//WPM_Mapped can not access kernel memory space.

	const tReadProcessMemory ReadProcessMemory = [&](uintptr_t Address, void* Buffer, size_t Size) {
		return ReadProcessMemoryCR3(Address, Buffer, Size, mapCR3);
	};

	const tWriteProcessMemory WriteProcessMemory = [&](uintptr_t Address, const void* Buffer, size_t Size) {
		return WriteProcessMemoryCR3(Address, Buffer, Size, mapCR3);
	};

	const tReadProcessMemory ReadProcessMemoryFast = [&](uintptr_t Address, void* Buffer, size_t Size) {
		uintptr_t MappedAddress = GetMappedAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler(Buffer, (void*)MappedAddress, Size))
			return true;

		SetCustomCR3();
		return ReadProcessMemory(Address, Buffer, Size);
	};

	const tWriteProcessMemory WriteProcessMemoryFast = [&](uintptr_t Address, const void* Buffer, size_t Size) {
		uintptr_t MappedAddress = GetMappedAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler((void*)MappedAddress, Buffer, Size))
			return true;

		SetCustomCR3();
		return WriteProcessMemory(Address, Buffer, Size);
	};

	EProcess* GetEPROCESS(DWORD Pid) const {
		for (EProcess* pProcess = SystemProcess->GetNextProcess(*this);
			pProcess && pProcess != SystemProcess;
			pProcess = pProcess->GetNextProcess(*this)) {
			if (pProcess->GetPid(*this) == Pid)
				return pProcess;
		}
		return 0;
	}

	CR3 GetKernelCR3(DWORD Pid) const {
		const EProcess* pProcess = GetEPROCESS(Pid);
		if (!pProcess)
			return 0;

		return pProcess->GetKernelCR3(*this);
	}

	uintptr_t GetPebAddress(DWORD Pid) const {
		const EProcess* pProcess = GetEPROCESS(Pid);
		if (!pProcess)
			return 0;

		return pProcess->GetPebAddress(*this);
	}

	uintptr_t GetMappedPhysicalAddress(PhysicalAddress Address) const {
		if (Address >= MaxPhysicalAddress) return 0;
		return 0x8000000000 * mapLinkPhysical + Address;
	}

	uintptr_t GetMappedAddress(uintptr_t Address) const {
		if (!mapCR3) return 0;
		if (!Address) return 0;
		if (Address >= 0x800000000000) return 0;
		if (!mapLink[Address / 0x8000000000]) return 0;
		return 0x8000000000 * mapLink[Address / 0x8000000000] + (Address % 0x8000000000);
	}

	bool MapProcess(DWORD Pid) {
		mapCR3 = 0;
		mapPid = 0;
		CR3 cr3 = GetKernelCR3(Pid);
		if (!cr3)
			return false;

		ResetCustomCR3();

		PML4E mapPML4E[0x100];
		dbvm.ReadPhysicalMemory(cr3.PageFrameNumber * 0x1000, mapPML4E, sizeof(mapPML4E));

		int j = 0x80;
		for (int i = 0; i < 0x100; i++) {
			mapLink[i] = 0;
			if (!mapPML4E[i])
				continue;

			//find empty entry
			while (DirectoryTableBase[j] && j < 0x100) j++;
			if (j >= 0x100)
				return false;
			mapPML4E[i].ExecuteDisable = 0;
			DirectoryTableBase[j] = mapPML4E[i];
			mapLink[i] = j++;
		}

		mapCR3 = cr3;
		mapPid = Pid;
		return true;
	}
};
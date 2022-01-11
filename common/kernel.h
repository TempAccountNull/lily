#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <string>
#include <stdio.h>
#include <intrin.h>
#pragma intrinsic(_enable)

#include "util.h"
#include "dbvm.h"
#include "encrypt_string.h"
#include "msrnames.h"
#include "physicalmemory.h"
#include "exception.h"
#include "patternscan.h"
#include "function_ref.hpp"

extern "C" void RunWithKernelStack(void* pFunc, void* pThis);

class Kernel {
private:
	Kernel* const _this = this;

	INITIALIZER_INCLASS(CommitAndLockPages) {
		MEMORY_BASIC_INFORMATION MemInfo;
		VirtualQuery(RunWithKernelStack, &MemInfo, sizeof(MemInfo));
		const uintptr_t Base = (uintptr_t)MemInfo.AllocationBase & ~0xFFF;
		const uintptr_t Size = (uintptr_t)MemInfo.BaseAddress - (uintptr_t)MemInfo.AllocationBase + MemInfo.RegionSize;
		for (size_t i = 0; i < Size; i += 0x1000) {
			void* Page = (void*)(Base + i);
			uint8_t byte;
			ReadProcessMemory((HANDLE)-1, Page, &byte, sizeof(byte), 0);
			VirtualLock(Page, 0x1000);
		}
	};

public:
	const DBVM& dbvm;
	Kernel(const DBVM& _dbvm) : dbvm(_dbvm) {}
	~Kernel() {
		VirtualFree(DirectoryTableBase, 0, MEM_RELEASE);
		VirtualFree(MapPhysicalPDPTE, 0, MEM_RELEASE);
	}

private:
	const PPML4E DirectoryTableBase = (PPML4E)VirtualAllocLock(0x1000, PAGE_READWRITE);
	const PPDPTE MapPhysicalPDPTE = (PPDPTE)VirtualAllocLock(0x1000, PAGE_READWRITE);
	const PML4E CustomPML4E = { .Value = dbvm.GetPhysicalAddress((uintptr_t)MapPhysicalPDPTE, dbvm.GetCR3()) };
	const CR3 CustomCR3 = [&] {
		verify(CustomPML4E.Value);
		GetReadyToUseCustomCR3();
		CR3 cr3 = { .Value = dbvm.GetPhysicalAddress((uintptr_t)DirectoryTableBase, dbvm.GetCR3()) };
		verify(cr3.Value);
		return cr3;
	}();

public:
	AUTO_VARIABLE(const WPM_dbvm, std::bind(&DBVM::WPM, &dbvm, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, CustomCR3));
	AUTO_VARIABLE(const RPM_dbvm, std::bind(&DBVM::RPM, &dbvm, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, CustomCR3));

private:
	
	//EPROCESS Offset
	const uintptr_t SystemProcess = [&] {
		const uintptr_t PsInitialSystemProcess = GetKernelProcAddress("ntoskrnl.exe"e, "PsInitialSystemProcess"e);
		RPM_dbvm(PsInitialSystemProcess, (void*)&SystemProcess, sizeof(SystemProcess));
		verify(SystemProcess);
		return SystemProcess;
	}();

	const unsigned Offset_DirectoryTableBase = 0x28;

	const unsigned Offset_UniqueProcessId = [&] {
		uintptr_t PsGetProcessId = (uintptr_t)GetKernelProcAddress("ntoskrnl.exe"e, "PsGetProcessId"e);
		RPM_dbvm(PsGetProcessId + 0x3, (void*)&Offset_UniqueProcessId, sizeof(Offset_UniqueProcessId));
		verify(Offset_UniqueProcessId);
		return Offset_UniqueProcessId;
	}();

	const unsigned Offset_ActiveProcessLinks = Offset_UniqueProcessId + 0x8;

	const unsigned Offset_Peb = [&] {
		uintptr_t PsGetProcessPeb = (uintptr_t)GetKernelProcAddress("ntoskrnl.exe"e, "PsGetProcessPeb"e);
		RPM_dbvm(PsGetProcessPeb + 0x3, (void*)&Offset_Peb, sizeof(Offset_Peb));
		verify(Offset_Peb);
		return Offset_Peb;
	}();

	//Map 512GB(few computer that has ram over 512GB)
	const PhysicalAddress MaxPhysicalAddress = 0x40000000i64 * 0x200;

	void MapPhysicalAddressSpaceToCustomCR3() {
		//Map 512GB(few computer that has ram over 512GB)
		for (uintptr_t i = 0; i < 0x200; i++) {
			PDPTE a;
			a.Value = i * 0x40000000;
			a.Present = 1;
			a.ReadWrite = 1;
			a.UserSupervisor = 1;
			a.PageSize = 1;
			MapPhysicalPDPTE[i].Value = a.Value;
		}

		for (mapLinkPhysical = 0x80; DirectoryTableBase[mapLinkPhysical].Value && mapLinkPhysical < 0x100; mapLinkPhysical++);

		PML4E _PML4E;
		_PML4E.Value = CustomPML4E.Value;
		_PML4E.Present = 1;
		_PML4E.ReadWrite = 1;
		_PML4E.UserSupervisor = 1;

		DirectoryTableBase[mapLinkPhysical].Value = _PML4E.Value;
	}

	void GetReadyToUseCustomCR3() {
		CR3 userCR3 = dbvm.GetCR3();
		CR3 krnlCR3 = { 0 };

		dbvm.SwitchToKernelMode(0x10);
		uintptr_t GsBase = __readmsr(IA32_KERNEL_GS_BASE_MSR);
		dbvm.ReturnToUserMode();

		//Get first accessible gs offset
		//if kva shadowed, only a page are accessible
		//in windows 7,  gs:[0x6000~0x6FFF] region is accessible and other regions are hidden.
		//in windows 10, gs:[0x7000~0x7FFF] region is accessible and other regions are hidden.
		if (!dbvm.GetPhysicalAddress(GsBase, userCR3)) {
			//The process is kva shadowed.
			for (unsigned i = 0x1000; i < 0x10000; i += 0x1000) {
				//Find CR3 offset
				if (dbvm.RPM(GsBase + i, &krnlCR3, sizeof(krnlCR3), userCR3)) {
					break;
				}
			}
			verify(krnlCR3.Value);
		}

		if (krnlCR3.Value) {
			//KernelDirectoryTableBase(if kva shadowed)
			//User page marked No-Execute Enable in windows 10 kva shadowed
			dbvm.ReadPhysicalMemory(userCR3.PageFrameNumber * 0x1000 + 0x000, &DirectoryTableBase[0x000], 0x800);
			dbvm.ReadPhysicalMemory(krnlCR3.PageFrameNumber * 0x1000 + 0x800, &DirectoryTableBase[0x100], 0x800);
		}
		else dbvm.ReadPhysicalMemory(userCR3.PageFrameNumber * 0x1000, &DirectoryTableBase[0x000], 0x1000);

		MapPhysicalAddressSpaceToCustomCR3();
	}

	CR3 mapCR3 = { 0 };
	DWORD mapPid = 0;
	uint8_t mapLinkPhysical = 0;
	uint8_t mapLink[0x100] = { 0 };

public:
	void KernelExecute(tl::function_ref<void(void)> CallBack, bool bSetInterrupt = false) const {
		dbvm.SwitchToKernelMode(0x10);
		_stac();
		if (bSetInterrupt) _enable();
		__writecr3(CustomCR3.Value);
		RunWithKernelStack(CallBack.callback_, CallBack.obj_);
		dbvm.ReturnToUserMode();
	}

	template<class Type, class R = std::invoke_result_t<Type>, class = std::enable_if_t<!std::is_same_v<R, void>, Type>>
	auto KernelExecute(Type CallBack, bool bSetInterrupt = false) const {
		R Result;
		KernelExecute([&] { Result = CallBack(); }, bSetInterrupt);
		return Result;
	}

	void SetCustomCR3() const { dbvm.SetCR3(CustomCR3); }
	CR3 GetMappedProcessCR3() const { return mapCR3; }

	static uintptr_t GetKernelModuleAddress(const char* szModule) {
		void* Drivers[0x400];
		DWORD cbNeeded;
		if (!EnumDeviceDrivers(Drivers, sizeof(Drivers), &cbNeeded))
			return 0;

		for (auto Driver : Drivers) {
			char szBaseName[MAX_PATH];
			if (!GetDeviceDriverBaseNameA(Driver, szBaseName, sizeof(szBaseName)))
				continue;
			if (_stricmp(szBaseName, szModule) == 0)
				return (uintptr_t)Driver;
		}

		return 0;
	}

	static uintptr_t GetKernelProcAddress(const char* szModuleName, const char* szProcName) {
		HMODULE hModule = LoadLibraryA(szModuleName);
		uintptr_t hModuleKernel = GetKernelModuleAddress(szModuleName);
		uintptr_t ProcAddr = (uintptr_t)GetProcAddress(hModule, szProcName);
		if (!ProcAddr)
			return 0;
		uintptr_t Result = ProcAddr + hModuleKernel - (uintptr_t)hModule;
		FreeLibrary(hModule);
		return Result;
	}

	static uintptr_t GetUserProcAddress(const char* szModuleName, const char* szProcName) {
		HMODULE hModule = LoadLibraryA(szModuleName);
		uintptr_t Result = (uintptr_t)GetProcAddress(hModule, szProcName);
		FreeLibrary(hModule);
		return Result;
	}

	static bool MemcpyWithExceptionHandler(void* Dst, const void* Src, size_t Size) {
		return ExceptionHandler::TryExcept([&]() { memcpy(Dst, Src, Size); });
	}

private:
	bool _WritePhysicalMemory(PhysicalAddress Address, const void* Buffer, size_t Size) const {
		uintptr_t MappedAddress = GetMappedPhysicalAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler((void*)MappedAddress, Buffer, Size))
			return true;

		SetCustomCR3();
		return dbvm.WritePhysicalMemory(Address, Buffer, Size);
	}

	bool _ReadPhysicalMemory(PhysicalAddress Address, void* Buffer, size_t Size) const {
		uintptr_t MappedAddress = GetMappedPhysicalAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler(Buffer, (void*)MappedAddress, Size))
			return true;

		SetCustomCR3();
		return dbvm.ReadPhysicalMemory(Address, Buffer, Size);
	}

public:
	AUTO_VARIABLE(const WritePhysicalMemory, std::bind(&Kernel::_WritePhysicalMemory, _this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	AUTO_VARIABLE(const ReadPhysicalMemory, std::bind(&Kernel::_ReadPhysicalMemory, _this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	PhysicalAddress GetPhysicalAddress(uintptr_t VirtualAddress, CR3 cr3) const {
		return GetPhysicalAddressByPhysicalMemoryAccess(VirtualAddress, cr3, ReadPhysicalMemory);
	}

	bool WPM_Physical(uintptr_t Address, const void* Buffer, size_t Size, CR3 cr3) const {
		return WriteProcessMemoryByPhysicalMemoryAccess(Address, Buffer, Size, cr3, ReadPhysicalMemory, WritePhysicalMemory);
	}

	bool RPM_Physical(uintptr_t Address, void* Buffer, size_t Size, CR3 cr3) const {
		return ReadProcessMemoryByPhysicalMemoryAccess(Address, Buffer, Size, cr3, ReadPhysicalMemory);
	}

	bool WPM_Physical(uintptr_t Address, const void* Buffer, size_t Size) const {
		return WPM_Physical(Address, Buffer, Size, mapCR3);
	}

	bool RPM_Physical(uintptr_t Address, void* Buffer, size_t Size) const {
		return RPM_Physical(Address, Buffer, Size, mapCR3);
	}

	//Be carefull with system calls, exceptions, interrupts and memory commit when KVA shadowed.(CR3 change)
	//no CR3 Change when the program run with "administrator privileges".
	//WPM_Mapped can not access kernel memory space.
	bool WPM_Mapped(uintptr_t Address, const void* Buffer, size_t Size) const {
		uintptr_t MappedAddress = GetMappedAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler((void*)MappedAddress, Buffer, Size))
			return true;

		SetCustomCR3();
		return WPM_Physical(Address, Buffer, Size, mapCR3);
	}

	//Be carefull with system calls, exceptions, interrupts and memory commit when KVA shadowed.(CR3 change)
	//no CR3 Change when the program run with "administrator privileges".
	//RPM_Mapped can not access kernel memory space.
	bool RPM_Mapped(uintptr_t Address, void* Buffer, size_t Size) const {
		uintptr_t MappedAddress = GetMappedAddress(Address);
		if (!MappedAddress)
			return false;

		if (MemcpyWithExceptionHandler(Buffer, (void*)MappedAddress, Size))
			return true;

		SetCustomCR3();
		return RPM_Physical(Address, Buffer, Size, mapCR3);
	}

	uintptr_t GetEPROCESS(DWORD Pid) const {
		uintptr_t pProcess = SystemProcess;
		do {
			DWORD UniqueProcessId;
			if (!RPM_Physical(pProcess + Offset_UniqueProcessId, &UniqueProcessId, sizeof(UniqueProcessId), CustomCR3))
				break;

			if (UniqueProcessId == Pid)
				return pProcess;

			LIST_ENTRY ListEntry;
			if (!RPM_Physical(pProcess + Offset_ActiveProcessLinks, &ListEntry, sizeof(ListEntry), CustomCR3))
				break;

			pProcess = (uintptr_t)ListEntry.Flink - Offset_ActiveProcessLinks;
		} while (pProcess != SystemProcess);

		return 0;
	}

	CR3 GetKernelCR3(DWORD Pid) const {
		CR3 cr3 = { 0 };

		uintptr_t pProcess = GetEPROCESS(Pid);
		if (pProcess)
			RPM_Physical(pProcess + Offset_DirectoryTableBase, &cr3, sizeof(cr3), CustomCR3);

		return cr3;
	}

	uintptr_t GetPebAddress(DWORD Pid) const {
		uintptr_t pProcess = GetEPROCESS(Pid);
		if (!pProcess)
			return 0;

		uintptr_t PebAddress;
		if (!RPM_Physical(pProcess + Offset_Peb, &PebAddress, sizeof(PebAddress), CustomCR3))
			return 0;

		return PebAddress;
	}

	uintptr_t GetMappedPhysicalAddress(PhysicalAddress Address) const {
		if (Address >= MaxPhysicalAddress) return 0;
		return 0x8000000000 * mapLinkPhysical + Address;
	}

	uintptr_t GetMappedAddress(uintptr_t Address) const {
		if (!mapCR3.Value) return 0;
		if (!Address) return 0;
		if (Address >= 0x800000000000) return 0;
		if (!mapLink[Address / 0x8000000000]) return 0;
		return 0x8000000000 * mapLink[Address / 0x8000000000] + (Address % 0x8000000000);
	}

	bool MapProcess(DWORD Pid) {
		mapCR3.Value = mapPid = 0;
		CR3 cr3 = GetKernelCR3(Pid);
		if (!cr3.Value)
			return false;

		GetReadyToUseCustomCR3();

		PML4E mapPML4E[0x100];
		dbvm.ReadPhysicalMemory(cr3.PageFrameNumber * 0x1000, mapPML4E, sizeof(mapPML4E));

		int j = 0x80;
		for (int i = 0; i < 0x100; i++) {
			mapLink[i] = 0;
			if (!mapPML4E[i].Value)
				continue;

			//find empty entry
			while (DirectoryTableBase[j].Value && j < 0x100) j++;
			if (j >= 0x100)
				return false;
			mapPML4E[i].ExecuteDisable = 0;
			DirectoryTableBase[j].Value = mapPML4E[i].Value;
			mapLink[i] = j++;
		}

		mapCR3 = cr3;
		mapPid = Pid;
		return true;
	}

	template <class F>
	class SafeCall;

	template <class R, class... Args>
	class SafeCall<R(Args...)> {
		using Type = R(*)(Args...);
		Type pFunc;
	public:
		SafeCall(auto p) : pFunc((Type)p) {}

		SafeCall& operator=(auto p) {
			return pFunc = (Type)p;
		}

		__declspec(guard(ignore)) R operator()(Args... args) const {
			return pFunc(args...);
		}
	};

	template <class F>
	class KernelCall;

	template <class R, class... Args>
	class KernelCall<R(Args...)> : public SafeCall<R(Args...)> {
		const Kernel& kernel;
	public:
		KernelCall(auto p, const Kernel& kernel) : SafeCall<R(Args...)>(p), kernel(kernel) {}

		R operator()(Args... args) const {
			return kernel.KernelExecute([&] { return (*(const SafeCall<R(Args...)>*)this)(args...); });
		}
	};
};
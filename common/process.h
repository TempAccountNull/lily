#pragma once
#include <windows.h>
#include <tlhelp32.h>

#include "kernel_lily.h"
#include "peb.h"

class Process {
private:
	uintptr_t BaseAddress = 0;
	uintptr_t SizeOfImage = 0;
	PEB64 PebSaved = {0};
	HWND hWndSaved = 0;
	DWORD PidSaved = 0;

	bool LoadEntireImageToRam() const {
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, PidSaved);
		if (!hProcess)
			return false;

		bool bSuccess = true;
		for (uintptr_t i = 0; i < SizeOfImage; i += 0x1000) {
			uint8_t byte;
			if (!::ReadProcessMemory(hProcess, (void*)(BaseAddress + i), &byte, 1, 0)) {
				bSuccess = false;
				break;
			}
		}
		CloseHandle(hProcess);
		return bSuccess;
	}

public:
	KernelLily& kernel;

	Process(KernelLily& kernel) : kernel(kernel) {}

	uintptr_t GetBaseAddress() const { return BaseAddress; }
	uintptr_t GetSizeOfImage() const { return SizeOfImage; }
	HWND GetHwnd() const { return hWndSaved; }
	DWORD GetPid() const { return PidSaved; }

	const tl::function<bool(uintptr_t Address, void* Buffer, uintptr_t Size)> ReadProcessMemory =
		[&](uintptr_t Address, void* Buffer, uintptr_t Size) -> bool {
		return kernel.RPM_Mapped(Address, Buffer, Size);
	};
	const tl::function<bool(uintptr_t Address, const void* Buffer, uintptr_t Size)> WriteProcessMemory =
		[&](uintptr_t Address, const void* Buffer, uintptr_t Size) {
		return kernel.WPM_Mapped(Address, Buffer, Size);
	};

	template <class T>
	bool GetValue(uintptr_t Address, T* Buffer) const { return ReadProcessMemory(Address, Buffer, sizeof(T)); }
	template <class T>
	bool SetValue(uintptr_t Address, const T* Buffer) const { return WriteProcessMemory(Address, Buffer, sizeof(T)); }

	template <class T>
	bool GetBaseValue(uintptr_t Address, T* Buffer) const { return ReadProcessMemory(BaseAddress + Address, Buffer, sizeof(T)); }
	template <class T>
	bool SetBaseValue(uintptr_t Address, const T* Buffer) const { return WriteProcessMemory(BaseAddress + Address, Buffer, sizeof(T)); }

	bool SetBaseDLL(const char* szDLLName = 0) {
		wchar_t wDLLName[0x200] = { 0 };
		if (szDLLName)
			mbstowcs(wDLLName, szDLLName, strlen(szDLLName));

		PEB_LDR_DATA64 ldr_data;
		if (GetValue(PebSaved.LoaderData, &ldr_data) == 0)
			return false;

		uintptr_t pldr = ldr_data.InLoadOrderModuleList.Flink;

		do {
			LDR_DATA_TABLE_ENTRY64 ldr_entry;
			if (!GetValue(pldr, &ldr_entry))
				break;

			wchar_t wBaseDLLName[0x200];
			if (ReadProcessMemory(ldr_entry.BaseDllName.Buffer, wBaseDLLName, ldr_entry.BaseDllName.Length * sizeof(wchar_t))) {
				if (szDLLName == 0 || _wcsicmp(wBaseDLLName, wDLLName) == 0) {
					BaseAddress = ldr_entry.BaseAddress;
					SizeOfImage = ldr_entry.SizeOfImage;
					return true;
				}
			}

			pldr = ldr_entry.InLoadOrderModuleList.Flink;
		} while (pldr != ldr_data.InLoadOrderModuleList.Flink);

		return false;
	}

	uintptr_t AobscanRange(uintptr_t BaseAddress, size_t Len, const char* szPattern) const {
		return PatternScan::Range(BaseAddress, Len, szPattern, ReadProcessMemory);
	}

	uintptr_t AobscanCurrentDLL(const char* szPattern, const char* szSectionName = ".text"e) const {
		LoadEntireImageToRam();
		return PatternScan::Module(BaseAddress, szSectionName, szPattern, ReadProcessMemory);
	}

	bool OpenProcessWithPid(DWORD Pid, const char* szDLLName = 0) {
		PidSaved = Pid;

		uintptr_t PebBaseAddress = kernel.GetPebAddress(PidSaved);
		if (!PebBaseAddress)
			return false;

		if (!kernel.MapProcess(PidSaved))
			return false;

		if (!GetValue(PebBaseAddress, &PebSaved))
			return false;

		return SetBaseDLL(szDLLName);
	}

	bool OpenProcessWithName(const char* szProcName, const char* szDLLName = 0) {
		PidSaved = 0;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(pe32);

		if (Process32First(hSnapshot, &pe32)) {
			do {
				if (_stricmp(pe32.szExeFile, szProcName) == 0) {
					PidSaved = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}

		CloseHandle(hSnapshot);
		return OpenProcessWithPid(PidSaved, szDLLName);
	}

	bool OpenProcessWithHWND(HWND hWnd, const char* szDLLName = 0) {
		hWndSaved = hWnd;
		if (!GetWindowThreadProcessId(hWndSaved, &PidSaved))
			return false;

		return OpenProcessWithPid(PidSaved, szDLLName);
	}
};
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

public:
	KernelLily& kernel;

	Process(KernelLily& kernel) : kernel(kernel) {}

	uintptr_t GetBaseAddress() const { return BaseAddress; }
	uintptr_t GetSizeOfImage() const { return SizeOfImage; }
	HWND GetHwnd() const { return hWndSaved; }
	DWORD GetPid() const { return PidSaved; }

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
			if (GetValueWithSize(ldr_entry.BaseDllName.Buffer, wBaseDLLName, ldr_entry.BaseDllName.Length * sizeof(wchar_t))) {
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
		return PatternScan::Range(BaseAddress, Len, szPattern,
			[&](uintptr_t Address, void* Buffer, size_t Size) {
				return GetValueWithSize(Address, Buffer, Size);
			});
	}

	uintptr_t AobscanCurrentDLL(const char* szPattern, const char* szSectionName = ".text"e) const {
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, PidSaved);
		if (hProcess) {
			for (uintptr_t i = 0; i < SizeOfImage; i += 0x1000) {
				uint8_t byte;
				//Trigger paging
				ReadProcessMemory(hProcess, (void*)(BaseAddress + i), &byte, 1, NULL);
			}
			CloseHandle(hProcess);
		}
		
		return PatternScan::Module(BaseAddress, szSectionName, szPattern,
			[&](uintptr_t Address, void* Buffer, size_t Size) {
				return GetValueWithSize(Address, Buffer, Size);
			});
	}

	bool GetValueWithSize(uintptr_t Address, void* Buffer, uintptr_t Size) const { return kernel.RPM_Mapped(Address, Buffer, Size); }
	bool SetValueWithSize(uintptr_t Address, const void* Buffer, uintptr_t Size) const { return kernel.WPM_Mapped(Address, Buffer, Size); }

	template <class T>
	bool GetValue(uintptr_t Address, T* Buffer) const { return kernel.RPM_Mapped(Address, Buffer, sizeof(T)); }
	template <class T>
	bool SetValue(uintptr_t Address, const T* Buffer) const { return kernel.WPM_Mapped(Address, Buffer, sizeof(T)); }

	bool GetBaseValueWithSize(uintptr_t Address, void* Buffer, uintptr_t Size) const { return kernel.RPM_Mapped(BaseAddress + Address, Buffer, Size); }
	bool SetBaseValueWithSize(uintptr_t Address, const void* Buffer, uintptr_t Size) const { return kernel.WPM_Mapped(BaseAddress + Address, Buffer, Size); }

	template <class T>
	bool GetBaseValue(uintptr_t Address, T* Buffer) const { return kernel.RPM_Mapped(BaseAddress + Address, Buffer, sizeof(T)); }
	template <class T>
	bool SetBaseValue(uintptr_t Address, const T* Buffer) const { return kernel.WPM_Mapped(BaseAddress + Address, Buffer, sizeof(T)); }

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
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 entry; //current process
		entry.dwSize = sizeof(entry);

		if (!Process32First(hSnapshot, &entry)) {
			CloseHandle(hSnapshot);
			return false;
		}

		DWORD Pid;
		do {
			if (_stricmp(entry.szExeFile, szProcName) == 0) {
				Pid = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &entry));

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
#pragma once
#include "kernel.h"
#include "obcallback.h"

#define ALTITUDE_BE L"363220"e

DefBaseClass(tagWND,
	MemberAtOffset(DWORD, dwExStyle, 0x18)
	MemberAtOffset(RECT, rcWindow, 0x58)
,)

typedef struct {}*PPPROP;
typedef struct {}*PPROP;
typedef struct {}*PWND;

class KernelLily : public Kernel {
public:
	KernelLily(const DBVM& dbvm) : Kernel(dbvm) {}

	const KernelFunction<PVOID(PVOID Process)> PsGetProcessWin32Process = {
		reinterpret_cast<PVOID(*)(PVOID Process)>
		(GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsGetProcessWin32Process"e)), *this };

	const KernelFunction<NTSTATUS(PVOID Process, PVOID Win32Process, PVOID PrevWin32Process)> PsSetProcessWin32Process = {
		reinterpret_cast<NTSTATUS(*)(PVOID Process, PVOID Win32Process, PVOID PrevWin32Process)>
		(GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsSetProcessWin32Process"e)), *this };

	const KernelFunction<PWND(HWND hWnd)> ValidateHwnd = {
		reinterpret_cast<PWND(*)(HWND hWnd)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "ValidateHwnd"e)), *this };
		
	const KernelFunction<PVOID(DWORD PoolType, SIZE_T NumberOfBytes)> ExAllocatePool = {
		reinterpret_cast<PVOID(*)(DWORD PoolType, SIZE_T NumberOfBytes)>
		(GetKernelProcAddressVerified("ntoskrnl.exe"e, "ExAllocatePool"e)), *this };
		
	const KernelFunction<ATOM(PCWSTR AtomName)> UserFindAtom = {
		reinterpret_cast<ATOM(*)(PCWSTR AtomName)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "UserFindAtom"e)), *this };

	const KernelFunction<HANDLE(PPROP pProp, ATOM nAtom, DWORD dwFlag)> RealGetProp = {
		reinterpret_cast<HANDLE(*)(PPROP pProp, ATOM nAtom, DWORD dwFlag)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "RealGetProp"e)), *this };

	const KernelFunction<BOOL(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag)> RealInternalSetProp = {
		reinterpret_cast<BOOL(*)(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "RealInternalSetProp"e)), *this };

	const UserFunction<tagWND* (HWND hWnd)> UserValidateHwnd = [&] {
		const uintptr_t ScanResult = PatternScan::Range((uintptr_t)IsChild, 0x30, "48 8B CA E8"e, ReadProcessMemoryWinAPI);
		verify(ScanResult);
		const auto p = PatternScan::GetJumpAddress(ScanResult + 0x3, ReadProcessMemoryDBVM);
		verify(p);
		return decltype(UserValidateHwnd)(reinterpret_cast<tagWND * (*)(HWND hWnd)>(p));
	}();

private:
	const uint32_t OffsetProp = [&] {
		const uintptr_t pNtUserGetProp = GetKernelProcAddressVerified("win32kfull.sys"e, "NtUserGetProp"e);
		const uintptr_t ScanResult = PatternScan::Range(pNtUserGetProp, 0x100, "48 8B ? ? ? 00 00 48 FF 15"e, ReadProcessMemoryDBVM);
		verify(ScanResult);

		ReadProcessMemoryDBVM(ScanResult + 0x3, (void*)&OffsetProp, sizeof(OffsetProp));
		verify(OffsetProp);
		return OffsetProp;
	}();

	const uintptr_t pPsProcessType = [&] {
		const uintptr_t ppPsProcessType = GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsProcessType"e);
		ReadProcessMemoryDBVM(ppPsProcessType, (void*)&pPsProcessType, sizeof(pPsProcessType));
		verify(pPsProcessType);
		return pPsProcessType;
	}();
	
	uintptr_t GetCallbackEntryItemWithAltitude(const wchar_t* wAltitude) const {
		OBJECT_TYPE PsProcessType;
		if (!ReadProcessMemoryDBVM(pPsProcessType, &PsProcessType, sizeof(PsProcessType)))
			return 0;

		const uintptr_t pStartCallback = (uintptr_t)PsProcessType.CallbackList.Flink;
		uintptr_t pCallback = pStartCallback;
		do {
			CALLBACK_ENTRY_ITEM CallbackEntryItem;
			if (!ReadProcessMemoryDBVM(pCallback, &CallbackEntryItem, sizeof(CallbackEntryItem)))
				break;

			CALLBACK_ENTRY CallBackEntry;
			if (!ReadProcessMemoryDBVM((uintptr_t)CallbackEntryItem.CallbackEntry, &CallBackEntry, sizeof(CallBackEntry)))
				break;

			wchar_t wCallbackAltitude[0x100] = { 0 };
			if (!ReadProcessMemoryDBVM((uintptr_t)CallBackEntry.Altitude.Buffer, &wCallbackAltitude,
				std::min(sizeof(wCallbackAltitude), (size_t)CallBackEntry.Altitude.Length)))
				break;

			if (wcscmp(wCallbackAltitude, wAltitude) == 0)
				return pCallback;

			pCallback = (uintptr_t)CallbackEntryItem.EntryItemList.Flink;
		} while (pCallback != pStartCallback);

		return 0;
	}

	PPPROP GetPPProp(PWND pWnd) const {
		return (PPPROP)(pWnd + OffsetProp);
	}

	PPROP GetPProp(PWND pWnd) const {
		PPROP pProp = 0;
		ReadProcessMemoryDBVM((uintptr_t)GetPPProp(pWnd), &pProp, sizeof(pProp));
		return pProp;
	}

public:
	HANDLE UserGetProp(HWND hWnd, ATOM nAtom, DWORD dwFlag = 1) const {
		PWND pWnd = ValidateHwnd(hWnd);
		if (!pWnd)
			return 0;

		PPROP pProp = GetPProp(pWnd);
		if (!pProp)
			return 0;

		return RealGetProp(pProp, nAtom, dwFlag);
	}

	bool UserSetProp(HWND hWnd, ATOM nAtom, HANDLE hValue, DWORD dwFlag = 5) const {
		PWND pWnd = ValidateHwnd(hWnd);
		if (!pWnd)
			return 0;

		return RealInternalSetProp(GetPPProp(pWnd), nAtom, hValue, dwFlag);
	}

	bool PsSetProcessWin32ProcessWrapper(HWND hWnd, auto f) const {
		DWORD dwPid;
		if (!GetWindowThreadProcessId(hWnd, &dwPid))
			return false;

		const PVOID CurrentProcess = (PVOID)GetEPROCESS(GetCurrentProcessId());
		if (!CurrentProcess)
			return false;

		const PVOID ProcessHWND = (PVOID)GetEPROCESS(dwPid);
		if (!ProcessHWND)
			return false;

		const PVOID Win32ProcessCurrent = PsGetProcessWin32Process(CurrentProcess);
		const PVOID Win32ProcessHWND = PsGetProcessWin32Process(ProcessHWND);
		PsSetProcessWin32Process(CurrentProcess, 0, Win32ProcessCurrent);
		PsSetProcessWin32Process(CurrentProcess, Win32ProcessHWND, 0);
		f();
		PsSetProcessWin32Process(CurrentProcess, 0, Win32ProcessHWND);
		PsSetProcessWin32Process(CurrentProcess, Win32ProcessCurrent, 0);

		const bool bWin32ProcessRollbacked = (PsGetProcessWin32Process(CurrentProcess) == Win32ProcessCurrent);
		verify(bWin32ProcessRollbacked);
		return true;
	}
};
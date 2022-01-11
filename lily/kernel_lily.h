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

	const KernelCall<PVOID(PVOID Process)> PsGetProcessWin32Process = [&]{
		const auto p = GetKernelProcAddress("ntoskrnl.exe"e, "PsGetProcessWin32Process"e);
		verify(p);
		return decltype(PsGetProcessWin32Process)(p, *this);
	}();
	const KernelCall<NTSTATUS(PVOID Process, PVOID Win32Process, PVOID PrevWin32Process)> PsSetProcessWin32Process = [&] {
		const auto p = GetKernelProcAddress("ntoskrnl.exe"e, "PsSetProcessWin32Process"e);
		verify(p);
		return decltype(PsSetProcessWin32Process)(p, *this);
	}();
	const KernelCall<PWND(HWND hWnd)> ValidateHwnd = [&] {
		const auto p = GetKernelProcAddress("win32kbase.sys"e, "ValidateHwnd"e);
		verify(p);
		return decltype(ValidateHwnd)(p, *this);
	}();
	const KernelCall<PVOID(DWORD PoolType, SIZE_T NumberOfBytes)> ExAllocatePool = [&] {
		const auto p = GetKernelProcAddress("ntoskrnl.exe"e, "ExAllocatePool"e);
		verify(p);
		return decltype(ExAllocatePool)(p, *this);
	}();
	const KernelCall<ATOM(PCWSTR AtomName)> UserFindAtom = [&] {
		const auto p = GetKernelProcAddress("win32kbase.sys"e, "UserFindAtom"e);
		verify(p);
		return decltype(UserFindAtom)(p, *this);
	}();
	const KernelCall<HANDLE(PPROP pProp, ATOM nAtom, DWORD dwFlag)> RealGetProp = [&] {
		const auto p = GetKernelProcAddress("win32kbase.sys"e, "RealGetProp"e);
		verify(p);
		return decltype(RealGetProp)(p, *this);
	}();
	const KernelCall<BOOL(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag)> RealInternalSetProp = [&] {
		const auto p = GetKernelProcAddress("win32kbase.sys"e, "RealInternalSetProp"e);
		verify(p);
		return decltype(RealInternalSetProp)(p, *this);
	}();
	const SafeCall<tagWND* (HWND hWnd)> UserValidateHwnd = [&] {
		const uintptr_t ScanResult = PatternScan::Range((uintptr_t)IsChild, 0x30, "48 8B CA E8"e, RPM_dbvm);
		verify(ScanResult);
		const auto p = PatternScan::GetJumpAddress(ScanResult + 0x3, RPM_dbvm);
		verify(p);
		return p;
	}();

private:
	const uint32_t OffsetProp = [&] {
		const uintptr_t pNtUserGetProp = GetKernelProcAddress("win32kfull.sys"e, "NtUserGetProp"e);
		verify(pNtUserGetProp);

		const uintptr_t ScanResult = PatternScan::Range(pNtUserGetProp, 0x100, "48 8B ? ? ? 00 00 48 FF 15"e, RPM_dbvm);
		verify(ScanResult);

		RPM_dbvm(ScanResult + 0x3, (void*)&OffsetProp, sizeof(OffsetProp));
		verify(OffsetProp);
		return OffsetProp;
	}();

	const uintptr_t pPsProcessType = [&] {
		const uintptr_t ppPsProcessType = GetKernelProcAddress("ntoskrnl.exe"e, "PsProcessType"e);
		verify(ppPsProcessType);

		RPM_dbvm(ppPsProcessType, (void*)&pPsProcessType, sizeof(pPsProcessType));
		verify(pPsProcessType);
		return pPsProcessType;
	}();
	
	uintptr_t GetCallbackEntryItemWithAltitude(const wchar_t* wAltitude) const {
		OBJECT_TYPE PsProcessType;
		if (!RPM_dbvm(pPsProcessType, &PsProcessType, sizeof(PsProcessType)))
			return 0;

		const uintptr_t pStartCallback = (uintptr_t)PsProcessType.CallbackList.Flink;
		uintptr_t pCallback = pStartCallback;
		do {
			CALLBACK_ENTRY_ITEM CallbackEntryItem;
			if (!RPM_dbvm(pCallback, &CallbackEntryItem, sizeof(CallbackEntryItem)))
				break;

			CALLBACK_ENTRY CallBackEntry;
			if (!RPM_dbvm((uintptr_t)CallbackEntryItem.CallbackEntry, &CallBackEntry, sizeof(CallBackEntry)))
				break;

			wchar_t wCallbackAltitude[0x100] = { 0 };
			if (!RPM_dbvm((uintptr_t)CallBackEntry.Altitude.Buffer, &wCallbackAltitude,
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
		RPM_dbvm((uintptr_t)GetPPProp(pWnd), &pProp, sizeof(pProp));
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
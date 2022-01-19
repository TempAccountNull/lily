#pragma once
#include "common/kernel.h"
#include "common/obcallback.h"

#define ALTITUDE_BE L"363220"e

class EmptyWindow {
public:
	const HWND hWnd = CreateWindowExA(
		0, "Static"e, ""e, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, GetModuleHandleA(0), 0);
	operator HWND() const { return hWnd; }
	~EmptyWindow() { DestroyWindow(hWnd); }
};

class KernelLily : public Kernel {
public:
	DefBaseClass(tagWND_USER,
		MemberAtOffset(DWORD, dwExStyle, 0x18)
		MemberAtOffset(RECT, rcWindow, 0x58)
		MemberAtOffset(RECT, rcClient, 0x68)
		MemberAtOffset(WORD, FNID, 0x2A)
	,)
	class PROP {};
	class ETHREAD {};

	class tagWND {
	private:
		constexpr static size_t Offset_OwningThread = 0x10;
	public:
		PROP** GetPPProp(const KernelLily& kernel) const {
			return (PROP**)((uintptr_t)this + kernel.OffsetProp);
		}
		PROP* GetPProp(const KernelLily& kernel) const {
			PROP* pProp = 0;
			if (!kernel.ReadProcessMemoryDBVM((uintptr_t)GetPPProp(kernel), &pProp, sizeof(pProp)))
				return 0;
			return pProp;
		}
		ETHREAD* GetEThread(const KernelLily& kernel) const {
			ETHREAD* pThread = 0;
			if (!kernel.ReadProcessMemoryDBVM(((uintptr_t)this + Offset_OwningThread), &pThread, sizeof(pThread)))
				return 0;
			return pThread;
		}
		bool SetEThread(const KernelLily& kernel, ETHREAD* pThread) {
			return kernel.WriteProcessMemoryDBVM(((uintptr_t)this + Offset_OwningThread), &pThread, sizeof(pThread));
		}
	};

	KernelLily(const DBVM& dbvm) : Kernel(dbvm) {}

	const KernelFunction<PVOID(PVOID Process)> PsGetProcessWin32Process = {
		reinterpret_cast<PVOID(*)(PVOID Process)>
		(GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsGetProcessWin32Process"e)), *this };

	const KernelFunction<NTSTATUS(PVOID Process, PVOID Win32Process, PVOID PrevWin32Process)> PsSetProcessWin32Process = {
		reinterpret_cast<NTSTATUS(*)(PVOID Process, PVOID Win32Process, PVOID PrevWin32Process)>
		(GetKernelProcAddressVerified("ntoskrnl.exe"e, "PsSetProcessWin32Process"e)), *this };

	const KernelFunction<tagWND* (HWND hWnd)> ValidateHwnd = {
		reinterpret_cast<tagWND * (*)(HWND hWnd)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "ValidateHwnd"e)), *this };

	const KernelFunction<PVOID(DWORD PoolType, SIZE_T NumberOfBytes)> ExAllocatePool = {
		reinterpret_cast<PVOID(*)(DWORD PoolType, SIZE_T NumberOfBytes)>
		(GetKernelProcAddressVerified("ntoskrnl.exe"e, "ExAllocatePool"e)), *this };

	const KernelFunction<ATOM(PCWSTR AtomName)> UserFindAtom = {
		reinterpret_cast<ATOM(*)(PCWSTR AtomName)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "UserFindAtom"e)), *this };

	const KernelFunction<HANDLE(PROP* pProp, ATOM nAtom, DWORD dwFlag)> RealGetProp = {
		reinterpret_cast<HANDLE(*)(PROP * pProp, ATOM nAtom, DWORD dwFlag)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "RealGetProp"e)), *this };

	const KernelFunction<BOOL(PROP* pProp, ATOM nAtom, DWORD dwFlag)> RealInternalRemoveProp = {
		reinterpret_cast<BOOL(*)(PROP * pProp, ATOM nAtom, DWORD dwFlag)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "RealInternalRemoveProp"e)), *this };

	const KernelFunction<BOOL(PROP** pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag)> RealInternalSetProp = {
		reinterpret_cast<BOOL(*)(PROP * *pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag)>
		(GetKernelProcAddressVerified("win32kbase.sys"e, "RealInternalSetProp"e)), *this };

	const KernelFunction<NTSTATUS(tagWND* pWnd, BOOL topmopst)> EditionNotifyDwmForSystemVisualDestruction = {
		reinterpret_cast<NTSTATUS(*)(tagWND * pWnd, BOOL topmopst)>
		(GetKernelProcAddressVerified("win32kfull.sys"e, "EditionNotifyDwmForSystemVisualDestruction"e)), *this };

	const UserFunction<tagWND_USER* (HWND hWnd)> UserValidateHwnd = [&] {
		const uintptr_t ScanResult = PatternScan::Range((uintptr_t)IsChild, 0x30, "48 8B CA E8"e, ReadProcessMemoryWinAPI);
		verify(ScanResult);
		const auto p = PatternScan::GetJumpAddress(ScanResult + 0x3, ReadProcessMemoryDBVM);
		verify(p);
		return decltype(UserValidateHwnd)(reinterpret_cast<tagWND_USER * (*)(HWND hWnd)>(p));
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

public:
	ATOM UserFindAtomVerified(PCWSTR AtomName) const {
		const ATOM atom = UserFindAtom(AtomName);
		if (!atom)
			error(AtomName, L"Atom"e);
		return atom;
	}

	HANDLE KernelGetProp(HWND hWnd, ATOM nAtom, DWORD dwFlag = 1) const {
		tagWND* pWnd = ValidateHwnd(hWnd);
		if (!pWnd)
			return 0;

		PROP* pProp = pWnd->GetPProp(*this);
		if (!pProp)
			return 0;

		return RealGetProp(pProp, nAtom, dwFlag);
	}

	bool KernelRemoveProp(HWND hWnd, ATOM nAtom, DWORD dwFlag = 1) const {
		tagWND* pWnd = ValidateHwnd(hWnd);
		if (!pWnd)
			return 0;

		PROP* pProp = pWnd->GetPProp(*this);
		if (!pProp)
			return 0;

		return RealInternalRemoveProp(pProp, nAtom, dwFlag);
	}

	bool KernelSetProp(HWND hWnd, ATOM nAtom, HANDLE hValue, DWORD dwFlag = 5) const {
		tagWND* pWnd = ValidateHwnd(hWnd);
		if (!pWnd)
			return 0;

		return RealInternalSetProp(pWnd->GetPPProp(*this), nAtom, hValue, dwFlag);
	}

	bool SetOwningThreadWrapper(HWND hWnd, auto f) const {
		const EmptyWindow hWndFrom;
		tagWND* const pWndFrom = ValidateHwnd(hWndFrom);
		if (!pWndFrom)
			return false;

		tagWND* const pWndTo = ValidateHwnd(hWnd);
		if (!pWndTo)
			return false;

		ETHREAD* const pThreadFrom = pWndFrom->GetEThread(*this);
		if (!pThreadFrom)
			return false;

		ETHREAD* const pThreadTo = pWndTo->GetEThread(*this);
		if (!pThreadTo)
			return false;

		if (!pWndTo->SetEThread(*this, pThreadFrom))
			return false;

		f();
		return pWndTo->SetEThread(*this, pThreadTo);
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
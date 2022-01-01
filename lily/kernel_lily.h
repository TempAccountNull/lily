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

using tUserValidateHwnd = tagWND*(*)(HWND hWnd);
using tValidateHwnd = PWND(*)(HWND hWnd);
using tUserFindAtom = ATOM(*)(PCWSTR AtomName);
using tRealGetProp = HANDLE(*)(PPROP pProp, ATOM nAtom, DWORD dwFlag);
using tRealInternalSetProp = BOOL(*)(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag);
using tExAllocatePool = PVOID (*)(DWORD PoolType, SIZE_T NumberOfBytes);

class KernelLily : public Kernel {
private:
	uintptr_t pPsProcessType = 0;

	tValidateHwnd pValidateHwnd = 0;
	tUserFindAtom pUserFindAtom = 0;
	tRealGetProp pRealGetProp = 0;
	tRealInternalSetProp pRealInternalSetProp = 0;
	uint32_t OffsetProp = 0;

	tExAllocatePool pExAllocatePool = 0;

	tUserValidateHwnd pUserValidateHwnd = 0;

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

	PWND ValidateHwnd(HWND hWnd) const {
		PWND Result;
		KernelExecute([&] { Result = SafeCall(pValidateHwnd, hWnd); });
		return Result;
	}

	HANDLE RealGetProp(PPROP pProp, ATOM nAtom, DWORD dwFlag) const {
		if (!pProp)
			return 0;
		HANDLE Result;
		KernelExecute([&] { Result = SafeCall(pRealGetProp, pProp, nAtom, dwFlag); });
		return Result;
	}

	bool RealInternalSetProp(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag) const {
		if (!pProp)
			return false;
		bool Result;
		KernelExecute([&] { Result = SafeCall(pRealInternalSetProp, pProp, nAtom, hValue, dwFlag); });
		return Result;
	}

public:
	tagWND* UserValidateHwnd(HWND hWnd) const { return SafeCall(pUserValidateHwnd, hWnd); }

	ATOM UserFindAtom(PCWSTR AtomName) const {
		ATOM Result;
		KernelExecute([&] { Result = SafeCall(pUserFindAtom, AtomName); });
		return Result;
	}

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

	void* ExAllocatePool(size_t NumberOfBytes) const {
		void* Result;
		KernelExecute([&] { Result = SafeCall(pExAllocatePool, 0, NumberOfBytes); });
		return Result;
	}

	KernelLily(const DBVM& dbvm) : Kernel(dbvm) {
		uintptr_t ScanResult = 0;

		pExAllocatePool = (tExAllocatePool)GetKernelProcAddress("ntoskrnl.exe"e, "ExAllocatePool"e);
		verify(pExAllocatePool);

		pValidateHwnd = (tValidateHwnd)GetKernelProcAddress("win32kbase.sys"e, "ValidateHwnd"e);
		verify(pValidateHwnd);
		pUserFindAtom = (tUserFindAtom)GetKernelProcAddress("win32kbase.sys"e, "UserFindAtom"e);
		verify(pUserFindAtom);
		pRealGetProp = (tRealGetProp)GetKernelProcAddress("win32kbase.sys"e, "RealGetProp"e);
		verify(pRealGetProp);
		pRealInternalSetProp = (tRealInternalSetProp)GetKernelProcAddress("win32kbase.sys"e, "RealInternalSetProp"e);
		verify(pRealInternalSetProp);

		uintptr_t pNtUserGetProp = GetKernelProcAddress("win32kfull.sys"e, "NtUserGetProp"e);
		verify(pNtUserGetProp);

		ScanResult = PatternScan::Range(pNtUserGetProp, 0x100, "48 8B ? ? ? 00 00 48 FF 15"e, RPM_dbvm);
		verify(ScanResult);

		RPM_dbvm(ScanResult + 0x3, &OffsetProp, sizeof(OffsetProp));
		verify(OffsetProp);

		uintptr_t ppPsProcessType = GetKernelProcAddress("ntoskrnl.exe"e, "PsProcessType");
		verify(ppPsProcessType);

		RPM_dbvm(ppPsProcessType, &pPsProcessType, sizeof(pPsProcessType));
		verify(pPsProcessType);

		ScanResult = PatternScan::Range((uintptr_t)IsChild, 0x30, "48 8B CA E8", RPM_dbvm);
		verify(ScanResult);

		pUserValidateHwnd = (tUserValidateHwnd)PatternScan::GetJumpAddress(ScanResult + 0x3, RPM_dbvm);
		verify(pUserValidateHwnd);
	}
};
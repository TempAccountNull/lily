#pragma once
#include "kernel.h"
#include "obcallback.h"

#define ALTITUDE_BE L"363220"e
#define DISPAFFINITY_PROP_NAME L"SysDispAffinity"e
#define LAYER_PROP_NAME L"SysLayer"e

DefBaseClass(tagWND,
	MemberAtOffset(DWORD, dwExStyle, 0x18)
,)

typedef struct {}*PPPROP;
typedef struct {}*PPROP;
typedef struct {}*PWND;

using tUserValidateHwnd = tagWND(*)(HWND hWnd);

using tValidateHwnd = PWND(*)(HWND hWnd);
using tUserFindAtom = ATOM(*)(PCWSTR AtomName);
using tRealGetProp = HANDLE(*)(PPROP pProp, ATOM nAtom, DWORD dwFlag);
using tRealInternalSetProp = BOOL(*)(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag);

class KernelLily : public Kernel {
private:
	uintptr_t pKernelFuncRet0 = 0;
	uintptr_t pPsProcessType = 0;

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

	tValidateHwnd pValidateHwnd = 0;
	tUserFindAtom pUserFindAtom = 0;
	tRealGetProp pRealGetProp = 0;
	tRealInternalSetProp pRealInternalSetProp = 0;
	uint32_t OffsetProp = 0;

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
		KernelExecute([&] { Result = KernelCall(pValidateHwnd, hWnd); });
		return Result;
	}

	ATOM UserFindAtom(PCWSTR AtomName) const {
		ATOM Result;
		KernelExecute([&] { Result = KernelCall(pUserFindAtom, AtomName); });
		return Result;
	}

	HANDLE RealGetProp(PPROP pProp, ATOM nAtom, DWORD dwFlag) const {
		if (!pProp)
			return 0;
		HANDLE Result;
		KernelExecute([&] { Result = KernelCall(pRealGetProp, pProp, nAtom, dwFlag); });
		return Result;
	}

	bool RealInternalSetProp(PPPROP pProp, ATOM nAtom, HANDLE hValue, DWORD dwFlag) const {
		if (!pProp)
			return false;
		bool Result;
		KernelExecute([&] { Result = KernelCall(pRealInternalSetProp, pProp, nAtom, hValue, dwFlag); });
		return Result;
	}

public:
	tUserValidateHwnd UserValidateHwnd = 0;

	ATOM atomDispAffinity;
	ATOM atomLayer;

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

	KernelLily(const DBVM& dbvm) : Kernel(dbvm) {
		uintptr_t ScanResult = 0;

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

		atomLayer = UserFindAtom(LAYER_PROP_NAME);
		verify(atomLayer);
		atomDispAffinity = UserFindAtom(DISPAFFINITY_PROP_NAME);
		verify(atomDispAffinity);

		uintptr_t hNtoskrnl = GetKernelModuleAddress("ntoskrnl.exe"e);
		pKernelFuncRet0 = PatternScan::Module(hNtoskrnl, ".text", "33 C0 C3", RPM_dbvm);
		verify(pKernelFuncRet0);

		uintptr_t ppPsProcessType = GetKernelProcAddress("ntoskrnl.exe"e, "PsProcessType");
		verify(ppPsProcessType);

		RPM_dbvm(ppPsProcessType, &pPsProcessType, sizeof(pPsProcessType));
		verify(pPsProcessType);

		ScanResult = PatternScan::Range((uintptr_t)IsChild, 0x30, "48 8B CA E8", RPM_dbvm);
		verify(ScanResult);

		UserValidateHwnd = (tUserValidateHwnd)PatternScan::GetJumpAddress(ScanResult + 0x3, RPM_dbvm);
		verify(UserValidateHwnd);
	}

	/*
	bool BlockSetProp(auto f) const {
		uint8_t CodePatch[] = { 0x48, 0x31, 0xC0, 0xFF, 0xC0, 0xC3 };
		uint8_t CodeOriginal[sizeof(CodePatch)];

		if (!RPM_dbvm((uintptr_t)pRealInternalSetProp, CodeOriginal, sizeof(CodePatch)))
			return false;

		if (!WPM_dbvm((uintptr_t)pRealInternalSetProp, CodePatch, sizeof(CodePatch)))
			return false;

		f();

		while (!WPM_dbvm((uintptr_t)pRealInternalSetProp, CodeOriginal, sizeof(CodePatch)));
		return true;
	}

	bool BlockCallback(const wchar_t* wAltitude, auto f) const {
		uintptr_t BePreCallback = GetCallbackEntryItemWithAltitude(wAltitude);
		if (!BePreCallback)
			return false;

		CALLBACK_ENTRY_ITEM CallbackEntryItem;
		if (!RPM_dbvm(BePreCallback, &CallbackEntryItem, sizeof(CallbackEntryItem)))
			return false;

		WPM_dbvm(BePreCallback + offsetof(CALLBACK_ENTRY_ITEM, PreOperation), &pKernelFuncRet0, sizeof(pKernelFuncRet0));
		f();
		WPM_dbvm(BePreCallback + offsetof(CALLBACK_ENTRY_ITEM, PreOperation), &CallbackEntryItem.PreOperation, sizeof(pKernelFuncRet0));
		return true;
	}
	*/
};
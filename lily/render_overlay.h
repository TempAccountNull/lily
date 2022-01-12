#pragma once
#include "render.h"

#include <Shlobj.h>
#include <dwmapi.h>
#include <d3d9.h>
#include "kernel_lily.h"

class RenderOverlay : public Render {
private:
	const KernelLily& kernel;
	const ATOM atomDispAffinity;
	HWND hESPWnd;
	mutable HWND hAttachWnd = 0;
	HANDLE hJob;

	bool IsValid() const {
		const bool bResult = [&] {
			if (!IsWindow(hESPWnd))
				return false;

			DWORD dwExStyle = GetWindowLongA(hESPWnd, GWL_EXSTYLE);
			if (dwExStyle & WS_EX_TRANSPARENT)
				return false;
			if (dwExStyle & WS_EX_LAYERED)
				return false;
			if (dwExStyle & WS_EX_TOPMOST)
				return false;

			COLORREF crKey;
			BYTE bAlpha;
			DWORD dwFlags;
			if (GetLayeredWindowAttributes(hESPWnd, &crKey, &bAlpha, &dwFlags))
				return false;

			DWORD dwNewAffinity = WDA_MONITOR;
			if (!GetWindowDisplayAffinity(hESPWnd, &dwNewAffinity))
				return false;
			if (dwNewAffinity != WDA_NONE)
				return false;

			DWORD dwAttribute = 0;
			DwmGetWindowAttribute(hESPWnd, DWMWA_CLOAKED, &dwAttribute, sizeof(dwAttribute));
			if (dwAttribute)
				return false;

			return true;
		}();

		if (!bResult)
			CloseHandle(hJob);

		return bResult;
	}

	bool MakeWindowIgnoreHitTest() const {
		const tagWND* pWnd = kernel.UserValidateHwnd(hESPWnd);
		if (!pWnd)
			return false;

		const RECT rcWindow = { 0, 0, 1, 1 };
		return kernel.WriteProcessMemoryDBVM((uintptr_t)&pWnd->rcWindow, &rcWindow, sizeof(rcWindow));
	}

	bool RemoteSetWindowDisplayAffinityBypassed(HWND hWnd) {
		bool bResult = false;
		if (!kernel.PsSetProcessWin32ProcessWrapper(hWnd, [&] {
			bResult = SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
			})) return false;

		if (!bResult)
			return false;

		if (!kernel.UserSetProp(hWnd, atomDispAffinity, HANDLE(WDA_NONE)))
			return false;

		return true;
	}

	bool InitJob() {
		hJob = CreateJobObjectA(0, 0);
		if (!hJob)
			return false;

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION JobExLimitInfo = { .BasicLimitInformation = {.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE } };
		if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &JobExLimitInfo, sizeof(JobExLimitInfo)))
			return false;

		return true;
	}

	bool AddRenderWindowToJob() const {
		const HANDLE hProcess = [&] {
			DWORD dwPid = 0;
			if (!GetWindowThreadProcessId(hESPWnd, &dwPid))
				return (HANDLE)0;

			return OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		}();

		if (!hProcess) {
			PostMessageA(hESPWnd, WM_CLOSE, 0, 0);
			return false;
		}

		const bool bResult = AssignProcessToJobObject(hJob, hProcess);
		if (!bResult)
			TerminateProcess(hProcess, 0);

		CloseHandle(hProcess);
		return bResult;
	}

	bool OpenRestreamChat() {
		hESPWnd = FindWindowA("Chrome_WidgetWin_1"e, "Restream Chat"e);
		if (hESPWnd)
			return true;

		char szPath[MAX_PATH];
		if (!SHGetSpecialFolderPathA(0, szPath, CSIDL_LOCAL_APPDATA, 0))
			return false;

		strcat(szPath, "\\Programs\\restream-chat\\Restream Chat.exe"e);

		CreateProcessCMD(szPath);
		Sleep(2000);

		hESPWnd = FindWindowA("Chrome_WidgetWin_1"e, "Restream Chat"e);
		if (!hESPWnd)
			return false;

		while (!IsWindowVisible(hESPWnd));
		return true;
	}

	void UpdateWindowPos(HWND hWndOver) const {
		HWND hWndInsertAfter = 0;
		UINT uFlags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW;

		[&] {
			if (hWndOver == 0 || hWndOver == hESPWnd)
				return;

			for (HWND hWnd = GetWindow(hESPWnd, GW_HWNDFIRST); hWnd; hWnd = GetWindow(hWnd, GW_HWNDNEXT)) {
				if (GetWindowLongA(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
					continue;

				if (hWnd == hESPWnd)
					break;

				if (hWnd != hWndOver)
					continue;

				hWndInsertAfter = HWND_TOP;
				uFlags &= ~SWP_NOZORDER;

				const HWND hForeWnd = GetForegroundWindow();
				if (hAttachWnd == hForeWnd)
					return;
				if (AttachThreadInput(GetWindowThreadProcessId(hESPWnd, 0), GetWindowThreadProcessId(hForeWnd, 0), TRUE))
					hAttachWnd = hForeWnd;
				return;
			}
		}();
		
		SetWindowPos(hESPWnd, hWndInsertAfter, 0, 0, ScreenWidth, ScreenHeight, uFlags);
	}

	void UpdateWindow(HWND hWndOver) const {
		const HDC hDC = GetDC(hESPWnd);
		if (!hDC)
			return;

		bool IsVisible = PtVisible(hDC, 5, 5);
		ReleaseDC(hESPWnd, hDC);
		if (IsVisible)
			return;

		UpdateWindowPos(hWndOver);
		MakeWindowIgnoreHitTest();
	}

	bool InitOverlay() {
		if (!OpenRestreamChat())
			return false;

		if (!AddRenderWindowToJob())
			return false;

		bool bResult = [&] {
			//SetWindowDisplayAffinity fail when parent & child windows has WS_EX_LAYERED and WS_EX_NOREDIRECTIONBITMAP
			//Close "Intermediate D3D Window" for use SetWindowDisplayAffinity (WS_EX_NOREDIRECTIONBITMAP cannot be removed)
			HWND hChild = FindWindowExA(hESPWnd, 0, "Intermediate D3D Window"e, 0);
			if (hChild)
				SendMessageA(hChild, WM_CLOSE, 0, 0);

			SetWindowLongA(hESPWnd, GWL_EXSTYLE, GetWindowLongA(hESPWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED & ~WS_EX_TRANSPARENT);
			if (!RemoteSetWindowDisplayAffinityBypassed(hESPWnd))
				return false;

			//Move window to re-create "Intermediate D3D Window"
			SetWindowPos(hESPWnd, HWND_BOTTOM, 1, 1, ScreenWidth - 1, ScreenHeight - 1, SWP_SHOWWINDOW);
			//Wait until "Intermediate D3D Window" created
			while (!(hChild = FindWindowExA(hESPWnd, 0, "Intermediate D3D Window"e, 0)));
			MoveWindow(hESPWnd, 0, 0, ScreenWidth, ScreenHeight, false);

			BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };
			UpdateLayeredWindow(hChild, 0, 0, 0, 0, 0, 0, &blendPixelFunction, ULW_ALPHA);

			if (!MakeWindowIgnoreHitTest())
				return false;

			return true;
		}();

		if (!bResult)
			CloseHandle(hJob);

		return bResult;
	}

public:
	RenderOverlay(IDirect3DDevice9Ex* pDirect3DDevice9Ex, const KernelLily& kernel, int ScreenWidth, int ScreenHeight) :
		Render(pDirect3DDevice9Ex, ScreenWidth, ScreenHeight), kernel(kernel),
		atomDispAffinity(kernel.UserFindAtom(L"SysDispAffinity"e)) {
		verify(atomDispAffinity);
		verify(InitJob());
		verify(InitOverlay());
		verify(IsValid());
		Clear();
	}

	virtual void Present(HWND hWnd) const {
		verify(IsValid());
		UpdateWindow(hWnd);
		pDirect3DDevice9Ex->Present(0, 0, hESPWnd, 0);
	}
};
#pragma once
#include "render.h"
#include <Shlobj.h>

class RestreamChatRender : public Render {
private:
	const KernelLily& kernel;
	int ScreenWidth, ScreenHeight;
	HWND hESPWnd;
	DWORD PidSaved = 0;
	PhysicalAddress PhysicalAddressUser32NotTriggerdCOW;

	tagREDIRECT* pRedirectInfo = 0;
	HBITMAP hBitmapSaved = 0;

	bool CheckBypassed() const {
		auto IsBypassed = [&] {
			if (GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT)
				return false;

			DWORD dwNewAffinity = 1;
			if (!GetWindowDisplayAffinity(hESPWnd, &dwNewAffinity))
				return false;
			if (dwNewAffinity != 0)
				return false;

			COLORREF crKey;
			BYTE bAlpha;
			DWORD dwFlags;
			if (GetLayeredWindowAttributes(hESPWnd, &crKey, &bAlpha, &dwFlags))
				return false;

			return true;
		}();

		if (IsBypassed)
			return true;

		TerminateOverlayProcess();
		return false;
	}

	void TerminateOverlayProcess() const {
		DWORD Pid = 0;
		DWORD Tid = GetWindowThreadProcessId(hESPWnd, &Pid);

		HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, Tid);
		if (hThread)
			TerminateThread(hThread, 0);

		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, Pid);
		if (hProcess)
			TerminateProcess(hProcess, 0);

		PostMessageA(hESPWnd, WM_CLOSE, 0, 0);
	}

	static bool RemoteSetWindowDisplayAffinity(HWND hWnd, DWORD dwAffinity) {
		DWORD Pid;
		if (GetWindowThreadProcessId(hWnd, &Pid) == 0)
			return false;

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, Pid);
		if (hProcess == 0)
			return false;

		void* pShellCode = VirtualAllocEx(hProcess, 0, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		uint8_t ShellCode[] = {
			0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0xFF, 0xD0, 0xC3
		};

		*(uintptr_t*)(ShellCode + 0x2) = (uintptr_t)hWnd;
		*(uintptr_t*)(ShellCode + 0xC) = (uintptr_t)dwAffinity;
		*(uintptr_t*)(ShellCode + 0x16) = (uintptr_t)SetWindowDisplayAffinity;

		size_t Written;
		WriteProcessMemory(hProcess, pShellCode, ShellCode, sizeof(ShellCode), &Written);
		HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, LPTHREAD_START_ROUTINE(pShellCode), 0, 0, 0);
		WaitForSingleObject(hThread, INFINITE);
		VirtualFreeEx(hProcess, pShellCode, 0x1000, MEM_RELEASE);

		DWORD dwExitCode = 0;
		GetExitCodeThread(hThread, &dwExitCode);

		CloseHandle(hThread);
		CloseHandle(hProcess);

		return dwExitCode == 1;
	}

	bool RemoteSetWindowDisplayAffinityBypassed(HWND hWnd) {
		if (!IsWindowVisible(hWnd))
			return false;

		if (!RemoteSetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE))
			return false;

		if (!kernel.UserSetProp(hWnd, kernel.atomDispAffinity, HANDLE(WDA_NONE)))
			return false;

		return true;
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

	bool InitOverlay() {
		if (!OpenRestreamChat())
			return false;

		pRedirectInfo = (tagREDIRECT*)kernel.UserGetProp(hESPWnd, kernel.atomLayer);
		if (!pRedirectInfo)
			return false;

		//SetWindowDisplayAffinity fail when parent & child windows has WS_EX_LAYERED and WS_EX_NOREDIRECTIONBITMAP
		//Close "Intermediate D3D Window" for use SetWindowDisplayAffinity (WS_EX_NOREDIRECTIONBITMAP cannot be removed)
		HWND hChild = FindWindowExA(hESPWnd, 0, "Intermediate D3D Window"e, 0);
		if (hChild)
			SendMessageA(hChild, WM_CLOSE, 0, 0);

		SetWindowLongPtrA(hESPWnd, GWL_EXSTYLE, GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED & ~WS_EX_TRANSPARENT);
		if (!RemoteSetWindowDisplayAffinityBypassed(hESPWnd))
			return false;
		SetWindowLongPtrA(hESPWnd, GWL_EXSTYLE, GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

		//Move window to re-create "Intermediate D3D Window"
		ShowWindow(hESPWnd, SW_NORMAL);
		MoveWindow(hESPWnd, 0, 0, 0, 0, false);
		//Wait until "Intermediate D3D Window" created
		while (!(hChild = FindWindowExA(hESPWnd, 0, "Intermediate D3D Window"e, 0)));
		MoveWindow(hESPWnd, 0, 0, ScreenWidth, ScreenHeight, false);

		BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };
		UpdateLayeredWindow(hChild, 0, 0, 0, 0, 0, 0, &blendPixelFunction, ULW_ALPHA);
		return true;
	}

	bool UpdateLayeredWindowAttributes() {
		tagREDIRECT RedirectInfo;
		if (!kernel.RPM_dbvm((uintptr_t)pRedirectInfo, &RedirectInfo, sizeof(RedirectInfo)))
			return false;

		if (RedirectInfo.hbm == hBitmapSaved)
			return true;
		
		SetLayeredWindowAttributes(hESPWnd, COLOR_CLEAR, 0, LWA_COLORKEY);

		UINT uFlags = 0;
		if (!kernel.WPM_dbvm((uintptr_t)pRedirectInfo + offsetof(tagREDIRECT, uFlags), &uFlags, sizeof(uFlags)))
			return false;

		hBitmapSaved = RedirectInfo.hbm;
		return true;
	}

	void UpdateWindowPos(HWND hWndOver) const {
		MoveWindow(hESPWnd, 0, 0, ScreenWidth, ScreenHeight, false);
		ShowWindow(hESPWnd, SW_NORMAL);

		if (hWndOver == 0 || hWndOver == hESPWnd)
			return;

		for (HWND hWnd = GetWindow(hESPWnd, GW_HWNDFIRST); hWnd != 0; hWnd = GetWindow(hWnd, GW_HWNDNEXT)) {
			if (GetWindowLongA(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
				continue;

			if (hWnd == hESPWnd)
				break;

			if (hWnd == hWndOver) {
				SetWindowPos(hESPWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
				SetWindowPos(hESPWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
				break;
			}
		}
	}

public:
	RestreamChatRender(IDirect3DDevice9Ex* pDirect3DDevice9Ex, const KernelLily& kernel, int ScreenWidth, int ScreenHeight) :
		Render(pDirect3DDevice9Ex), kernel(kernel), ScreenWidth(ScreenWidth), ScreenHeight(ScreenHeight) {
		verify(InitOverlay());
		verify(UpdateLayeredWindowAttributes());
		verify(CheckBypassed());
	}
	~RestreamChatRender() { TerminateOverlayProcess(); }

	virtual void Present(HWND hGameWnd) {
		if (!IsWindow(hESPWnd))
			verify(InitOverlay());

		verify(UpdateLayeredWindowAttributes());
		verify(CheckBypassed());

		UpdateWindowPos(IsWindow(hGameWnd) ? hGameWnd : GetForegroundWindow());

		ImGuiRenderDrawData();
		pDirect3DDevice9Ex->Present(0, 0, hESPWnd, 0);
	}

	virtual bool IsFocused(HWND hGameWnd) const {
		const HWND hForegroundWnd = GetForegroundWindow();
		if (hForegroundWnd == hGameWnd)
			return true;
		if (hForegroundWnd == hESPWnd)
			return true;

		return false;
	}
};
#pragma once
#include "render.h"
#include <Shlobj.h>

class RestreamChatRender : public Render {
private:
	int ScreenWidth, ScreenHeight;
	HWND hESPWnd;

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

	static uintptr_t GetInternalSetProp(Process& process) {
		uintptr_t ScanResult = PatternScan::Module(
			process.handler.GetKernelModuleAddress("win32kfull.sys"e), ".text"e,
			"75 ? 44 ? ? ? ? 44 ? ? 05 0f ? ? ? ? ? ? 48 ? ? e8"e,
			process.handler.RPM_dbvm);

		return PatternScan::GetJumpAddress(ScanResult + 0x15, process.handler.RPM_dbvm);
	}

	static bool RemoteSetWindowDisplayAffinityBypassed(Process& process, HWND hWnd) {
		if (!IsWindowVisible(hWnd))
			return false;

		uintptr_t InternalSetProp = GetInternalSetProp(process);
		if (!InternalSetProp)
			return false;

		//Block InternalSetProp
		uint8_t CodePatch[] = { 0x48, 0x31, 0xC0, 0xFF, 0xC0, 0xC3 };
		uint8_t CodeOriginal[sizeof(CodePatch)];

		process.OpenProcessWithPid(GetCurrentProcessId());
		if (!process.handler.RPM_Physical(InternalSetProp, CodeOriginal, sizeof(CodePatch)))
			return false;

		if (!process.handler.WPM_Physical(InternalSetProp, CodePatch, sizeof(CodePatch)))
			return false;

		bool bResult = RemoteSetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);

		//Restore InternalSetProp
		while (!process.handler.WPM_Physical(InternalSetProp, CodeOriginal, sizeof(CodePatch)));

		if (!bResult)
			return false;

		//Check if bypassed
		DWORD dwNewAffinity = 1;
		if (!GetWindowDisplayAffinity(hWnd, &dwNewAffinity))
			return false;
		if (dwNewAffinity != 0)
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

		char szCommand[MAX_PATH];
		sprintf(szCommand, "if exist \"%s\" start \"\" \"%s\""e, szPath, szPath);
		system(szCommand);
		Sleep(2000);

		hESPWnd = FindWindowA("Chrome_WidgetWin_1"e, "Restream Chat"e);
		if (!hESPWnd)
			return false;

		while (!IsWindowVisible(hESPWnd));
		return true;
	}

	bool HookGetWindowData(Process& process) {
		HMODULE hUser32 = GetModuleHandleA("user32.dll"e);

		uintptr_t ScanResult = PatternScan::Module((uintptr_t)hUser32,
			".text"e, "FF 77 77 0A"e,
			[&](uintptr_t Address, void* Buffer, size_t Size) {
				return ReadProcessMemory((HANDLE)-1, LPCVOID(Address), Buffer, Size, 0);
			});

		if (ScanResult) {
			uint32_t Value = 0x0A7777DF;
			return process.handler.dbvm.WPM(ScanResult, &Value, sizeof(Value), process.handler.GetCustomCR3());
		}
		
		return PatternScan::Module((uintptr_t)hUser32,
			".text"e, "DF 77 77 0A"e,
			[&](uintptr_t Address, void* Buffer, size_t Size) {
				return ReadProcessMemory((HANDLE)-1, LPCVOID(Address), Buffer, Size, 0);
			}) != 0;
	}

	bool InitOverlay(Process& process) {
		if (!HookGetWindowData(process))
			return false;

		if (!OpenRestreamChat())
			return false;

		//SetWindowDisplayAffinity fail when parent & child windows has WS_EX_LAYERED and WS_EX_NOREDIRECTIONBITMAP
		//Close "Intermediate D3D Window" for use SetWindowDisplayAffinity (WS_EX_NOREDIRECTIONBITMAP cannot be removed)
		HWND hChild = FindWindowExA(hESPWnd, 0, "Intermediate D3D Window"e, 0);
		if (hChild)
			SendMessageA(hChild, WM_CLOSE, 0, 0);

		SetWindowLongPtrA(hESPWnd, GWL_EXSTYLE, GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED & ~WS_EX_TRANSPARENT);
		if (!RemoteSetWindowDisplayAffinityBypassed(process, hESPWnd))
			return false;
		SetWindowLongPtrA(hESPWnd, GWL_EXSTYLE, GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
		SetLayeredWindowAttributes(hESPWnd, 0, 255, LWA_ALPHA);

		if (GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT)
			return false;

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

	bool CheckBypassed() const {
		auto IsBypassed = [&] {
			if (GetWindowLongPtrA(hESPWnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT)
				return false;

			DWORD dwNewAffinity = 1;
			if (!GetWindowDisplayAffinity(hESPWnd, &dwNewAffinity))
				return false;
			if (dwNewAffinity != 0)
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

	//void Hack::UpdateWindowPos(HWND hWndOver) const {
	//	MoveWindow(hESPWnd, 0, 0, (int)ScreenWidth, (int)ScreenHeight, false);
	//	ShowWindow(hESPWnd, SW_NORMAL);
	//
	//	static HWND hAttachWnd = 0;
	//
	//	HWND hForeWnd = GetForegroundWindow();
	//	if (hForeWnd != hWndOver)
	//		return;
	//
	//	HWND hWndNext = hESPWnd;
	//	while (hWndNext = GetWindow(hWndNext, GW_HWNDNEXT))
	//		if (hWndNext == hWndOver)
	//			return;
	//
	//	if (hAttachWnd == hForeWnd) {
	//		BringWindowToTop(hESPWnd);
	//		return;
	//	}
	//
	//	hAttachWnd = hForeWnd;
	//
	//	DWORD idAttachTo = GetWindowThreadProcessId(hAttachWnd, 0);
	//	DWORD idAttach = GetWindowThreadProcessId(hESPWnd, 0);
	//	AttachThreadInput(idAttach, idAttachTo, TRUE);
	//	BringWindowToTop(hESPWnd);
	//}

public:
	RestreamChatRender(IDirect3DDevice9Ex* pDirect3DDevice9Ex, Process& process, int ScreenWidth, int ScreenHeight) :
		Render(pDirect3DDevice9Ex), ScreenWidth(ScreenWidth), ScreenHeight(ScreenHeight) {
		verify(InitOverlay(process));
	}
	~RestreamChatRender() { TerminateOverlayProcess(); }

	virtual void Present(const Hack& hack) {
		ImGuiRenderDrawData();

		if (!IsWindow(hESPWnd))
			verify(InitOverlay(hack.process));

		verify(CheckBypassed());

		HWND hWndOver = hack.process.GetHwnd();
		if (!IsWindow(hWndOver))
			hWndOver = GetForegroundWindow();

		UpdateWindowPos(hWndOver);
		pDirect3DDevice9Ex->Present(0, 0, hESPWnd, 0);
	}
};
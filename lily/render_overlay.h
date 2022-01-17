#pragma once
#include "render.h"

#include <Shlobj.h>
#include <dwmapi.h>
#include <dcomp.h>

#include "kernel_lily.h"

class RenderOverlay : public Render {
private:
	const KernelLily& kernel;
	IDCompositionTarget* m_pDCompTarget;
	IDCompositionVisual* pDirectCompositionVisual;

	virtual void Present(HWND hWnd) const {
		Global::pDXGISwapChain1->Present(0, 0);
	}

	void Release(HWND hWnd) {
		Global::pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TRUE, &m_pDCompTarget);
		m_pDCompTarget->Release();
	}

public:
	~RenderOverlay() {
		const HWND hWnd = FindWindowA("Notepad"e, 0);
		Release(hWnd);
	}

	RenderOverlay(const KernelLily& kernel, int ScreenWidth, int ScreenHeight) :
		Render(ScreenWidth, ScreenHeight), kernel(kernel) {

		//EnumWindows([](HWND hWnd, LPARAM)->BOOL {
		//	RECT wndrect;
		//	GetWindowRect(hWnd, &wndrect);
		//	//if (wndrect.left != 0 || wndrect.top != 0)
		//	//	return TRUE;

		//	if (wndrect.right == wndrect.left || wndrect.bottom == wndrect.top)
		//		return TRUE;

		//	POINT p = { 0, 0 };
		//	ScreenToClient(hWnd, &p);
		//	if (p.x != 0 || p.y != 0)
		//		return TRUE;

		//	DWORD dwStyle = GetWindowLongA(hWnd, GWL_STYLE);

		//	if (!(dwStyle & WS_VISIBLE))
		//		return TRUE;

		//	//DWORD dwExStyle = GetWindowLongA(hWnd, GWL_EXSTYLE);
		//	//if (!(dwStyle & WS_EX_TOPMOST))
		//	//	return TRUE;

		//	char szClass[0x100];
		//	char szText[0x100];
		//	GetClassNameA(hWnd, szClass, sizeof(szClass));
		//	GetWindowTextA(hWnd, szText, sizeof(szText));

		//	printf("%08X %s %s\n", (DWORD)(uint64_t)hWnd, szClass, szText);
		//	return TRUE;
		//	}, 0);

		//exit(0);

		//const HWND hWnd = FindWindowA("CEF-OSC-WIDGET"e, "NVIDIA GeForce Overlay"e);
		const HWND hWnd = FindWindowA("Notepad"e, 0);
		//const HWND hWnd = FindWindowA(0, "FolderView"e);

		HRESULT hr = 0;

		const bool bSuccess = [&] {

			if (!kernel.SetOwningThreadWrapper(hWnd, [&] {
				Release(hWnd);
				hr = Global::pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TRUE, &m_pDCompTarget);
				})) return false;

			if (FAILED(hr))
				return false;

			hr = Global::pDirectCompositionDevice->CreateVisual(&pDirectCompositionVisual);
			if (FAILED(hr))
				return false;

			hr = pDirectCompositionVisual->SetContent(Global::pDXGISwapChain1.Get());
			if (FAILED(hr))
				return false;

			hr = m_pDCompTarget->SetRoot(pDirectCompositionVisual);
			if (FAILED(hr))
				return false;

			hr = Global::pDirectCompositionDevice->Commit();
			if (FAILED(hr))
				return false;

			hr = Global::pDirectCompositionDevice->WaitForCommitCompletion();
			if (FAILED(hr))
				return false;

			bool b1 = kernel.KernelRemoveProp(hWnd, kernel.UserFindAtom(L"SysDCompHwndTargets"e));
			bool b2 = kernel.KernelRemoveProp(hWnd, kernel.UserFindAtom(L"SysVisRgnTracker"e));
			return true;
		}();

		verify(bSuccess);
		Clear();
	}
};
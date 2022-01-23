#pragma once
#include "common/render_dcomp.h"
#include "kernel_lily.h"

class RenderLily : public RenderDComp {
private:
	const KernelLily& kernel;

	const ATOM CHwndTargetProp = kernel.UserFindAtomVerified(L"SysDCompHwndTargets"e);
	const ATOM CVisRgnTrackerProp = kernel.UserFindAtomVerified(L"SysVisRgnTracker"e);

	void ReleaseDirectCompositionTarget() final {
		kernel.SetOwningThreadWrapper(hAttachWnd, [&] {
			pDirectCompositionDevice->CreateTargetForHwnd(hAttachWnd, TOPMOST, &pDirectCompositionTarget);
			pDirectCompositionTarget->Release();
			});
	}

	bool CreateDirectCompositionTarget(HWND hWnd) final {
		HRESULT hr;

		if (!kernel.SetOwningThreadWrapper(hWnd, [&] {
			hr = pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget);
			})) return false;

		if (FAILED(hr))
			return false;

		if (!kernel.KernelRemoveProp(hWnd, CHwndTargetProp) || !kernel.KernelRemoveProp(hWnd, CVisRgnTrackerProp)) {
			pDirectCompositionTarget->Release();
			return false;
		}

		//if (!SetClientRectWrapper(hWnd, { 0, 0, ScreenWidth, ScreenHeight }, [&] {
		//	hr = kernel.KernelRemoveProp(hWnd, CVisRgnTrackerProp) ? S_OK : E_FAIL;
		//	})) return false;

		return true;
	}

public:
	RenderLily(const KernelLily& kernel, float DefaultFontSize) : kernel(kernel), RenderDComp(DefaultFontSize, true) {}
	~RenderLily() final { ReleaseDirectCompositionTarget(); }
};
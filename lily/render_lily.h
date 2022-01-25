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
			if (SUCCEEDED(pDirectCompositionDevice->CreateTargetForHwnd(hAttachWnd, TOPMOST, &pDirectCompositionTarget)))
				pDirectCompositionTarget.ReleaseAndGetAddressOf();
			});
	}

	bool CreateDirectCompositionTarget(HWND hWnd) final {
		bool bSuccess = false;

		if (!kernel.SetOwningThreadWrapper(hWnd, [&] {
			if (FAILED(pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget)))
				return;
			bSuccess = kernel.KernelRemoveProp(hWnd, CHwndTargetProp) && kernel.KernelRemoveProp(hWnd, CVisRgnTrackerProp);
			if (!bSuccess)
				pDirectCompositionTarget.ReleaseAndGetAddressOf();
			})) return false;

		//if (!SetClientRectWrapper(hWnd, { 0, 0, ScreenWidth, ScreenHeight }, [&] {
		//	hr = kernel.KernelRemoveProp(hWnd, CVisRgnTrackerProp) ? S_OK : E_FAIL;
		//	})) return false;

		return bSuccess;
	}

public:
	RenderLily(const KernelLily& kernel, float DefaultFontSize) : kernel(kernel), RenderDComp(DefaultFontSize, true) {}
	~RenderLily() final { ReleaseDirectCompositionTarget(); }
};
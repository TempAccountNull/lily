#pragma once
#include "common/render_dcomp.h"
#include "common/shellcode.h"
#include "kernel_lily.h"

class RenderLily : public RenderDComp {
private:
	const KernelLily& kernel;

	void ReleaseDirectCompositionTarget() final {
		kernel.SetOwningThreadWrapper(hAttachWnd, [&] {
			pDirectCompositionDevice->CreateTargetForHwnd(hAttachWnd, TOPMOST, &pDirectCompositionTarget);
			pDirectCompositionTarget.ReleaseAndGetAddressOf();
			});
	}

	bool CreateDirectCompositionTarget(HWND hWnd) final {
		const ShellCode_Ret0 ShellCode;
		bool bSuccess = false;

		kernel.dbvm.CloakWrapper(kernel.EditionNotifyDwmForSystemVisualDestruction, &ShellCode, sizeof(ShellCode), kernel.KrnlCR3, [&] {
			kernel.SetOwningThreadWrapper(hWnd, [&] {
				bSuccess =
					pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget) == S_OK &&
					kernel.NtUserDestroyDCompositionHwndTarget(hWnd, TOPMOST);
				if (!bSuccess)
					pDirectCompositionTarget.ReleaseAndGetAddressOf();
				});
			});

		return bSuccess;
	}

public:
	RenderLily(const KernelLily& kernel, float DefaultFontSize) : kernel(kernel), RenderDComp(DefaultFontSize, true) {}
	~RenderLily() final { ReleaseDirectCompositionTarget(); }
};
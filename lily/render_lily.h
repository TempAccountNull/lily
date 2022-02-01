#pragma once
#include "common/render_dcomp.h"
#include "common/shellcode.h"
#include "kernel_lily.h"

class RenderLily : public RenderDComp {
private:
	const KernelLily& kernel;

	void ReleaseDirectCompositionTarget(HWND hWnd) final {
		kernel.SetOwningThreadWrapper(hWnd, [&] {
			pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget);
			pDirectCompositionTarget.ReleaseAndGetAddressOf();
			});
	}

	bool CreateDirectCompositionTarget(HWND hWnd) final {
		const ShellCode_Ret0 ShellCode;
		bool bSuccess = false;

		kernel.SetOwningThreadWrapper(hWnd, [&] {
			pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget);
			pDirectCompositionTarget.ReleaseAndGetAddressOf();
			if (pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget) != S_OK)
				return;
			kernel.dbvm.CloakWrapper(kernel.LpcRequestPort, &ShellCode, sizeof(ShellCode), kernel.KrnlCR3, [&] {
				bSuccess = kernel.NtUserDestroyDCompositionHwndTarget(hWnd, TOPMOST);
				});
			});

		return bSuccess;
	}

public:
	RenderLily(const KernelLily& kernel, float DefaultFontSize) : kernel(kernel), RenderDComp(DefaultFontSize, true) {}
};
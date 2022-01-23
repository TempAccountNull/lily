#pragma once
#include "render.h"

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <dcomp.h>
#pragma comment(lib, "dcomp.lib")

class RenderDComp : public Render {
private:
	ComPtr<IDXGISwapChain1> pDXGISwapChain1;
	ComPtr<ID3D11RenderTargetView> pD3D11RenderTargetView;
	ComPtr<IDCompositionVisual> pDirectCompositionVisual;

	virtual void ReleaseDirectCompositionTarget() {
		pDirectCompositionTarget.ReleaseAndGetAddressOf();
	}

	virtual bool CreateDirectCompositionTarget(HWND hWnd) {
		return pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget) == S_OK;
	}
	
	bool IsScreenPosNeeded() const final { return false; }

	ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const final { return pD3D11RenderTargetView; }

	void Present(HWND hWnd) final {
		if (hWnd && hWnd != hAttachWnd)
			AttachWindow(hWnd);

		pDXGISwapChain1->Present(1, 0);
	}

	void DetachWindow() {
		if (!hAttachWnd)
			return;

		ReleaseDirectCompositionTarget();
		hAttachWnd = 0;
	}

	void AttachWindow(HWND hWnd) {
		if (!IsWindowVisible(hWnd))
			return;

		DetachWindow();

		const bool bSuccess = [&] {
			HRESULT hr;

			if (!CreateDirectCompositionTarget(hWnd)) {
				pDirectCompositionTarget.ReleaseAndGetAddressOf();
				return false;
			}

			hr = pDirectCompositionDevice->CreateVisual(&pDirectCompositionVisual);
			if (FAILED(hr))
				return false;

			hr = pDirectCompositionVisual->SetContent(pDXGISwapChain1.Get());
			if (FAILED(hr))
				return false;

			hr = pDirectCompositionTarget->SetRoot(pDirectCompositionVisual.Get());
			if (FAILED(hr))
				return false;

			hr = pDirectCompositionDevice->Commit();
			if (FAILED(hr))
				return false;

			hr = pDirectCompositionDevice->WaitForCommitCompletion();
			if (FAILED(hr))
				return false;

			return true;
		}();

		verify(bSuccess);
		hAttachWnd = hWnd;
	}

	bool InitD3D(bool IsProtected) {
		HRESULT hr;

		ComPtr<IDXGIDevice> pDXGIDevice;
		hr = pD3D11Device->QueryInterface(__uuidof(pDXGIDevice), &pDXGIDevice);
		if (FAILED(hr))
			return false;

		ComPtr<IDXGIFactory2> dxFactory;
		hr = CreateDXGIFactory2(0, __uuidof(dxFactory), &dxFactory);
		if (FAILED(hr))
			return false;

		const DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {
			.Width = (UINT)ScreenWidth,
			.Height = (UINT)ScreenHeight,
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
			.SampleDesc = {.Count = 1},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = 2,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
			.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED,
			.Flags = IsProtected ? (UINT)DXGI_SWAP_CHAIN_FLAG_HW_PROTECTED : 0	//DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY
		};

		hr = dxFactory->CreateSwapChainForComposition(pDXGIDevice.Get(), &SwapChainDesc, 0, &pDXGISwapChain1);
		if (FAILED(hr))
			return false;

		ComPtr<ID3D11Texture2D> pBuffer;
		hr = pDXGISwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBuffer);
		if (FAILED(hr))
			return false;

		hr = pD3D11Device->CreateRenderTargetView(pBuffer.Get(), 0, &pD3D11RenderTargetView);
		if (FAILED(hr))
			return false;

		hr = DCompositionCreateDevice(pDXGIDevice.Get(), __uuidof(IDCompositionDevice), &pDirectCompositionDevice);
		if (FAILED(hr))
			return false;

		return true;
	}

protected:
	constexpr static BOOL TOPMOST = TRUE;
	ComPtr<IDCompositionDevice> pDirectCompositionDevice;
	ComPtr<IDCompositionTarget> pDirectCompositionTarget;
	HWND hAttachWnd = 0;

public:
	RenderDComp(float DefaultFontSize, bool IsProtected = false) : Render(DefaultFontSize) {
		verify(InitD3D(IsProtected));
		Clear();
	}

	virtual ~RenderDComp() override {}
};
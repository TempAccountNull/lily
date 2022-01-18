#pragma once
#include "render.h"

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <dcomp.h>

#include "kernel_lily.h"

class RenderOverlay : public Render {
private:
	constexpr static BOOL TOPMOST = TRUE;

	const KernelLily& kernel;

	ComPtr<IDXGISwapChain1> pDXGISwapChain1;
	ComPtr<ID3D11RenderTargetView> pD3D11RenderTargetView;
	ComPtr<IDCompositionTarget> pDirectCompositionTarget;
	ComPtr<IDCompositionVisual> pDirectCompositionVisual;
	ComPtr<IDCompositionDevice> pDirectCompositionDevice;

	HWND hAttachWnd = 0;

	virtual bool IsScreenPosNeeded() const { return false; }

	virtual ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const { return pD3D11RenderTargetView; }

	virtual void Present(HWND hWnd) {
		if (hWnd && hWnd != hAttachWnd)
			AttachWindow(hWnd);

		pDXGISwapChain1->Present(0, 0);
	}

	void DetachWindow(HWND hWnd) {
		const bool bSuccess = [&] {
			HRESULT hr;

			if (!kernel.SetOwningThreadWrapper(hWnd, [&] {
				hr = pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget);
				pDirectCompositionTarget->Release();
				})) return false;

			if (FAILED(hr))
				return false;

			return true;
		}();

		verify(bSuccess);
		hAttachWnd = 0;
	}

	void AttachWindow(HWND hWnd) {
		DetachWindow(hWnd);

		const bool bSuccess = [&] {
			HRESULT hr;

			if (!kernel.SetOwningThreadWrapper(hWnd, [&] {
				hr = pDirectCompositionDevice->CreateTargetForHwnd(hWnd, TOPMOST, &pDirectCompositionTarget);
				})) return false;

			if (FAILED(hr))
				return false;

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

			return
				kernel.KernelRemoveProp(hWnd, kernel.UserFindAtom(L"SysDCompHwndTargets"e)) &&
				kernel.KernelRemoveProp(hWnd, kernel.UserFindAtom(L"SysVisRgnTracker"e));
		}();

		verify(bSuccess);
		hAttachWnd = hWnd;
	}

	bool InitD3D() {
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
			.Flags = DXGI_SWAP_CHAIN_FLAG_HW_PROTECTED	//DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY
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

public:
	RenderOverlay(const KernelLily& kernel, ComPtr<ID3D11Device> pD3D11Device, ComPtr<ID3D11DeviceContext> pD3D11DeviceContext, int ScreenWidth, int ScreenHeight) :
		kernel(kernel), Render(pD3D11Device, pD3D11DeviceContext, ScreenWidth, ScreenHeight) {
		verify(InitD3D());
		Clear();
	}

	virtual ~RenderOverlay() { DetachWindow(hAttachWnd); }
};
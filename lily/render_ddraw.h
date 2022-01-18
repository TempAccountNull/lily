#pragma once
#include "render.h"

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <ddraw.h> 
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

class RenderDDraw : public Render {
private:
	ComPtr<ID3D11Texture2D> pD3D11Texture2D;
	ComPtr<ID3D11RenderTargetView> pD3D11RenderTargetView;

	ComPtr<IDirectDrawSurface7> pPrimarySurface;
	ComPtr<IDirectDrawSurface7> pOverlaySurface;
	ComPtr<IDirectDrawSurface7> pAttachedSurface;

	bool InitD3D() {
		HRESULT hr;

		const D3D11_TEXTURE2D_DESC TextureDesc = {
			.Width = (UINT)ScreenWidth,
			.Height = (UINT)ScreenHeight,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
			.SampleDesc = { .Count = 1 },
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_RENDER_TARGET,
			.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE,
		};

		hr = pD3D11Device->CreateTexture2D(&TextureDesc, 0, &pD3D11Texture2D);
		if (FAILED(hr))
			return false;

		hr = pD3D11Device->CreateRenderTargetView(pD3D11Texture2D.Get(), 0, &pD3D11RenderTargetView);
		if (FAILED(hr))
			return false;

		return true;
	}

	bool InitDirectDraw() {
		HRESULT hr;

		ComPtr<IDirectDraw7> pDirectDraw7;
		hr = DirectDrawCreateEx(0, &pDirectDraw7, IID_IDirectDraw7, 0);
		if (FAILED(hr))
			return false;

		hr = pDirectDraw7->SetCooperativeLevel(0, DDSCL_NORMAL);
		if (FAILED(hr))
			return false;

		DDSURFACEDESC2 PrimarySurfaceDesc = {
			.dwSize = sizeof(PrimarySurfaceDesc),
			.dwFlags = DDSD_CAPS,
			.ddsCaps = {.dwCaps = DDSCAPS_PRIMARYSURFACE }
		};

		hr = pDirectDraw7->CreateSurface(&PrimarySurfaceDesc, &pPrimarySurface, 0);
		if (FAILED(hr))
			return false;

		DDSURFACEDESC2 OverlaySurfaceDesc = {
			.dwSize = sizeof(PrimarySurfaceDesc),
			.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_BACKBUFFERCOUNT | DDSD_PIXELFORMAT,
			.dwHeight = (unsigned long)ScreenHeight,
			.dwWidth = (unsigned long)ScreenWidth,
			.dwBackBufferCount = 1,
			.ddpfPixelFormat = DDPIXELFORMAT{ sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32, 0xFF0000, 0x00FF00, 0x0000FF, 0 },
			.ddsCaps = {.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY }
		};

		hr = pDirectDraw7->CreateSurface(&OverlaySurfaceDesc, &pOverlaySurface, 0);
		if (FAILED(hr))
			return false;

		DDSCAPS2 AttachedSurfaceCaps2 = { .dwCaps = DDSCAPS_BACKBUFFER };
		hr = pOverlaySurface->GetAttachedSurface(&AttachedSurfaceCaps2, &pAttachedSurface);
		if (FAILED(hr))
			return false;

		DDOVERLAYFX OverlayFX = { .dwSize = sizeof(OverlayFX), .dckSrcColorkey = { COLOR_CLEAR_RGB, COLOR_CLEAR_RGB }, };
		RECT Rect = { 0, 0, ScreenWidth, ScreenHeight };
		const DWORD dwUpdateFlags = DDOVER_SHOW | DDOVER_DDFX | DDOVER_KEYSRCOVERRIDE;

		for (unsigned i = 0; i < 10 && 
			FAILED(hr = pOverlaySurface->UpdateOverlay(0, pPrimarySurface.Get(), &Rect, dwUpdateFlags, &OverlayFX)); i++);

		if (FAILED(hr))
			return false;

		return true;
	}

	virtual bool IsScreenPosNeeded() const { return true; }

	virtual ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const { return pD3D11RenderTargetView; }

	virtual void Present(HWND) {
		ComPtr<IDXGISurface1> pDXGISurface;
		pD3D11Texture2D->QueryInterface(__uuidof(pDXGISurface), &pDXGISurface);

		HDC hSurfaceDC;
		pDXGISurface->GetDC(FALSE, &hSurfaceDC);
		HDC hAttachedSurfaceDC;
		pAttachedSurface->GetDC(&hAttachedSurfaceDC);
		BitBlt(hAttachedSurfaceDC, 0, 0, ScreenWidth, ScreenHeight, hSurfaceDC, 0, 0, SRCCOPY);
		pAttachedSurface->ReleaseDC(hAttachedSurfaceDC);
		pDXGISurface->ReleaseDC(0);

		pOverlaySurface->Flip(0, DDFLIP_DONOTWAIT | DDFLIP_NOVSYNC);
	}

public:
	RenderDDraw(ComPtr<ID3D11Device> pD3D11Device, ComPtr<ID3D11DeviceContext> pD3D11DeviceContext, int ScreenWidth, int ScreenHeight) :
		Render(pD3D11Device, pD3D11DeviceContext, ScreenWidth, ScreenHeight) {
		verify(InitD3D());
		verify(InitDirectDraw());
		Clear();
	}
};
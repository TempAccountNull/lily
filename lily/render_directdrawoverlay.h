#pragma once
#include "render.h"

#include <Windows.h>
#include <ddraw.h> 
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

class DirectDrawOverlayRender : public Render {
private:
	int ScreenWidth, ScreenHeight;

	LPDIRECTDRAWSURFACE7 pPrimarySurface;
	LPDIRECTDRAWSURFACE7 pOverlaySurface;
	LPDIRECTDRAWSURFACE7 pAttachedSurface;

	IDirect3DSurface9* pBackBufferSurface;
	IDirect3DSurface9* pOffscreenPlainSurface;

	bool InitDirectDraw(int ScreenWidth, int ScreenHeight) {
		LPDIRECTDRAW7 pDirectDraw7;
		HRESULT res = DirectDrawCreateEx(0, (void**)&pDirectDraw7, IID_IDirectDraw7, 0);
		if (FAILED(res))
			return false;

		res = pDirectDraw7->SetCooperativeLevel(0, DDSCL_NORMAL);
		if (FAILED(res))
			return false;

		DDSURFACEDESC2 Desc = { 0 };
		Desc.dwSize = sizeof(Desc);
		Desc.dwFlags = DDSD_CAPS;
		Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		res = pDirectDraw7->CreateSurface(&Desc, &pPrimarySurface, 0);
		if (FAILED(res))
			return false;

		Desc = { 0 };
		Desc.dwSize = sizeof(Desc);
		Desc.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
		Desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_BACKBUFFERCOUNT | DDSD_PIXELFORMAT;
		Desc.dwWidth = ScreenWidth;
		Desc.dwHeight = ScreenHeight;
		Desc.dwBackBufferCount = 1;
		Desc.ddpfPixelFormat = DDPIXELFORMAT{ sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32, 0xFF0000, 0x00FF00, 0x0000FF, 0 };
		res = pDirectDraw7->CreateSurface(&Desc, &pOverlaySurface, 0);
		if (FAILED(res))
			return false;

		DDSCAPS2 Caps2 = { DDSCAPS_BACKBUFFER };
		res = pOverlaySurface->GetAttachedSurface(&Caps2, &pAttachedSurface);
		if (FAILED(res))
			return false;

		DDOVERLAYFX OverlayFX = { 0 };
		OverlayFX.dwSize = sizeof(OverlayFX);
		OverlayFX.dckSrcColorkey = DDCOLORKEY{ COLOR_CLEAR, COLOR_CLEAR };
		RECT Rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
		DWORD dwUpdateFlags = DDOVER_SHOW | DDOVER_DDFX | DDOVER_KEYSRCOVERRIDE;

		//Retry if fail
		for (int i = 0; i < 0x20; i++) {
			res = pOverlaySurface->UpdateOverlay(0, pPrimarySurface, &Rect, dwUpdateFlags, &OverlayFX);
			if (SUCCEEDED(res))
				break;
		}

		if (FAILED(res))
			return false;

		pDirectDraw7->Release();
		return true;
	}

public:
	DirectDrawOverlayRender(IDirect3DDevice9Ex* pDirect3DDevice9Ex, IDirect3DSurface9* pBackBufferSurface, IDirect3DSurface9* pOffscreenPlainSurface, int ScreenWidth, int ScreenHeight) :
		Render(pDirect3DDevice9Ex), pBackBufferSurface(pBackBufferSurface), pOffscreenPlainSurface(pOffscreenPlainSurface), ScreenWidth(ScreenWidth), ScreenHeight(ScreenHeight) {
		verify(InitDirectDraw(ScreenWidth, ScreenHeight));
	}

	virtual void Present(HWND hGameWnd) {
		ImGuiRenderDrawData();

		pDirect3DDevice9Ex->GetRenderTargetData(pBackBufferSurface, pOffscreenPlainSurface);

		HDC hOffscreenPlainSurfaceDC, hAttachedSurfaceDC;
		pOffscreenPlainSurface->GetDC(&hOffscreenPlainSurfaceDC);
		pAttachedSurface->GetDC(&hAttachedSurfaceDC);
		BitBlt(hAttachedSurfaceDC, 0, 0, ScreenWidth, ScreenHeight, hOffscreenPlainSurfaceDC, 0, 0, SRCCOPY);
		pAttachedSurface->ReleaseDC(hAttachedSurfaceDC);
		pOffscreenPlainSurface->ReleaseDC(hOffscreenPlainSurfaceDC);

		pOverlaySurface->Flip(0, DDFLIP_DONOTWAIT | DDFLIP_NOVSYNC);
	}

	virtual bool IsFocused(HWND hGameWnd) const {
		if (GetForegroundWindow() == hGameWnd)
			return true;

		return false;
	}
};
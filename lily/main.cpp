#pragma once
#include <stdio.h>
#include <Shlobj.h>

#include "global.h"
#include "common/dbvm.h"
#include "kernel_lily.h"
#include "common/process.h"
#include "hack.h"
#include "common/encrypt_string.h"

#include "render_overlay.h"
#include "render_ddraw.h"

void realmain() {
#ifdef DPRINT
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON"e, "w"e, stdout);
	dprintf("debug print enabled"e);
#endif
	if (IsUserAnAdmin())
		dprintf("Admin"e);
	else
		dprintf("Not Admin"e);

	KernelLily kernel(Global::dbvm);
	dprintf("Kernel OK"e);

#ifdef _WINDLL
	RenderOverlay render(kernel, Global::pD3D11Device, Global::pD3D11DeviceContext, Global::ScreenWidth, Global::ScreenHeight);
#else
	RenderDDraw render(Global::pD3D11Device, Global::pD3D11DeviceContext, Global::ScreenWidth, Global::ScreenHeight);
#endif

	for (;; Sleep(1)) {
		render.RenderArea(FindWindowA("Progman"e, "Program Manager"e), [&] {
			render.DrawString({ 500.0f, 0.0f, 0.0f }, Render::MARGIN, "Waiting for PUBG..."e, Render::FONTSIZE_BIG, Render::COLOR_RED, true, true, true);
			});

		const HWND hGameWnd = FindWindowA("UnrealWindow"e, 0);
		if (!hGameWnd)
			continue;

		ExceptionHandler::TryExcept([&] {
			PubgProcess pubg(kernel, hGameWnd);
			if (!ExceptionHandler::TryExcept([&] {
				Hack hack(pubg, render, Global::Buf, sizeof(Global::Buf));
				hack.Loop();
				})) {
				error("exception loop"e);
			}
			});
	}
}
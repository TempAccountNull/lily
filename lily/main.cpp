#pragma once
#include <stdio.h>
#include <Shlobj.h>

#include "global.h"
#include "dbvm.h"
#include "kernel_lily.h"
#include "process.h"
#include "hack.h"
#include "encrypt_string.h"

#include "render_overlay.h"
#include "render_ddraw.h"

#include "dump.h"

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
	Process process(kernel);

#ifdef _WINDLL
	RenderOverlay render(Global::pDirect3DDevice9Ex, process.kernel, Global::ScreenWidth, Global::ScreenHeight);
#else
	RenderDDraw render(Global::pDirect3DDevice9Ex, Global::pBackBufferSurface, Global::pOffscreenPlainSurface, Global::ScreenWidth, Global::ScreenHeight);
#endif

	Hack hack(process, render, Global::ScreenWidth, Global::ScreenHeight, Global::Buf, sizeof(Global::Buf));
	dprintf("hack OK"e);

	while (true) {
		hack.RenderArea([&] {
			hack.DrawString({ 500.0f, 0.0f, 0.0f }, Hack::MARGIN, "Waiting for PUBG..."e, Hack::FONTSIZE_BIG, Hack::COLOR_RED, true, true, true);
			});

		HWND hGameWnd = FindWindowA("UnrealWindow"e, 0);
		if (!hGameWnd)
			continue;

		if (!process.OpenProcessWithHWND(hGameWnd, "TslGame.exe"e))
			continue;

		//DumpAll(process);
		//Dump(process);
		//break;

		hack.Loop(process);
	}
}
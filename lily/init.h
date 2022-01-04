#pragma once
#include <stdio.h>
#include <Shlobj.h>

#include "global.h"
#include "dbvm.h"
#include "kernel_lily.h"
#include "process.h"
#include "hack.h"
#include "encrypt_string.h"

#include "render_restreamchat.h"
#include "render_directdrawoverlay.h"

#include "dump.h"

int Init() {
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

	uint64_t password1 = *(uint64_t*)(Global::DBVMPassword + 0) ^ 0xda2355698be6166c;
	uint32_t password2 = *(uint32_t*)(Global::DBVMPassword + 8) ^ 0x6765fa70;
	uint64_t password3 = *(uint64_t*)(Global::DBVMPassword + 12) ^ 0xe21cb5155c065962;

	DBVM dbvm(password1, password2, password3);
	verify(dbvm.GetVersion());

	dprintf("DBVM OK"e);
	KernelLily kernel(dbvm);
	dprintf("Kernel OK"e);
	Process process(kernel);

#ifdef _WINDLL
	RestreamChatRender render(Global::pDirect3DDevice9Ex, process.kernel, Global::ScreenWidth, Global::ScreenHeight);
#else
	DirectDrawOverlayRender render(Global::pDirect3DDevice9Ex, Global::pBackBufferSurface, Global::pOffscreenPlainSurface, Global::ScreenWidth, Global::ScreenHeight);
#endif

	Hack hack(process, render, Global::ScreenWidth, Global::ScreenHeight, Global::Buf, sizeof(Global::Buf));
	dprintf("hack OK"e);

	while (1) {
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
		if (hack.IsTerminating())
			break;
	}

	return 0;
}
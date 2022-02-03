﻿#pragma once
#include <string>

#include "common/render.h"
#include "kernel_lily.h"
#include "pubg_class.h"
#include "pubg_func.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class Hack {
private:
	constexpr static float NOTICE_TIME = 3.0f;

	float NoticeTimeRemain = 0.0f;

	bool bFighterMode = false;
	bool bVehicle = true;
	bool bBox = true;
	bool bTeamKill = false;
	int nItem = 1;
	int nAimbot = 1;
	int nRange = 500;
	float CircleFovInDegrees = 6.0f;

	constexpr static float DefaultAimbotSpeed = 1.0f;
	constexpr static float DefaultAimbotSpeedMax = DefaultAimbotSpeed * 2.0f * 2.0f;
	constexpr static float DefaultAimbotSpeedMin = DefaultAimbotSpeed * 0.5f * 0.5f;
	float AimbotSpeedX = DefaultAimbotSpeed;
	float AimbotSpeedY = DefaultAimbotSpeed;
	float RemainMouseX = 0.0f;
	float RemainMouseY = 0.0f;

	bool bPushingCTRL = false;
	bool bPushingShift = false;
	bool bPushingMouseM = false;
	bool bPushingMouseL = false;
	bool bPushingMouseR = false;

	bool bShowMenu = true;
	bool bDebug = false;

	Render& render;
	bool& bESP = render.bRender = true;
	bool& bIgnoreMouseInput = render.bIgnoreMouseInput = false;

	mutable char szBuf[0x100];

	constexpr static unsigned DEBUGLOG_MAXSIZE = 0x4000;
	constexpr static unsigned DEBUGLOG_REMOVESIZE = 0x1000;
	std::string debuglog;
	bool bCaptureLog = true;
	bool bNeedToScroll = false;

public:
	constexpr static unsigned MARGIN = 10;

	constexpr static float FONTSIZE_SMALL = 15.0f;
	constexpr static float FONTSIZE_NORMAL = 20.0f;
	constexpr static float FONTSIZE_BIG = 40.0f;
	constexpr static float FONTSIZE = FONTSIZE_SMALL;

	PubgProcess& pubg;
	KernelLily& kernel;
	const DBVM& dbvm = kernel.dbvm;

	Hack(PubgProcess& pubg, Render& render, KernelLily& kernel) : pubg(pubg), kernel(kernel), render(render) {}

	void Loop();

	FVector WorldToScreen(const FVector& WorldLocation, const FMatrix& RotationMatrix, const FVector& CameraLocation, float CameraFOV) const {
		return ::WorldToScreen(WorldLocation, RotationMatrix, CameraLocation, CameraFOV, render.Width, render.Height);
	}

	void DrawString(const FVector& pos, const char* szText, ImColor Color, bool bShowAlways) const {
		render.DrawString(pos, Hack::MARGIN, szText, FONTSIZE, Color, true, true, bShowAlways);
	}

	void DrawNotice(const char* szText, ImColor Color) const {
		render.DrawString({ 30.0f, 30.0f , 0.0f }, Hack::MARGIN, szText, Hack::FONTSIZE_NORMAL, Color, false, false, true);
	}

	void DrawFPS(unsigned FPS, ImColor Color) const {
		sprintf(szBuf, "FPS : %d"e, FPS);
		render.DrawString({ render.Width * 0.6f, 0.0f , 0.0f }, Hack::MARGIN, szBuf, Hack::FONTSIZE_SMALL, Color, true, true, true);
	}

	void DrawZeroingDistance(float ZeroingDistance, ImColor Color) const {
		sprintf(szBuf, "Zero : %.0f"e, ZeroingDistance);
		render.DrawString({ render.Width * 0.5f, 0.0f , 0.0f }, Hack::MARGIN, szBuf, Hack::FONTSIZE_SMALL, Color, true, true, true);
	}

	void DrawSpectatedCount(unsigned SpectatedCount, ImColor Color) const {
		sprintf(szBuf, "Spectators : %d"e, SpectatedCount);
		render.DrawString({ render.Width * 0.4f, 0.0f , 0.0f }, Hack::MARGIN, szBuf, Hack::FONTSIZE_SMALL, Color, true, true, true);
	}

	void DrawHotkey() const;
	void ProcessImGui();
	void ProcessHotkey();

	void printlog(const char* fmt, ...) {
		if (!bCaptureLog)
			return;
		va_list args;
		va_start(args, fmt);
		char buf[0x100];
		vsnprintf(buf, 0x100, fmt, args);
		va_end(args);

		if (strlen(buf) + debuglog.size() > DEBUGLOG_MAXSIZE)
			debuglog = debuglog.substr(DEBUGLOG_REMOVESIZE);

		bNeedToScroll = true;
		debuglog += buf;
	}
};
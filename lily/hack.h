#pragma once
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
	int nCapsLockMode = 0;
	bool bAimToEnemyFocusingMe = false;
	bool bPlayer = true;
	bool bVehicle = true;
	bool bRadar = true;
	bool bBox = true;
	bool bTeamKill = false;
	int nItem = 1;
	bool bAimbot = true;
	bool bSilentAim = false;
	bool bSilentAim_DangerousMode = false;
	bool bFocusNextEnemy = false;
	int nRange = 500;
	float CircleFovInDegrees = 6.0f;

	constexpr static float FiringTime = 0.2f;
	const ImVec2 RadarFrom = { 0.846f , 0.736f };
	const ImVec2 RadarTo = { 0.984f , 0.974f };
	const ImVec2 RadarSize = {
		RadarTo.x - RadarFrom.x,
		RadarTo.y - RadarFrom.y
	};
	const ImVec2 RadarCenter = {
		(RadarFrom.x + RadarTo.x) / 2.0f,
		(RadarFrom.y + RadarTo.y) / 2.0f
	};

	struct {
		bool bSkeleton = true;
		bool bHealth = true;
		bool bNickName = true;
		bool bTeam = true;
		bool bWeapon = true;
		bool bDistance = true;
		bool bKill = true;
		bool bDamage = true;
	}ESP_PlayerSetting;

	constexpr static float AimSpeedMaxFactor = 1.0f / 3.0f;
	constexpr static float AimSpeedMax = 1000000.0f;
	constexpr static float AimSpeedMin = 0.0f;
	constexpr static float AimSpeedDefault = 1000.0f;
	float AimSpeedX = AimSpeedDefault;
	float AimSpeedY = AimSpeedDefault;

	bool bPushingCTRL = false;
	bool bPushingShift = false;
	bool bPushingMouseM = false;
	bool bPushingMouseL = false;
	bool bPushingMouseR = false;
	bool bPushingCapsLock = false;

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

	constexpr static float MinFocusTime = 0.1f;

public:
	constexpr static unsigned MARGIN = 10;

	constexpr static float FONTSIZE_SMALL = 15.0f;
	constexpr static float FONTSIZE_NORMAL = 20.0f;
	constexpr static float FONTSIZE_BIG = 40.0f;
	constexpr static float FONTSIZE = FONTSIZE_SMALL;

	const ImColor TeamColors[51] = {
		ImColor::HSV(1.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(2.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(3.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(4.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(5.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(6.5f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(10.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(11.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(12.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(13.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(14.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(15.0f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(17.5f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(18.5f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(19.5f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(20.5f / 24.0f, 1.0f, 0.8f),
		ImColor::HSV(22.0f / 24.0f, 1.0f, 0.8f),

		ImColor::HSV(1.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(2.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(3.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(4.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(5.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(6.5f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(10.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(11.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(12.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(13.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(14.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(15.0f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(17.5f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(18.5f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(19.5f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(20.5f / 24.0f, 0.5f, 0.8f),
		ImColor::HSV(22.0f / 24.0f, 0.5f, 0.8f),

		ImColor::HSV(1.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(2.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(3.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(4.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(5.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(6.5f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(10.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(11.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(12.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(13.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(14.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(15.0f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(17.5f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(18.5f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(19.5f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(20.5f / 24.0f, 0.5f, 0.6f),
		ImColor::HSV(22.0f / 24.0f, 0.5f, 0.6f),
	};

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

	void DrawFPS(float FPS, ImColor Color) const {
		sprintf(szBuf, "FPS : %.0f"e, FPS);
		render.DrawString({ render.Width * 0.6f, 0.0f , 0.0f }, Hack::MARGIN, szBuf, Hack::FONTSIZE_SMALL, Color, true, true, true);
	}

	void DrawEnemiesFocusingMe(const char* szPlayers, ImColor Color) const {
		render.DrawString({ render.Width * 0.5f, 0.0f , 0.0f }, Hack::MARGIN, szPlayers, Hack::FONTSIZE_SMALL, Color, true, true, true);
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

	void MoveMouse(HWND hWnd, POINT Movement) {
		kernel.PostRawMouseInput(hWnd, { .usFlags = MOUSE_MOVE_RELATIVE, .lLastX = Movement.x, .lLastY = Movement.y });
		//mouse_event(MOUSEEVENTF_MOVE, Movement.x, Movement.y, 0, 0);
	}
};
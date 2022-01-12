#pragma once
#include <string>

#include "render.h"
#include "pubg_class.h"
#include "pubg_func.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

class Hack {
private:
	static constexpr float FONTSIZE = Render::FONTSIZE_SMALL;
	static constexpr float NOTICE_TIME = 3.0f;

	float NoticeTimeRemain = 0.0f;

	bool bTurnBackShortKey = false;
	bool bTurnBack = false;
	bool bFighterMode = false;

	bool& bESP;
	bool bVehicle = true;
	bool bBox = true;
	bool bTeamKill = false;
	int nItem = 1;
	int nAimbot = 1;
	int nRange = 500;
	float CircleFovInDegrees = 6.0f;

	bool bPushingCTRL = false;
	bool bPushingShift = false;
	bool bPushingMouseM = false;
	bool bPushingMouseL = false;
	bool bPushingMouseR = false;

	bool bShowMenu = true;
	bool bDebug = false;

	Render& render;

	char* const szBuf = 0;
	const size_t nBufSize = 0;

	uint64_t PrevFPSTime = GetTickCountInMicroSeconds();
	uint64_t PrevTime = GetTickCountInMicroSeconds();
	unsigned FPSCount = 0;
	unsigned FPS = 0;

	float ProcessTimeDelta() {
		FPSCount++;
		uint64_t CurrTime = GetTickCountInMicroSeconds();
		uint64_t Delta = CurrTime - PrevTime;

		//limit FPS
		//while (Delta < 5) {
		//	CurrTime = GetAccurateTickCount();
		//	Delta = CurrTime - PrevTime;
		//}

		//Calculate FPS each 1s 
		if (CurrTime - PrevFPSTime > 1000000) {
			PrevFPSTime = CurrTime;
			FPS = FPSCount;
			FPSCount = 0;
			dprintf("%d"e, FPS);
		}
		PrevTime = CurrTime;

		const float DeltaInSeconds = (float)Delta / 1000000.0f;

		if (DeltaInSeconds < NoticeTimeRemain)
			NoticeTimeRemain -= DeltaInSeconds;
		else
			NoticeTimeRemain = 0;

		return DeltaInSeconds;
	}

	constexpr static unsigned DEBUGLOG_MAXSIZE = 0x4000;
	constexpr static unsigned DEBUGLOG_REMOVESIZE = 0x1000;
	std::string debuglog;
	bool bCaptureLog = true;
	bool bNeedToScroll = false;

public:
	PubgProcess& pubg;
	Kernel& kernel;
	const DBVM& dbvm;

	static ImU32 GetItemColor(int ItemPriority) {
		switch (ItemPriority) {
		case 1: return Render::COLOR_YELLOW;
		case 2: return Render::COLOR_ORANGE;
		case 3: return Render::COLOR_PURPLE;
		case 4: return Render::COLOR_TEAL;
		case 5: return Render::COLOR_BLACK;
		default:return Render::COLOR_WHITE;
		}
	}

	Hack(PubgProcess& pubg, Render& render, char* szBuf, size_t nBufSize) :
		pubg(pubg), kernel(pubg.kernel), dbvm(kernel.dbvm), render(render),
		szBuf(szBuf), nBufSize(nBufSize), bESP(render.bRender) {
		bESP = true;
	}

	void Loop();
	
	void DrawString(const Vector& pos, const char* szText, ImU32 Color, bool bShowAlways) const {
		render.DrawString(pos, Render::MARGIN, szText, FONTSIZE, Color, true, true, bShowAlways);
	}

	void DrawNotice(const char* szText, ImU32 Color) const {
		render.DrawString({ 30.0f, 30.0f , 0.0f }, Render::MARGIN, szText, Render::FONTSIZE_NORMAL, Color, false, false, true);
	}

	void DrawFPS(unsigned FPS, ImU32 Color) const {
		float Width = ImGui::GetMainViewport()->WorkSize.x;
		sprintf(szBuf, "FPS : %d"e, FPS);
		render.DrawString({ Width * 0.6f, 0.0f , 0.0f }, Render::MARGIN, szBuf, Render::FONTSIZE_SMALL, Color, true, true, true);
	}

	void DrawZeroingDistance(float ZeroingDistance, ImU32 Color) const {
		float Width = ImGui::GetMainViewport()->WorkSize.x;
		sprintf(szBuf, "Zero : %.0f"e, ZeroingDistance);
		render.DrawString({ Width * 0.5f, 0.0f , 0.0f }, Render::MARGIN, szBuf, Render::FONTSIZE_SMALL, Color, true, true, true);
	}

	void DrawSpectatedCount(unsigned SpectatedCount, ImU32 Color) const {
		float Width = ImGui::GetMainViewport()->WorkSize.x;
		sprintf(szBuf, "Spectators : %d"e, SpectatedCount);
		render.DrawString({ Width * 0.4f, 0.0f , 0.0f }, Render::MARGIN, szBuf, Render::FONTSIZE_SMALL, Color, true, true, true);
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

	Vector WorldToScreen(const Vector& WorldLocation, const Matrix& RotationMatrix, const Vector& CameraLocation, float CameraFOV) const {
		return ::WorldToScreen(WorldLocation, RotationMatrix, CameraLocation, CameraFOV, render.GetWidth(), render.GetHeight());
	}
};
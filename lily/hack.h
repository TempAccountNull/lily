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
	static constexpr float NOTICE_TIME = 3.0f;

	float PosX = 0.0f, PosY = 0.0f;
	float Width = 0.0f, Height = 0.0f;

	float NoticeTimeRemain = 0.0f;

	bool bTurnBackShortKey = false;
	bool bTurnBack = false;
	bool bFighterMode = false;

	bool bESP = true;
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

	void UpdateGameClientRect() {
		HWND hGameWnd = process.GetHwnd();
		RECT ClientRect;
		GetClientRect(hGameWnd, &ClientRect);
		POINT ClientPos = { 0, 0 };
		ClientToScreen(hGameWnd, &ClientPos);
		Resize((float)ClientPos.x, (float)ClientPos.y, (float)ClientRect.right, (float)ClientRect.bottom);
	}

	constexpr static unsigned DEBUGLOG_MAXSIZE = 0x4000;
	constexpr static unsigned DEBUGLOG_REMOVESIZE = 0x1000;
	std::string debuglog;
	bool bCaptureLog = true;
	bool bNeedToScroll = false;

public:
	Process& process;
	Kernel& kernel;
	const DBVM& dbvm;

	static constexpr unsigned MARGIN = 10;

	static constexpr float FONTSIZE_SMALL = 15.0f;
	static constexpr float FONTSIZE_NORMAL = 20.0f;
	static constexpr float FONTSIZE_BIG = 40.0f;

	static constexpr ImU32 COLOR_RED = IM_COL32(255, 0, 0, 255);
	static constexpr ImU32 COLOR_GREEN = IM_COL32(0, 255, 0, 255);
	static constexpr ImU32 COLOR_BLUE = IM_COL32(0, 0, 255, 255);
	static constexpr ImU32 COLOR_BLACK = IM_COL32(0, 0, 0, 255);
	static constexpr ImU32 COLOR_WHITE = IM_COL32(255, 255, 255, 255);
	static constexpr ImU32 COLOR_ORANGE = IM_COL32(255, 128, 0, 255);
	static constexpr ImU32 COLOR_YELLOW = IM_COL32(255, 255, 0, 255);
	static constexpr ImU32 COLOR_PURPLE = IM_COL32(255, 0, 255, 255);
	static constexpr ImU32 COLOR_TEAL = IM_COL32(0, 255, 255, 255);
	static constexpr ImU32 COLOR_GRAY = IM_COL32(192, 192, 192, 255);

	static ImU32 GetItemColor(int ItemPriority) {
		switch (ItemPriority) {
		case 1: return COLOR_YELLOW;
		case 2: return COLOR_ORANGE;
		case 3: return COLOR_PURPLE;
		case 4: return COLOR_TEAL;
		case 5: return COLOR_BLACK;
		default:return COLOR_WHITE;
		}
	}

	Hack(Process& process, Render& render, int ScreenWidth, int ScreenHeight, char* szBuf, size_t nBufSize) :
		process(process), kernel(process.kernel), dbvm(kernel.dbvm), render(render),
		szBuf(szBuf), nBufSize(nBufSize), Width((float)ScreenWidth), Height((float)ScreenHeight) {}

	void Loop(Process& process);
	void Resize(float X, float Y, float W, float H) { PosX = X; PosY = Y; Width = W; Height = H; }
	RECT GetGameClientRect() const { return { (LONG)PosX, (LONG)PosY, (LONG)(PosX + Width), (LONG)(PosY + Height) }; }
	void InsertMouseInfo() const;

	static ImVec2 GetTextSize(const char* szText);
	void DrawString(const Vector& Pos, float Margin, const char* szText, float Size, ImU32 Color, bool bCenterPos, bool bCenterAligned, bool bShowAlways) const;
	void DrawString(const Vector& pos, const char* szText, ImU32 Color, bool bShowAlways) const;
	void DrawNotice(const char* szText, ImU32 Color) const;
	void DrawFPS(unsigned FPS, ImU32 Color) const;
	void DrawZeroingDistance(float ZeroingDistance, ImU32 Color) const;
	void DrawSpectatedCount(unsigned SpectatedCount, ImU32 Color) const;
	void DrawRatioBox(const Vector& from, const Vector& to, float HealthRatio, ImU32 ColorRemain, ImU32 ColorDamaged, ImU32 ColorEdge) const;
	void DrawLine(const Vector& from, const Vector& to, ImU32 Color, float thickness = 1.0f) const;
	void DrawCircle(const ImVec2& center, float radius, ImU32 Color, int num_segments = 0, float thickness = 1.0f) const;
	void DrawRectOutlined(const Vector& from, const Vector& to, ImU32 Color, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f) const;
	void DrawRectFilled(const Vector& from, const Vector& to, ImU32 Color, float rounding = 0.0f, ImDrawFlags flags = 0) const;
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

	void ProcessExitHotkey() const {
		if (!IsKeyPushing(VK_MENU) && !IsKeyPushing(VK_MBUTTON))
			return;
		if (!IsKeyPushed(VK_END))
			return;
		render.Clear();
		TerminateProcess((HANDLE)-1, 0);
	}

	void RenderArea(auto func) const {
		ProcessExitHotkey();

		ImGui_ImplDX9_NewFrame();
		ImGui::NewFrame();

		auto Viewport = ImGui::GetMainViewport();
		Viewport->Pos = { -PosX, -PosY };
		Viewport->WorkPos = { 0.0f, 0.0f };
		Viewport->Size = { PosX + Width, PosY + Height };
		Viewport->WorkSize = { Width, Height };

		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::SetNextWindowSize({ Width, Height });
		ImGui::Begin("ESP"e, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

		func();

		ImGui::End();
		ImGui::EndFrame();
		render.Present(process.GetHwnd());
	}

	Vector WorldToScreen(const Vector& WorldLocation, const Matrix& RotationMatrix, const Vector& CameraLocation, float CameraFOV) const {
		return ::WorldToScreen(WorldLocation, RotationMatrix, CameraLocation, CameraFOV, Width, Height);
	}
};
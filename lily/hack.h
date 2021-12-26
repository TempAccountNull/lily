#pragma once

#define LOGPRINT

#include <string>
#include <atlconv.h>
#include <functional>

#include "keystate.h"
#include "process.h"
#include "vector.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

class Render;

class Hack {
private:
	static constexpr float NOTICE_TIME = 3.0f;

	float PosX, PosY;
	float Width, Height;

	float NoticeTimeRemain = 0.0f;

	bool bTurnBackShortKey = false;
	bool bTurnBack = false;
	bool bFighterMode = false;

	bool bESP = true;
	bool bVehicle = true;
	int nItem = 1;
	bool bBox = true;
	int nAimbot = 1;
	int nRange = 500;
	float CircleFovInDegrees = 6.0f;

	bool bPushingCTRL = false;
	bool bPushingShift = false;
	bool bTeamKill = false;
	bool bPushingMouseM = false;
	bool bPushingMouseL = false;
	bool bPushingMouseR = false;


	bool bShowMenu = true;
	bool bTerminate = false;
	bool bDebug = false;

	Render& render;

	DWORD64 AimHookAddress = 0;
	char* const szBuf;
	const size_t nBufSize;

#ifdef LOGPRINT
	constexpr static unsigned DEBUGLOG_MAXSIZE = 0x4000;
	constexpr static unsigned DEBUGLOG_REMOVESIZE = 0x1000;
	std::string debuglog;
	bool bCaptureLog = true;
	bool bNeedToScroll = false;
#endif

	static uint64_t GetTickCountInMicroSeconds() {
		LARGE_INTEGER PerformanceCount, Frequency;
		QueryPerformanceFrequency(&Frequency);
		QueryPerformanceCounter(&PerformanceCount);
		return PerformanceCount.QuadPart * 1000000 / Frequency.QuadPart;
	}

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

	RECT PrevRect = { 0 };

	void UpdateClientRect() {
		HWND hGameWnd = process.GetHwnd();

		RECT ClientRect;
		::GetClientRect(hGameWnd, &ClientRect);

		POINT p1 = { ClientRect.left, ClientRect.top };
		POINT p2 = { ClientRect.right, ClientRect.bottom };
		ClientToScreen(hGameWnd, &p1);
		ClientToScreen(hGameWnd, &p2);

		RECT ScreenRect = { p1.x, p1.y, p2.x, p2.y };
		if (memcmp(&ScreenRect, &PrevRect, sizeof(RECT))) {
			PrevRect = ScreenRect;
			Resize((float)p1.x, (float)p1.y, (float)ClientRect.right, (float)ClientRect.bottom);
		}
	}

public:
	Process& process;
	Kernel& kernel;
	const DBVM& dbvm;

	static constexpr unsigned MARGIN = 10;

	static constexpr float FONTSIZE_SMALL = 15.0f;
	static constexpr float FONTSIZE_NORMAL = 20.0f;
	static constexpr float FONTSIZE_BIG = 40.0f;

	static constexpr ImU32 COLOR_NONE = IM_COL32(0, 0, 0, 0);
	static constexpr ImU32 COLOR_RED = IM_COL32(255, 0, 0, 255);
	static constexpr ImU32 COLOR_GREEN = IM_COL32(0, 255, 0, 255);
	static constexpr ImU32 COLOR_BLUE = IM_COL32(0, 0, 255, 255);
	static constexpr ImU32 COLOR_BLACK = IM_COL32(1, 1, 1, 255);
	static constexpr ImU32 COLOR_WHITE = IM_COL32(255, 255, 255, 255);
	static constexpr ImU32 COLOR_ORANGE = IM_COL32(255, 128, 0, 255);
	static constexpr ImU32 COLOR_YELLOW = IM_COL32(255, 255, 0, 255);
	static constexpr ImU32 COLOR_PURPLE = IM_COL32(255, 0, 255, 255);
	static constexpr ImU32 COLOR_TEAL = IM_COL32(0, 255, 255, 255);
	static constexpr ImU32 COLOR_GRAY = IM_COL32(192, 192, 192, 255);

	static std::wstring s2ws(const std::string& str) {
		USES_CONVERSION;
		return std::wstring(A2W(str.c_str()));
	}

	static std::string ws2s(const std::wstring& wstr) {
		USES_CONVERSION;
		return std::string(W2A(wstr.c_str()));
	}

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

	bool IsTerminating() const { return bTerminate; }
	void RenderArea(std::function<void(void)> func) const;
	void Clear() const { RenderArea([] {}); }
	void Resize(float X, float Y, float W, float H) { PosX = X; PosY = Y; Width = W; Height = H; }
	RECT GetClientRect() const { return { (LONG)PosX, (LONG)PosY, (LONG)(PosX + Width), (LONG)(PosY + Height) }; }
	void InsertMouseInfo() const;

	void HelpMarker(const char* szText) const;
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

	static ImVec2 GetTextSize(const char* szText) {
		const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];
		return pFont->CalcTextSizeA(FONTSIZE_SMALL, FLT_MAX, 0.0f, szText, 0, 0);
	}

	Hack(Process& process, Render& render, int ScreenWidth, int ScreenHeight, char* szBuf, size_t nBufSize)
		: process(process), kernel(process.handler), dbvm(kernel.dbvm), render(render), szBuf(szBuf), nBufSize(nBufSize) {
		Clear();
		Resize(0.0f, 0.0f, (float)ScreenWidth, (float)ScreenHeight);
	}

	void printlog(const char* fmt, ...) {
#ifdef LOGPRINT
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
#else
#endif
	}

	void DrawHotkey() const {
		if (!bESP) return;

		if (NoticeTimeRemain == 0)
			return;

		szBuf[0] = 0;

		if (bFighterMode)	strcat(szBuf, "Fighter Mode(~) : ON"e);
		else				strcat(szBuf, "Fighter Mode(~) : OFF"e);
		strcat(szBuf, "\n"e);

		if (bESP)			strcat(szBuf, "ESP(F1) : ON"e);
		else				strcat(szBuf, "ESP(F1) : OFF"e);
		strcat(szBuf, "\n"e);

		if (bVehicle)		strcat(szBuf, "Vehicle(F2) : ON"e);
		else				strcat(szBuf, "Vehicle(F2) : OFF"e);
		strcat(szBuf, "\n"e);

		if (bBox)			strcat(szBuf, "Box(F3) : ON"e);
		else				strcat(szBuf, "Box(F3) : OFF"e);
		strcat(szBuf, "\n"e);

		if (nItem == 0)		strcat(szBuf, "Item(F4) : OFF"e);
		if (nItem == 1)		strcat(szBuf, "Item(F4) : 1"e);
		if (nItem == 2)		strcat(szBuf, "Item(F4) : 2"e);
		if (nItem == 3)		strcat(szBuf, "Item(F4) : 3"e);
		if (nItem == 4)		strcat(szBuf, "Item(F4) : 4"e);
		strcat(szBuf, "\n"e);

		if (nAimbot == 0)	strcat(szBuf, "Aimbot(F5) : OFF"e);
		if (nAimbot == 1)	strcat(szBuf, "Aimbot(F5) : Normal"e);
		if (nAimbot == 2)	strcat(szBuf, "Aimbot(F5) : Silent"e);
		if (nAimbot == 3)	strcat(szBuf, "Aimbot(F5) : Silent (Dangerous!)"e);
		strcat(szBuf, "\n"e);

		if (bTurnBackShortKey)	strcat(szBuf, "Enable TurnBack Shortkey(F6, CapsLock) : ON"e);
		else					strcat(szBuf, "Enable TurnBack Shortkey(F6, CapsLock) : OFF"e);
		strcat(szBuf, "\n"e);

		if (bTeamKill)		strcat(szBuf, "TeamKill(F10) : ON"e);
		else				strcat(szBuf, "TeamKill(F10) : OFF"e);
		strcat(szBuf, "\n"e);

		strcat(szBuf, "Current Rnage (+-) : "e);
		strcat(szBuf, std::to_string(nRange).c_str());
		strcat(szBuf, "M"e);
		DrawNotice(szBuf, COLOR_TEAL);
	}

	void ProcessImGui() {
		if (!bESP) return;

		if (!bShowMenu)
			return;

		const float MenuSizeX = 320.0f;
		const float MenuSizeY = 450.0f;

		const float MenuPosX = Width - MenuSizeX - MARGIN;
		const float MenuPosY = Height / 2.0f - MenuSizeY / 2.0f;

		ImGui::SetNextWindowBgAlpha(0.4f);
		ImGui::SetNextWindowPos({ MenuPosX, MenuPosY }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ MenuSizeX, MenuSizeY }, ImGuiCond_Once);
		if (ImGui::Begin("Hotkey works holding mouse m-button or alt key"e))
		{
			ImGui::Text("Range (+,-)"e);
			ImGui::PushItemWidth(-1);
			ImGui::SliderInt("Range"e, &nRange, 100, 1000, "%dM"e, ImGuiSliderFlags_AlwaysClamp);
			ImGui::PopItemWidth();

			ImGui::Checkbox("Enable turnback (F6, CapsLock)"e, (bool*)&bTurnBackShortKey);
			ImGui::Checkbox("Fighter Mode (~)"e, (bool*)&bFighterMode);

			ImGui::NewLine();

			if (ImGui::BeginTabBar("MyTabBar"e))
			{
				if (ImGui::BeginTabItem("Visual"e))
				{
					ImGui::Checkbox("ESP (F1)"e, &bESP);
					ImGui::Checkbox("Vehicle (F2)"e, &bVehicle);
					ImGui::Checkbox("Box (F3)"e, &bBox);
					ImGui::Checkbox("Item (F4)"e, (bool*)&nItem);
					if (nItem > 0) {
						ImGui::PushItemWidth(-1);
						ImGui::SliderInt("Item"e, &nItem, 1, 4, "%d"e, ImGuiSliderFlags_AlwaysClamp);
						ImGui::PopItemWidth();
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Aimbot"e))
				{
					ImGui::Checkbox("Aimbot (F5)"e, (bool*)&nAimbot);
					if (nAimbot > 0) {
						ImGui::RadioButton("Normal"e, &nAimbot, 1);
						ImGui::RadioButton("Silent"e, &nAimbot, 2);
						ImGui::RadioButton("Silent (Dangerous!)"e, &nAimbot, 3);
					}

					ImGui::Text("Fov"e);
					ImGui::PushItemWidth(-1);
					ImGui::SliderFloat("Fov"e, &CircleFovInDegrees, 6.0f, 12.0f);
					ImGui::PopItemWidth();

					ImGui::Checkbox("TeamKill (F10)"e, &bTeamKill);

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Debug"e))
				{
					ImGui::Checkbox("Debug mode (F12)"e, &bDebug);

#ifdef LOGPRINT
					ImGui::Checkbox("Capture Log"e, &bCaptureLog);
					ImGui::InputTextMultiline("##source"e, debuglog.data(), debuglog.size(), {-FLT_MIN, -FLT_MIN}, ImGuiInputTextFlags_ReadOnly);
					if (bNeedToScroll) {
						ImGui::BeginChild("##source"e);
						ImGui::SetScrollHereY(1.0f);
						bNeedToScroll = false;
						ImGui::EndChild();
					}
#endif

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

	void ProcessHotkey() {
		bTurnBack = false;
		if (bTurnBackShortKey) {
			if (IsKeyPushed(VK_CAPITAL)) {
				bTurnBack = true;
			}
		}
		if (IsKeyPushing(VK_MENU) || IsKeyPushing(VK_MBUTTON)) {
			if (IsKeyPushed(VK_HOME)) {
				bShowMenu = !bShowMenu;
			}
			if (IsKeyPushed(VK_OEM_3)) {
				bFighterMode = !bFighterMode;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F1)) {
				bESP = !bESP;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F2)) {
				bVehicle = !bVehicle;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F3)) {
				bBox = !bBox;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F4)) {
				nItem = (nItem + 1) % 5;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F5)) {
				nAimbot = (nAimbot + 1) % 4;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F6)) {
				bTurnBackShortKey = !bTurnBackShortKey;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F10)) {
				bTeamKill = !bTeamKill;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_F12)) {
				bDebug = !bDebug;
				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_ADD)) {
				nRange += 100;
				nRange -= nRange % 100;
				if (nRange > 1000)
					nRange = 1000;

				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_SUBTRACT)) {
				if (nRange % 100 == 0)
					nRange -= 100;
				nRange -= nRange % 100;
				if (nRange < 100)
					nRange = 100;

				NoticeTimeRemain = NOTICE_TIME;
			}
			if (IsKeyPushed(VK_END)) {
				bTerminate = true;
			}
		}

		bPushingMouseM = IsKeyPushing(VK_MBUTTON);
		bPushingMouseL = IsKeyPushing(VK_LBUTTON);
		bPushingShift = IsKeyPushing(VK_LSHIFT);
		bPushingCTRL = IsKeyPushing(VK_LCONTROL);
		bPushingMouseR = IsKeyPushing(VK_RBUTTON);
	}

	static Vector WorldToScreen(const Vector& WorldLocation, const Matrix& RotationMatrix, const Vector& CameraLocation, float CameraFOV, float Width, float Height) {
		Vector Screenlocation(0, 0, 0);

		Vector AxisX = RotationMatrix.GetScaledAxisX();
		Vector AxisY = RotationMatrix.GetScaledAxisY();
		Vector AxisZ = RotationMatrix.GetScaledAxisZ();

		Vector vDelta(WorldLocation - CameraLocation);
		Vector vTransformed(vDelta | AxisY, vDelta | AxisZ, vDelta | AxisX);

		if (vTransformed.Z == 0.0)
			vTransformed.Z = -0.001f;

		Screenlocation.Z = vTransformed.Z;

		if (vTransformed.Z < 0.0f)
			vTransformed.Z = -vTransformed.Z;

		float ScreenCenterX = Width / 2.0f;
		float ScreenCenterY = Height / 2.0f;
		float TangentFOV = tanf(ConvertToRadians(CameraFOV / 2.0f));

		Screenlocation.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / TangentFOV) / vTransformed.Z;
		Screenlocation.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / TangentFOV) / vTransformed.Z;
		return Screenlocation;
	}

	Vector WorldToScreen(const Vector& WorldLocation, const Matrix& RotationMatrix, const Vector& CameraLocation, float CameraFOV) const {
		return WorldToScreen(WorldLocation, RotationMatrix, CameraLocation, CameraFOV, Width, Height);
	}

	void Loop(Process& process);
};
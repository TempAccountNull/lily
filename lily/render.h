#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "vector.h"
#include "encrypt_string.h"

struct IDirect3DDevice9Ex;

class __declspec(novtable) Render {
private:
	float PosX = 0.0f, PosY = 0.0f;
	float Width = (float)ScreenWidth, Height = (float)ScreenHeight;

	void ImGuiRenderDrawData() const;

	void ProcessExitHotkey() const {
		if (!IsKeyPushing(VK_MENU) && !IsKeyPushing(VK_MBUTTON))
			return;
		if (!IsKeyPushed(VK_END))
			return;
		Clear();
		exit(0);
	}

	void UpdateRenderArea(HWND hWnd) {
		RECT ClientRect;
		GetClientRect(hWnd, &ClientRect);
		POINT ClientPos = { 0, 0 };
		ClientToScreen(hWnd, &ClientPos);
		PosX = (float)ClientPos.x;
		PosY = (float)ClientPos.y;
		Width = (float)ClientRect.right;
		Height = (float)ClientRect.bottom;
	}

	void InsertMouseInfo() const {
		if (!bRender) return;

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = IsKeyPushing(VK_LBUTTON);
		io.MouseDown[1] = IsKeyPushing(VK_RBUTTON);
		io.MouseDown[2] = IsKeyPushing(VK_MBUTTON);

		POINT CursorPos;
		GetCursorPos(&CursorPos);
		io.MousePos.x = (float)CursorPos.x - PosX;
		io.MousePos.y = (float)CursorPos.y - PosY;
	}

protected:
	constexpr static auto COLOR_CLEAR = RGB(1, 1, 1);

	void Clear() const {
		ImGui_ImplDX9_NewFrame();
		ImGui::NewFrame();
		ImGui::EndFrame();
		ImGuiRenderDrawData();
		Present(0);
	}

	virtual void Present(HWND hGameWnd) const;

public:
	constexpr static unsigned MARGIN = 10;

	constexpr static float FONTSIZE_SMALL = 15.0f;
	constexpr static float FONTSIZE_NORMAL = 20.0f;
	constexpr static float FONTSIZE_BIG = 40.0f;

	constexpr static ImU32 COLOR_RED = IM_COL32(255, 0, 0, 255);
	constexpr static ImU32 COLOR_GREEN = IM_COL32(0, 255, 0, 255);
	constexpr static ImU32 COLOR_BLUE = IM_COL32(0, 0, 255, 255);
	constexpr static ImU32 COLOR_BLACK = IM_COL32(0, 0, 0, 255);
	constexpr static ImU32 COLOR_WHITE = IM_COL32(255, 255, 255, 255);
	constexpr static ImU32 COLOR_ORANGE = IM_COL32(255, 128, 0, 255);
	constexpr static ImU32 COLOR_YELLOW = IM_COL32(255, 255, 0, 255);
	constexpr static ImU32 COLOR_PURPLE = IM_COL32(255, 0, 255, 255);
	constexpr static ImU32 COLOR_TEAL = IM_COL32(0, 255, 255, 255);
	constexpr static ImU32 COLOR_GRAY = IM_COL32(192, 192, 192, 255);

	const int ScreenWidth, ScreenHeight;

	bool bRender = true;

	float GetPosX() const { return PosX; }
	float GetPosY() const { return PosY; }
	float GetWidth() const { return Width; }
	float GetHeight() const { return Height; }

	IDirect3DDevice9Ex* const pDirect3DDevice9Ex;
	Render(IDirect3DDevice9Ex* pDirect3DDevice9Ex, int ScreenWidth, int ScreenHeight) : 
		pDirect3DDevice9Ex(pDirect3DDevice9Ex), ScreenWidth(ScreenWidth), ScreenHeight(ScreenHeight) {}

	void RenderArea(HWND hWnd, auto func) {
		if (hWnd == GetForegroundWindow())
			InsertMouseInfo();

		ProcessExitHotkey();
		UpdateRenderArea(hWnd);

		ImGui_ImplDX9_NewFrame();
		ImGui::NewFrame();

		auto Viewport = ImGui::GetMainViewport();
		Viewport->Pos = { -PosX, -PosY };
		Viewport->WorkPos = { 0.0f, 0.0f };
		Viewport->Size = { PosX + Width, PosY + Height };
		Viewport->WorkSize = { Width, Height };

		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::SetNextWindowSize({ Width, Height });
		ImGui::Begin("lily"e, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

		func();

		ImGui::End();
		ImGui::EndFrame();
		ImGuiRenderDrawData();
		Present(hWnd);
	}

	static ImVec2 GetTextSize(float FontSize, const char* szText);
	void DrawString(const Vector& Pos, float Margin, const char* szText, float Size, ImU32 Color, bool bCenterPos, bool bCenterAligned, bool bShowAlways) const;
	void DrawRatioBox(const Vector& from, const Vector& to, float HealthRatio, ImU32 ColorRemain, ImU32 ColorDamaged, ImU32 ColorEdge) const;
	void DrawLine(const Vector& from, const Vector& to, ImU32 Color, float thickness = 1.0f) const;
	void DrawCircle(const ImVec2& center, float radius, ImU32 Color, int num_segments = 0, float thickness = 1.0f) const;
	void DrawRectOutlined(const Vector& from, const Vector& to, ImU32 Color, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f) const;
	void DrawRectFilled(const Vector& from, const Vector& to, ImU32 Color, float rounding = 0.0f, ImDrawFlags flags = 0) const;
};
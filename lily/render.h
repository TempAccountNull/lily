#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "ue4math/vector.h"
#include "common/encrypt_string.h"
#include "common/util.h"

class __declspec(novtable) Render {
private:
	float PosX = 0, PosY = 0;
	float Width = (float)ScreenWidth, Height = (float)ScreenHeight;

	void ImGuiRenderDrawData() const;

	void ProcessExitHotkey() {
		if (!IsKeyPushing(VK_MENU) && !IsKeyPushing(VK_MBUTTON))
			return;
		if (!IsKeyPushed(VK_END))
			return;
		Clear();

		this->~Render();
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

	uint64_t PrevFPSTime = GetTickCountInMicroSeconds();
	uint64_t PrevTime = GetTickCountInMicroSeconds();
	uint32_t FPSCount = 0;
	uint32_t FPS = 0;
	float TimeDelta = 0;

	void UpdateTimeDelta() {
		FPSCount++;
		const uint64_t CurrTime = GetTickCountInMicroSeconds();
		const uint64_t Delta = CurrTime - PrevTime;

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
		TimeDelta = (float)Delta / 1000000;
	}

protected:
	template<class Interface>
	using ComPtr = Microsoft::WRL::ComPtr<Interface>;

	const ComPtr<ID3D11Device> pD3D11Device;
	const ComPtr<ID3D11DeviceContext> pD3D11DeviceContext;

	constexpr static auto CLEARVAL = 0;

	constexpr static auto COLOR_CLEAR_RGB = RGB(CLEARVAL, CLEARVAL, CLEARVAL);
	const ImVec4 COLOR_CLEAR_VEC4 = ImVec4(CLEARVAL / 255.0f, CLEARVAL / 255.0f, CLEARVAL / 255.0f, 0.0f);

	void Clear() {
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		ImGui::EndFrame();
		ImGuiRenderDrawData();
		Present(0);
	}

	virtual void Present(HWND hWnd);
	virtual ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const;
	virtual bool IsScreenPosNeeded() const;
	
	Render(ComPtr<ID3D11Device> pD3D11Device, ComPtr<ID3D11DeviceContext> pD3D11DeviceContext, int ScreenWidth, int ScreenHeight) :
		pD3D11Device(pD3D11Device), pD3D11DeviceContext(pD3D11DeviceContext), ScreenWidth(ScreenWidth), ScreenHeight(ScreenHeight) {}

	virtual ~Render() {}

public:
	constexpr static unsigned MARGIN = 10;

	constexpr static float FONTSIZE_SMALL = 15.0f;
	constexpr static float FONTSIZE_NORMAL = 20;
	constexpr static float FONTSIZE_BIG = 40;

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
	float GetTimeDelta() const { return TimeDelta; }
	uint32_t GetFPS() const { return FPS; }

	void RenderArea(HWND hWnd, auto func) {
		ProcessExitHotkey();
		UpdateTimeDelta();
		UpdateRenderArea(hWnd);
		if (hWnd == GetForegroundWindow())
			InsertMouseInfo();

		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		auto Viewport = ImGui::GetMainViewport();
		Viewport->Pos = IsScreenPosNeeded() ? ImVec2(-PosX, -PosY) : ImVec2(0.0f, 0.0f);
		Viewport->Size = IsScreenPosNeeded() ? ImVec2(PosX + Width, PosY + Height) : ImVec2(Width, Height);
		Viewport->WorkPos = { 0.0f, 0.0f };
		Viewport->WorkSize = { Width, Height };

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ Width, Height });
		ImGui::Begin("lily"e, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

		func();

		ImGui::End();
		ImGui::EndFrame();
		ImGuiRenderDrawData();
		Present(hWnd);
	}

	static ImVec2 GetTextSize(float FontSize, const char* szText);
	void DrawString(const FVector& Pos, float Margin, const char* szText, float Size, ImU32 Color, bool bCenterPos, bool bCenterAligned, bool bShowAlways) const;
	void DrawRatioBox(const FVector& from, const FVector& to, float HealthRatio, ImU32 ColorRemain, ImU32 ColorDamaged, ImU32 ColorEdge) const;
	void DrawLine(const FVector& from, const FVector& to, ImU32 Color, float thickness = 1.0f) const;
	void DrawCircle(const ImVec2& center, float radius, ImU32 Color, int num_segments = 0, float thickness = 1.0f) const;
	void DrawRectOutlined(const FVector& from, const FVector& to, ImU32 Color, float rounding = 0, ImDrawFlags flags = 0, float thickness = 1.0f) const;
	void DrawRectFilled(const FVector& from, const FVector& to, ImU32 Color, float rounding = 0, ImDrawFlags flags = 0) const;
};
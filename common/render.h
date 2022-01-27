#pragma once
#include <Windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

#include "../ue4math/vector.h"
#include "encrypt_string.h"
#include "util.h"

class __declspec(novtable) Render {
public:
	const int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

protected:
	template<class Interface>
	using ComPtr = Microsoft::WRL::ComPtr<Interface>;

private:
	float PosX = 0;
	float PosY = 0;
	float _Width = (float)ScreenWidth;
	float _Height = (float)ScreenHeight;

	virtual void Present(HWND hWnd);
	virtual ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const;
	virtual bool IsScreenPosNeeded() const;

	void ImGuiRenderDrawData() const {
		ImGui::Render();
		pD3D11DeviceContext->OMSetRenderTargets(1, GetRenderTargetView().GetAddressOf(), 0);
		pD3D11DeviceContext->ClearRenderTargetView(GetRenderTargetView().Get(), COLOR_CLEAR_FLOAT4);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

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
		_Width = (float)ClientRect.right;
		_Height = (float)ClientRect.bottom;
	}

	void InsertMouseInfo() const {
		if (!bRender) return;

		ImGuiIO& io = ImGui::GetIO();
		if (io.BackendPlatformUserData)
			return;

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
	uint32_t _FPS = 0;
	float _TimeDelta = 0;

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
			_FPS = FPSCount;
			FPSCount = 0;
			dprintf("%d"e, FPS);
		}

		PrevTime = CurrTime;
		_TimeDelta = (float)Delta / 1000000;
	}

protected:
	constexpr static float COLOR_CLEAR_FLOAT4[4] = { 1.0f / 255.0f , 1.0f / 255.0f , 1.0f / 255.0f , 0.0f };

	const ComPtr<ID3D11Device> pD3D11Device;
	const ComPtr<ID3D11DeviceContext> pD3D11DeviceContext;

	void Clear() {
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		ImGui::EndFrame();
		ImGuiRenderDrawData();
		Present(0);
	}

	Render(float DefaultFontSize) {
		const HRESULT hr = D3D11CreateDevice(0,
			D3D_DRIVER_TYPE_HARDWARE, 0,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT, 0, 0, D3D11_SDK_VERSION,
			(ID3D11Device**)pD3D11Device.GetAddressOf(), 0,
			(ID3D11DeviceContext**)pD3D11DeviceContext.GetAddressOf());

		verify(SUCCEEDED(hr));

		ImGui::CreateContext();
		ImGui_ImplDX11_Init(pD3D11Device.Get(), pD3D11DeviceContext.Get());

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = { (float)ScreenWidth , (float)ScreenHeight };

		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf"e, DefaultFontSize);
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf"e, 60.0f);
		io.IniFilename = 0;
	}

	virtual ~Render() {
		ImGui_ImplDX11_Shutdown();
		ImGuiIO& io = ImGui::GetIO();
		if (io.BackendPlatformUserData)
			ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

public:
	constexpr static ImU32 COLOR_CLEAR = IM_COL32(0, 0, 0, 0);
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

	bool bRender = true;

	const float& Width = _Width;
	const float& Height = _Height;
	const float& TimeDelta = _TimeDelta;
	const uint32_t& FPS = _FPS;

	void RenderArea(HWND hWnd, ImColor BgColor, auto func) {
		ProcessExitHotkey();
		UpdateTimeDelta();
		UpdateRenderArea(hWnd);
		if (hWnd == GetForegroundWindow())
			InsertMouseInfo();

		ImGuiIO& io = ImGui::GetIO();
		if (io.BackendPlatformUserData)
			ImGui_ImplWin32_NewFrame();

		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		auto Viewport = ImGui::GetMainViewport();
		Viewport->Pos = IsScreenPosNeeded() ? ImVec2(-PosX, -PosY) : ImVec2(0.0f, 0.0f);
		Viewport->Size = IsScreenPosNeeded() ? ImVec2(PosX + Width, PosY + Height) : ImVec2(Width, Height);
		Viewport->WorkPos = { 0.0f, 0.0f };
		Viewport->WorkSize = { Width, Height };

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ Width, Height });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)BgColor);
		ImGui::Begin("root"e, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		func();

		ImGui::End();
		ImGui::EndFrame();
		ImGuiRenderDrawData();
		Present(hWnd);
	}

	static ImVec2 GetTextSize(float FontSize, const char* szText);
	void DrawString(const FVector& Pos, float Margin, const char* szText, float Size, ImColor Color, bool bCenterPos, bool bCenterAligned, bool bShowAlways) const;
	void DrawRatioBox(const FVector& from, const FVector& to, float HealthRatio, ImColor ColorRemain, ImColor ColorDamaged, ImColor ColorEdge) const;
	void DrawLine(const FVector& from, const FVector& to, ImColor Color, float thickness = 1.0f) const;
	void DrawCircle(const ImVec2& center, float radius, ImColor Color, int num_segments = 0, float thickness = 1.0f) const;
	void DrawRectOutlined(const FVector& from, const FVector& to, ImColor Color, float rounding = 0, ImDrawFlags flags = 0, float thickness = 1.0f) const;
	void DrawRectFilled(const FVector& from, const FVector& to, ImColor Color, float rounding = 0, ImDrawFlags flags = 0) const;
};
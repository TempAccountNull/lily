#include "hack.h"
#include "vector.h"
#include "encrypt_string.hpp"
#include "render.h"

void AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* szText, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = 0) {
	ImGui::GetWindowDrawList()->AddText(font, font_size, pos, col, szText, 0, wrap_width, cpu_fine_clip_rect);
}

void AddTextOutlined(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* szText, float wrap_width, const ImVec4* cpu_fine_clip_rect) {
	AddText(font, font_size, { pos.x + 1, pos.y + 1 }, IM_COL32_BLACK, szText, wrap_width, cpu_fine_clip_rect);
	AddText(font, font_size, { pos.x - 1, pos.y - 1 }, IM_COL32_BLACK, szText, wrap_width, cpu_fine_clip_rect);
	AddText(font, font_size, { pos.x + 1, pos.y - 1 }, IM_COL32_BLACK, szText, wrap_width, cpu_fine_clip_rect);
	AddText(font, font_size, { pos.x - 1, pos.y + 1 }, IM_COL32_BLACK, szText, wrap_width, cpu_fine_clip_rect);
	AddText(font, font_size, pos, col, szText, wrap_width, cpu_fine_clip_rect);
}

void Hack::RenderArea(std::function<void(void)> func) const {
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
	render.Present(*this);
}

void Hack::InsertMouseInfo() const {
	if (!bESP) return;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[0] = IsKeyPushing(VK_LBUTTON);
	io.MouseDown[1] = IsKeyPushing(VK_RBUTTON);
	io.MouseDown[2] = IsKeyPushing(VK_MBUTTON);

	POINT CursorPos;
	GetCursorPos(&CursorPos);
	io.MousePos.x = (float)CursorPos.x - PosX;
	io.MousePos.y = (float)CursorPos.y - PosY;
}

void Hack::HelpMarker(const char* szText) const {
	if (!bESP) return;

	char szHelp[] = { '(', '?', ')', 0 };
	ImGui::TextDisabled(szHelp);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(szText);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Hack::DrawString(const Vector& Pos, float Margin, const char* szText, float Size, ImU32 Color, bool bCenterPos, bool bCenterAligned, bool bShowAlways) const {
	if (!bESP) return;

	if (Pos.Z < 0.0f && !bShowAlways)
		return;

	ImVec2 LeftTopPos = { Pos.X, Pos.Y };

	const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];
	ImVec2 TextSize = pFont->CalcTextSizeA(Size, FLT_MAX, 0.0f, szText, 0, 0);

	float Width = ImGui::GetMainViewport()->WorkSize.x;
	float Height = ImGui::GetMainViewport()->WorkSize.y;

	if (bCenterPos)
		LeftTopPos = { LeftTopPos.x - (TextSize.x / 2.0f), LeftTopPos.y - (TextSize.y / 2.0f) };

	if (Pos.Z < 0.0f)
		LeftTopPos.y = FLT_MAX;

	if (bShowAlways) {
		const float MinPosX = Margin;
		const float MaxPosX = Width - Margin - TextSize.x;

		const float MinPosY = Margin;
		const float MaxPosY = Height - Margin - TextSize.y;

		LeftTopPos.x = std::max(LeftTopPos.x, MinPosX);
		LeftTopPos.x = std::min(LeftTopPos.x, MaxPosX);

		LeftTopPos.y = std::max(LeftTopPos.y, MinPosY);
		LeftTopPos.y = std::min(LeftTopPos.y, MaxPosY);
	}

	if (bCenterAligned) {
		float CurrentPosY = LeftTopPos.y;
		size_t pos = 0;
		while (1) {
			std::string text = szText;
			size_t cpos = text.find('\n', pos);
			std::string strLine = text.substr(pos, cpos - pos);

			//print line
			ImVec2 LineTextSize = pFont->CalcTextSizeA(Size, FLT_MAX, 0.0f, strLine.c_str(), 0, 0);
			ImVec2 CurrentPos = { LeftTopPos.x + (TextSize.x / 2.0f) - (LineTextSize.x / 2.0f), CurrentPosY };
			AddTextOutlined(pFont, Size, CurrentPos, Color, strLine.c_str(), 0.0f, 0);

			if (cpos == std::string::npos)
				break;

			CurrentPosY += LineTextSize.y;
			pos = cpos + 1;
		}
	}
	else {
		AddTextOutlined(pFont, Size, LeftTopPos, Color, szText, 0.0f, 0);
	}
}

void Hack::DrawString(const Vector& pos, const char* szText, ImU32 Color, bool bShowAlways) const {
	if (!bESP) return;

	DrawString(pos, MARGIN, szText, FONTSIZE_SMALL, Color, true, true, bShowAlways);
}

void Hack::DrawNotice(const char* szText, ImU32 Color) const {
	if (!bESP) return;

	DrawString({ 30.0f, 30.0f , 0.0f }, MARGIN, szText, FONTSIZE_NORMAL, Color, false, false, true);
}

void Hack::DrawFPS(unsigned FPS, ImU32 Color) const {
	if (!bESP) return;

	float Width = ImGui::GetMainViewport()->WorkSize.x;
	sprintf(szBuf, "FPS : %d"e, FPS);
	DrawString({ Width * 0.6f, 0.0f , 0.0f }, MARGIN, szBuf, FONTSIZE_SMALL, Color, true, true, true);
}

void Hack::DrawZeroingDistance(float ZeroingDistance, ImU32 Color) const {
	if (!bESP) return;

	float Width = ImGui::GetMainViewport()->WorkSize.x;
	sprintf(szBuf, "Zero : %.0f"e, ZeroingDistance);
	DrawString({ Width * 0.5f, 0.0f , 0.0f }, MARGIN, szBuf, FONTSIZE_SMALL, Color, true, true, true);
}

void Hack::DrawSpectatedCount(unsigned SpectatedCount, ImU32 Color) const {
	if (!bESP) return;

	float Width = ImGui::GetMainViewport()->WorkSize.x;
	sprintf(szBuf, "Spectators : %d"e, SpectatedCount);
	DrawString({ Width * 0.4f, 0.0f , 0.0f }, MARGIN, szBuf, FONTSIZE_SMALL, Color, true, true, true);
}

void Hack::DrawRectOutlined(const Vector& from, const Vector& to, ImU32 Color, float rounding, ImDrawFlags flags, float thickness) const {
	if (!bESP) return;

	if (from.Z < 0.0f) return;
	ImGui::GetWindowDrawList()->AddRect({ from.X, from.Y }, { to.X, to.Y }, Color, rounding, flags, thickness);
}

void Hack::DrawRectFilled(const Vector& from, const Vector& to, ImU32 Color, float rounding, ImDrawFlags flags) const {
	if (!bESP) return;

	if (from.Z < 0.0f) return;
	ImGui::GetWindowDrawList()->AddRectFilled({ from.X, from.Y }, { to.X, to.Y }, Color, rounding, flags);
}

void Hack::DrawRatioBox(const Vector& from, const Vector& to, float Ratio, ImU32 ColorRemain, ImU32 ColorDamaged, ImU32 ColorEdge) const {
	if (!bESP) return;

	if (from.Z < 0.0f) return;

	float RemainLen = (to.X - from.X) * Ratio;
	if (Ratio < 1.0f && Ratio > 0.0f)
		RemainLen = ceilf(RemainLen);

	const Vector RemainFrom = { from.X, from.Y, 0.0f };
	const Vector RemainTo = { from.X + RemainLen, to.Y, 0.0f };
	const Vector DamagedFrom = { from.X + RemainLen, from.Y, 0.0f };
	const Vector DamagedTo = { to.X, to.Y, 0.0f };

	DrawRectFilled(DamagedFrom, DamagedTo, ColorDamaged);
	DrawRectFilled(RemainFrom, RemainTo, ColorRemain);
	DrawRectOutlined(from, to, ColorEdge);
}

void Hack::DrawLine(const Vector& from, const Vector& to, ImU32 Color, float thickness) const {
	if (!bESP) return;

	if (from.Z < 0.0f) return;
	ImGui::GetWindowDrawList()->AddLine({ from.X, from.Y }, { to.X, to.Y }, Color, thickness);
}

void Hack::DrawCircle(const ImVec2& center, float radius, ImU32 Color, int num_segments, float thickness) const {
	if (!bESP) return;

	ImGui::GetWindowDrawList()->AddCircle(center, radius, Color, num_segments, thickness);
}
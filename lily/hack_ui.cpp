#include "hack.h"

void Hack::DrawHotkey() const {
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
	DrawNotice(szBuf, Render::COLOR_TEAL);
}

void Hack::ProcessImGui() {
	if (!bESP) return;

	if (!bShowMenu)
		return;

	const float MenuSizeX = 320.0f;
	const float MenuSizeY = 450.0f;

	const float MenuPosX = render.GetWidth() - MenuSizeX - Render::MARGIN;
	const float MenuPosY = render.GetHeight() / 2.0f - MenuSizeY / 2.0f;

	ImGui::SetNextWindowBgAlpha(0.4f);
	ImGui::SetNextWindowPos({ MenuPosX, MenuPosY }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ MenuSizeX, MenuSizeY }, ImGuiCond_Once);
	if (ImGui::Begin("Hotkey works holding mouse m-button or alt key"e))
	{
		ImGui::Text("Update : " __DATE__ " " __TIME__ ""e);
		ImGui::NewLine();

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

				ImGui::Checkbox("Capture Log"e, &bCaptureLog);
				ImGui::InputTextMultiline("##source"e, debuglog.data(), debuglog.size(), { -FLT_MIN, -FLT_MIN }, ImGuiInputTextFlags_ReadOnly);
				if (bNeedToScroll) {
					ImGui::BeginChild("##source"e);
					ImGui::SetScrollHereY(1.0f);
					bNeedToScroll = false;
					ImGui::EndChild();
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void Hack::ProcessHotkey() {
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
	}

	bPushingMouseM = IsKeyPushing(VK_MBUTTON);
	bPushingMouseL = IsKeyPushing(VK_LBUTTON);
	bPushingShift = IsKeyPushing(VK_LSHIFT);
	bPushingCTRL = IsKeyPushing(VK_LCONTROL);
	bPushingMouseR = IsKeyPushing(VK_RBUTTON);
}
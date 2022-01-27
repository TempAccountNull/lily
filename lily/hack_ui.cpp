#include "hack.h"

void Hack::DrawHotkey() const {
	if (!bESP) return;

	if (NoticeTimeRemain == 0)
		return;

	std::string strNotice;

	auto AddOnOff = [&](const std::string& strFuncName, bool bOnOff) {
		strNotice += strFuncName;
		if (bOnOff)
			strNotice += (std::string)" : ON\n"e;
		else
			strNotice += (std::string)" : OFF\n"e;
	};

	AddOnOff("Fighter Mode(~)"e, bFighterMode);
	AddOnOff("ESP(F1)"e, bESP);
	AddOnOff("Vehicle(F2)"e, bVehicle);
	AddOnOff("Box(F3)"e, bBox);

	strNotice += (std::string)"Item(F4) : "e;
	strNotice += nItem > 0 ? std::to_string(nItem) : "OFF"e;
	strNotice += (std::string)"\n"e;

	strNotice += (std::string)"Aimbot(F5) : "e;
	strNotice +=
		nAimbot == 1 ? (std::string)"Normal"e :
		nAimbot == 2 ? (std::string)"Silent"e :
		nAimbot == 3 ? (std::string)"Aimbot(F5) : Silent (Dangerous!)"e :
		"OFF"e;
	strNotice += (std::string)"\n"e;

	AddOnOff("TeamKill(F10)"e, bTeamKill);

	strNotice += (std::string)"Current Rnage (+-) : "e;
	strNotice += std::to_string(nRange);
	strNotice += (std::string)"M"e;
	DrawNotice(strNotice.c_str(), Render::COLOR_TEAL);
}

void Hack::ProcessImGui() {
	if (!bESP) return;

	if (!bShowMenu)
		return;

	const float MenuSizeX = 320.0f;
	const float MenuSizeY = 450.0f;

	const float MenuPosX = render.Width - MenuSizeX - Hack::MARGIN;
	const float MenuPosY = render.Height / 2.0f - MenuSizeY / 2.0f;

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

				ImGui::Text("Aim Speed(X,Y)"e);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("SpeedX"e, &AimbotSpeedX, DefaultAimbotSpeedMin, DefaultAimbotSpeedMax, "%.3f"e, ImGuiSliderFlags_Logarithmic);
				ImGui::SliderFloat("SpeedY"e, &AimbotSpeedY, DefaultAimbotSpeedMin, DefaultAimbotSpeedMax, "%.3f"e, ImGuiSliderFlags_Logarithmic);
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
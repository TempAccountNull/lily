#pragma once
#include <d3d9.h>

#include "dbvm.h"
#include "lily.h"
#include "encrypt_string.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#define INI ".\\config.ini"e
#define APP "INJECTOR"e

enum class EInjectionType {
	Normal,
	NxBitSwap,
	IntoDLL
};

class InjectorUI {
private:
	DBVM dbvm;

	bool bInjected = false;
	HWND hWnd = 0;
	IDirect3DDevice9Ex* pDirect3DDevice9Ex = 0;

	char szProcessName[0x100] = { 0 };
	bool bCreateProcess = false;
	char szImageName[0x100] = { 0 };
	char szIntoDLL[0x100] = { 0 };
	char szParam[0x100] = { 0 };
	EInjectionType InjectionType = EInjectionType::Normal;

	char szServiceName[0x100] = { 0 };
	char szProcessEventName[0x100] = { 0 };
	char szThreadEventName[0x100] = { 0 };
	char szSysFileName[0x100] = { 0 };

	uint64_t default_password1 = 0xf26ec13d3110be5c;
	uint32_t default_password2 = 0xbcdc7ab6;
	uint64_t default_password3 = 0xc635c7311748ef9a;

	void GetDriver64Data() {
		FILE* in = fopen("driver64.dat"e, "r"e);
		if (!in)
			return;

		int Result = fscanf(in, "%s %s %s %s %I64x %x %I64x"e,
			szServiceName, szProcessEventName, szThreadEventName, szSysFileName,
			&default_password1, &default_password2, &default_password3);

		fclose(in);
		verify(Result == 7);
	}

	static void SetClientRect(HWND hWnd, int width, int height) {
		RECT crt;
		DWORD Style, ExStyle;

		SetRect(&crt, 0, 0, width, height);
		Style = GetWindowLong(hWnd, GWL_STYLE);
		ExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

		AdjustWindowRectEx(&crt, Style, GetMenu(hWnd) != NULL, ExStyle);
		if (Style & WS_VSCROLL) crt.right += GetSystemMetrics(SM_CXVSCROLL);
		if (Style & WS_HSCROLL) crt.bottom += GetSystemMetrics(SM_CYVSCROLL);
		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
			SWP_NOMOVE | SWP_NOZORDER);
	}

	void RenderArea(auto func) const {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		RECT Rect;
		GetClientRect(hWnd, &Rect);

		ImGui::SetNextWindowPos({ (float)Rect.left, (float)Rect.top });
		ImGui::SetNextWindowSize({ (float)Rect.right, (float)Rect.bottom });
		ImGui::Begin("Injector"e, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

		func();

		SetClientRect(hWnd, 500, (int)ImGui::GetCursorPosY());
		ImGui::End();
		ImGui::EndFrame();

		pDirect3DDevice9Ex->SetRenderState(D3DRS_ZENABLE, 0);
		pDirect3DDevice9Ex->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
		pDirect3DDevice9Ex->SetRenderState(D3DRS_SCISSORTESTENABLE, 0);
		pDirect3DDevice9Ex->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB(0, 0, 0), 1.0f, 0);
		if (pDirect3DDevice9Ex->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			pDirect3DDevice9Ex->EndScene();
		}

		pDirect3DDevice9Ex->Present(&Rect, &Rect, hWnd, 0);
	}

	bool SetPasswordFromParam();

	void OnButtonSetPassword();
	void OnButtonDBVM();
	void OnButtonInject();

	void GetDataFromINI() {
		GetPrivateProfileString(APP, "PROCESS"e, ""e, szProcessName, sizeof(szProcessName), INI);
		GetPrivateProfileString(APP, "IMAGE"e, ""e, szImageName, sizeof(szImageName), INI);
		GetPrivateProfileString(APP, "INTODLL"e, "", szIntoDLL, sizeof(szIntoDLL), INI);
		GetPrivateProfileString(APP, "PARAM"e, "", szParam, sizeof(szParam), INI);
		InjectionType = (EInjectionType)GetPrivateProfileIntA(APP, "TYPE"e, (int)EInjectionType::Normal, INI);
		bCreateProcess = GetPrivateProfileIntA(APP, "CREATE"e, false, INI);
	}

	void SaveDataToINI() const {
		WritePrivateProfileString(APP, "PROCESS"e, szProcessName, INI);
		WritePrivateProfileString(APP, "IMAGE"e, szImageName, INI);
		WritePrivateProfileString(APP, "INTODLL"e, szIntoDLL, INI);
		WritePrivateProfileString(APP, "PARAM"e, szParam, INI);
		char buffer[100];
		_itoa((int)InjectionType, buffer, 10);
		WritePrivateProfileString(APP, "TYPE"e, buffer, INI);
		_itoa(bCreateProcess, buffer, 10);
		WritePrivateProfileString(APP, "CREATE"e, buffer, INI);
	}

public:
	InjectorUI(HWND hWnd, IDirect3DDevice9Ex* pDirect3DDevice9Ex) : hWnd(hWnd), pDirect3DDevice9Ex(pDirect3DDevice9Ex) {
		GetDataFromINI();

		szServiceName << "93827461_CEDRIVER60"e;
		szProcessEventName << "93827461_DBKProcList60"e;
		szThreadEventName << "93827461_DBKThreadList60"e;
		szSysFileName << "dbk64.sys"e;

		GetDriver64Data();

		dbvm.SetPassword(default_password1, default_password2, default_password3);
		if (!dbvm.GetVersion()) {
			dbvm.SetDefaultPassword();
			if (dbvm.GetVersion())
				dbvm.ChangePassword(default_password1, default_password2, default_password3);
		}

		SetPasswordFromParam();
	}

	~InjectorUI() {
		SaveDataToINI();
	}

	bool ProcessFrame() {
		auto CreateRightAlignedItem = [&](auto f) {
			const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;

			//The 100.0f is just a guess size for the first frame.
			static float HostButtonWidth = 100.0f;
			float pos = HostButtonWidth + ItemSpacing;
			ImGui::SameLine(ImGui::GetWindowWidth() - pos);
			f();
			//Get the actual width for next frame.
			HostButtonWidth = ImGui::GetItemRectSize().x;
		};

		RenderArea([&] {
			if (dbvm.GetVersion())
				ImGui::Text("DBVM detected"e);
			else
				ImGui::Text("DBVM not detected"e);

			ImGui::Text("Process"e);
			ImGui::SameLine();
			float TextAlign = ImGui::GetCursorPosX();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
			ImGui::InputText("##Process"e, szProcessName, sizeof(szProcessName));
			ImGui::SameLine();
			if (ImGui::Button("GO"e, { -1, 0.0f }))
				OnButtonInject();
			
			ImGui::Text("IMAGE"e);
			ImGui::SameLine();
			ImGui::SetCursorPosX(TextAlign);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
			ImGui::InputText("##DLL"e, szImageName, sizeof(szImageName));
			ImGui::SameLine();
			if (ImGui::Button("DBVM"e, { -1, 0.0f }))
				OnButtonDBVM();

			if (ImGui::RadioButton("Normal"e, InjectionType == EInjectionType::Normal))
				InjectionType = EInjectionType::Normal;
			ImGui::SameLine();

			CreateRightAlignedItem([&] {
				ImGui::Checkbox("CreateProcess"e, &bCreateProcess);
				});

			if (ImGui::RadioButton("NxBitSwap"e, InjectionType == EInjectionType::NxBitSwap))
				InjectionType = EInjectionType::NxBitSwap;
			if (ImGui::RadioButton("IntoDLL"e, InjectionType == EInjectionType::IntoDLL))
				InjectionType = EInjectionType::IntoDLL;

			ImGui::SameLine();
			ImGui::SetNextItemWidth(-1);
			ImGui::InputText("##IntoDLL"e, szIntoDLL, sizeof(szIntoDLL));

			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextMultiline("##License"e, szParam, sizeof(szParam));

			ImGui::NewLine();

			CreateRightAlignedItem([&] {
				if (ImGui::Button("SetPassword"e))
					OnButtonSetPassword();
				});
		});
		return bInjected;
	}
};
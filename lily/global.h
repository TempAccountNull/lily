#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <psapi.h>
#pragma comment(lib, "d3d9.lib")

#include "initializer.h"
#include "util.h"
#include "encrypt_string.h"
#include "dbvm.h"
#include "msrnames.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

class Global {
public:
	static inline char DBVMPassword[21];
	static inline DBVM dbvm;
	static inline const int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	static inline const int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	static inline IDirect3D9Ex* pDirect3D9Ex;
	static inline IDirect3DDevice9Ex* pDirect3DDevice9Ex;
	static inline IDirect3DSurface9* pBackBufferSurface;
	static inline IDirect3DSurface9* pOffscreenPlainSurface;
	static inline char Buf[0x200];

	static void SetModuleInfo(auto Base, auto Size) {
		ModuleBase = (uintptr_t)Base;
		ModuleSize = (uintptr_t)Size;
	}
private:
	static inline uintptr_t ModuleBase = 0;
	static inline size_t ModuleSize = 0;

	INITIALIZER_INCLASS(UEF) {
		SetUnhandledExceptionFilter([](PEXCEPTION_POINTERS pExceptionInfo)->LONG {
			const uintptr_t ExceptionAddress = (uintptr_t)pExceptionInfo->ExceptionRecord->ExceptionAddress;
			char szAddress[0x40];
			if (ModuleBase && ExceptionAddress >= ModuleBase && ExceptionAddress < ModuleBase + ModuleSize)
				sprintf(szAddress, "Base+0x%I64X"e, ExceptionAddress - ModuleBase);
			else
				sprintf(szAddress, "0x%I64X"e, ExceptionAddress);

			error(szAddress, "Unhandled exception"e);
			return EXCEPTION_CONTINUE_SEARCH;
			});
	};

	INITIALIZER_INCLASS(InitDBVM) {
		DBVMPassword << "qS2n9TLRX8iZ9YmGqgsK"e;

		uint64_t password1 = *(uint64_t*)(Global::DBVMPassword + 0) ^ 0xda2355698be6166c;
		uint32_t password2 = *(uint32_t*)(Global::DBVMPassword + 8) ^ 0x6765fa70;
		uint64_t password3 = *(uint64_t*)(Global::DBVMPassword + 12) ^ 0xe21cb5155c065962;

		dbvm.SetPassword(password1, password2, password3);
		verify(dbvm.GetVersion());
	};

	INITIALIZER_INCLASS(InitD3D) {
		bool bD3DSuccess = [&] {
			HRESULT res = Direct3DCreate9Ex(D3D_SDK_VERSION, &pDirect3D9Ex);
			if (FAILED(res))
				return false;

			D3DPRESENT_PARAMETERS d3dpp = { 0 };
			d3dpp.Windowed = true;
			d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d3dpp.hDeviceWindow = 0;
			d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
			d3dpp.BackBufferCount = 1;
			d3dpp.BackBufferWidth = (UINT)ScreenWidth;
			d3dpp.BackBufferHeight = (UINT)ScreenHeight;
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			d3dpp.EnableAutoDepthStencil = TRUE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

			res = pDirect3D9Ex->CreateDeviceEx(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				nullptr,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dpp,
				nullptr,
				&pDirect3DDevice9Ex
			);
			if (FAILED(res))
				return false;

			res = pDirect3DDevice9Ex->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface);
			if (FAILED(res))
				return false;

			res = pDirect3DDevice9Ex->CreateOffscreenPlainSurface(
				ScreenWidth, ScreenHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pOffscreenPlainSurface, 0);
			if (FAILED(res))
				return false;

			return true;
		}();
		verify(bD3DSuccess);
	};

	INITIALIZER_INCLASS(InitImGui) {
		ImGui::CreateContext();
		ImGui_ImplDX9_Init(pDirect3DDevice9Ex);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = { (float)ScreenWidth , (float)ScreenHeight };

		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf"e, 15.0f);
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf"e, 40.0f);

		io.IniFilename = 0;
	};
};

#ifndef __INTELLISENSE__
template <fixstr::basic_fixed_string Src>
const ElementType(Src)* operator""eg() noexcept {
	return MovString<Src>((ElementType(Src)*)Global::Buf);
}
#else
const char* operator""eg(const char*, size_t);
const wchar_t* operator""eg(const wchar_t*, size_t);
#endif
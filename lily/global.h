#pragma once
#include "common/util.h"

#include <Windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include <dcomp.h>
#pragma comment(lib, "dcomp.lib")
#include <wrl.h>

#include "common/initializer.h"
#include "common/encrypt_string.h"
#include "common/dbvm.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class Global {
public:
	static inline char DBVMPassword[21];
	static inline DBVM dbvm;
	static inline const int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	static inline const int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	template<class Interface>
	using ComPtr = Microsoft::WRL::ComPtr<Interface>;

	static inline ComPtr<ID3D11Device> pD3D11Device;
	static inline ComPtr<ID3D11DeviceContext> pD3D11DeviceContext;

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
		const bool bD3DSuccess = [&] {
			HRESULT hr;
			hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
				0, 0, D3D11_SDK_VERSION, &pD3D11Device, 0, &pD3D11DeviceContext);
			if (FAILED(hr))
				return false;

			return true;
		}();
		verify(bD3DSuccess);
	};

	INITIALIZER_INCLASS(InitImGui) {
		ImGui::CreateContext();
		ImGui_ImplDX11_Init(pD3D11Device.Get(), pD3D11DeviceContext.Get());

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
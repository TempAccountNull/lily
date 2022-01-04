#include <windows.h>
#include "global.h"
#include "injectorUI.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    Global::hModule = GetModuleHandleA(0);

    WNDCLASS WndClass = { };
    WndClass.lpfnWndProc = WndProc;
    WndClass.hInstance = hInstance;
    WndClass.lpszClassName = "Injector"e;
    RegisterClass(&WndClass);

    HWND hWnd = CreateWindowExA(0, "Injector"e, "Injector"e, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0, hInstance, 0);

    if (!hWnd)
        return 0;

    ImGui_ImplWin32_Init(hWnd);
    ShowWindow(hWnd, SW_SHOW);

    InjectorUI UI(hWnd, Global::pDirect3DDevice9Ex);

    while (true) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
              return 0;
        }

        if (UI.ProcessFrame())
            break;
    }

    return 0;
}
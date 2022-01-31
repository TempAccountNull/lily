#pragma once
#include "harderror.h"
#include "common/encrypt_string.h"

#include <windows.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <atlconv.h>
#include <algorithm>
#include <sstream>
#include <utility>

#ifdef _WINDLL
//#define DPRINT
#else
#define DPRINT
#endif

#ifdef DPRINT
#define dprintf(...) [&]{ printf(__VA_ARGS__); putchar('\n'); }()
#else
#define dprintf(...) ((void)0)
#endif

static void MessageBoxCSRSS(const char* szText, const char* szCaption, UINT uType, DWORD dwMilliseconds = INFINITE);
static void MessageBoxCSRSS(const wchar_t* szText, const wchar_t* szCaption, UINT uType, DWORD dwMilliseconds = INFINITE);

static void error(const char* Msg, const char* Title = "Error"e) {
#ifdef DEBUG
	MessageBoxA(0, Msg, Title, MB_ICONERROR | MB_TOPMOST);
#else
	MessageBoxCSRSS(Msg, Title, MB_ICONERROR);
#endif
	exit(0);
}

static void error(const wchar_t* Msg, const wchar_t* Title = L"Error"e) {
#ifdef DEBUG
	MessageBoxW(0, Msg, Title, MB_ICONERROR | MB_TOPMOST);
#else
	MessageBoxCSRSS(Msg, Title, MB_ICONERROR);
#endif
	exit(0);
}

template<fixstr::basic_fixed_string path>
consteval auto ExtractOnlyFileName() {
	return path.substr<path.find_last_of('\\') + 1>();
}

#define STRING(x) #x
#define PACK(x) STRING(x)

#define verify(expression) \
[&]{ \
	if (expression) return; \
	error(EncryptedString<ExtractOnlyFileName<__FILE__ " : " PACK(__LINE__) "\n" #expression>()>(), "Assertion failed!"e); \
}()

static int CreateProcessCMD(const char* szPath) {
	char szCommand[0x100];
	sprintf(szCommand, "if exist \"%s\" start \"\" \"%s\""e, szPath, szPath);
	return system(szCommand);
}

static bool IsKeyPushing(int vKey) { return (GetAsyncKeyState(vKey) & 0x8000) != 0; }
static bool IsKeyPushed(int vKey) { return (GetAsyncKeyState(vKey) & 0x1) != 0; }

static std::wstring s2ws(const std::string& str) {
	USES_CONVERSION;
	return std::wstring(A2W(str.c_str()));
}

static std::string ws2s(const std::wstring& wstr) {
	USES_CONVERSION;
	return std::string(W2A(wstr.c_str()));
}

static std::wstring to_hex_string(uintptr_t i) {
	std::wstringstream s;
	s << (const wchar_t*)L"0x"e << std::hex << i;
	return s.str();
}

static std::wstring trim(const std::wstring& s) {
	auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
	auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) {return std::isspace(c); }).base();
	return (wsback <= wsfront ? std::wstring() : std::wstring(wsfront, wsback));
}

static uint64_t GetTickCountInMicroSeconds() {
	LARGE_INTEGER PerformanceCount, Frequency;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&PerformanceCount);
	return PerformanceCount.QuadPart * 1000000 / Frequency.QuadPart;
}

static void* VirtualAllocVerified(size_t Size, DWORD dwProtect) {
	void* const pResult = [&]() -> void* {
		void* const pMemory = VirtualAlloc(0, Size, MEM_COMMIT | MEM_RESERVE, dwProtect);
		if (!pMemory)
			return 0;

		for (size_t i = 0; i < Size; i += 0x1000) {
			uint8_t byte;
			if (!ReadProcessMemory((HANDLE)-1, (const void*)((uintptr_t)pMemory + i), &byte, 1, 0))
				return 0;
		}

		return pMemory;
	}();

	if (!pResult)
		error("VirtualAlloc"e);

	return pResult;
}

static HMODULE GetKernelModuleAddressVerified(const char* szModule) {
	const HMODULE hKernelModule = [&]()-> HMODULE {
		void* Drivers[0x100];
		DWORD cbNeeded;
		if (!EnumDeviceDrivers(Drivers, sizeof(Drivers), &cbNeeded))
			return 0;

		for (auto Driver : Drivers) {
			char szBaseName[MAX_PATH];
			if (!GetDeviceDriverBaseNameA(Driver, szBaseName, sizeof(szBaseName)))
				continue;
			if (_stricmp(szBaseName, szModule) == 0)
				return (HMODULE)Driver;
		}

		return 0;
	}();

	if (!hKernelModule)
		error(szModule);

	return hKernelModule;
}

static uintptr_t GetKernelProcAddress(const char* szModuleName, const char* szProcName) {
	const HMODULE hModule = LoadLibraryA(szModuleName);
	if (!hModule)
		return 0;

	const HMODULE hModuleKernel = GetKernelModuleAddressVerified(szModuleName);
	if (!hModuleKernel)
		return 0;

	const FARPROC ProcAddr = GetProcAddress(hModule, szProcName);
	FreeLibrary(hModule);

	if (!ProcAddr)
		return 0;

	return (uintptr_t)ProcAddr + (uintptr_t)hModuleKernel - (uintptr_t)hModule;
}

static uintptr_t GetKernelProcAddressVerified(const char* szModuleName, const char* szProcName) {
	const uintptr_t Result = GetKernelProcAddress(szModuleName, szProcName);
	if (!Result)
		error(szProcName, szModuleName);

	return Result;
}

static uintptr_t GetUserProcAddress(const char* szModuleName, const char* szProcName) {
	const HMODULE hModule = GetModuleHandleA(szModuleName);
	if (!hModule)
		return 0;

	const FARPROC ProcAddr = GetProcAddress(hModule, szProcName);
	FreeLibrary(hModule);

	if (!ProcAddr)
		return 0;

	return (uintptr_t)ProcAddr;
}

static uintptr_t GetUserProcAddressVerified(const char* szModuleName, const char* szProcName) {
	const uintptr_t Result = GetUserProcAddress(szModuleName, szProcName);

	if (!Result)
		error(szProcName, szModuleName);

	return Result;
}

static void MessageBoxCSRSS(const wchar_t* Text, const wchar_t* Caption, UINT uType, DWORD dwMilliseconds) {
	static tNtRaiseHardError NtRaiseHardError = 0;
	if (!NtRaiseHardError) {
		NtRaiseHardError = (tNtRaiseHardError)GetUserProcAddress("ntdll.dll"e, "NtRaiseHardError"e);
		if (!NtRaiseHardError)
			exit(0);
	}

	const USHORT LenText = (USHORT)(wcslen(Text) * sizeof(wchar_t));
	const USHORT LenCaption = (USHORT)(wcslen(Caption) * sizeof(wchar_t));
	const UNICODE_STRING uText = { LenText, LenText, (PWSTR)Text };
	const UNICODE_STRING uCaption = { LenCaption, LenCaption, (PWSTR)Caption };
	HARDERROR_RESPONSE Response;

	uintptr_t Params[] = { (uintptr_t)&uText, (uintptr_t)&uCaption, (uintptr_t)uType, (uintptr_t)dwMilliseconds };
	constexpr auto NumOfParams = sizeof(Params) / sizeof(*Params);

	NtRaiseHardError(STATUS_SERVICE_NOTIFICATION_2, NumOfParams, (1 << 0) | (1 << 1), Params, OptionOkNoWait, &Response);
}

static void MessageBoxCSRSS(const char* Text, const char* Caption, UINT uType, DWORD dwMilliseconds) {
	USES_CONVERSION;
	MessageBoxCSRSS(A2W(Text), A2W(Caption), uType, dwMilliseconds);
}

static DWORD GetPIDFromHWND(HWND hWnd) {
	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);
	return dwPid;
}

static DWORD GetPIDByProcessName(const char* szProcessName) {
	const HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD Pid = 0;

	PROCESSENTRY32 pe32 = { .dwSize = sizeof(pe32) };
	if (Process32First(hSnapShot, &pe32)) {
		do {
			if (_stricmp(pe32.szExeFile, szProcessName) == 0) {
				Pid = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapShot, &pe32));
	}

	CloseHandle(hSnapShot);
	return Pid;
}

static void SetClientRect(HWND hWnd, RECT Rect) {
	const DWORD dwStyle = GetWindowLongA(hWnd, GWL_STYLE);
	const DWORD dwExStyle = GetWindowLongA(hWnd, GWL_EXSTYLE);

	AdjustWindowRectEx(&Rect, dwStyle, GetMenu(hWnd) != 0, dwExStyle);
	if (dwStyle & WS_VSCROLL) Rect.right += GetSystemMetrics(SM_CXVSCROLL);
	if (dwStyle & WS_HSCROLL) Rect.bottom += GetSystemMetrics(SM_CYVSCROLL);
	SetWindowPos(hWnd, 0, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

static bool SetClientRectWrapper(HWND hWnd, RECT Rect, auto f) {
	RECT ClientRect;
	if (!GetClientRect(hWnd, &ClientRect))
		return false;

	SetClientRect(hWnd, Rect);
	f();
	SetClientRect(hWnd, ClientRect);
	return true;
}

static bool SetPrivilege(HANDLE hToken, const char* szPrivilege, bool bEnablePrivilege) {
	LUID Luid;
	if (!LookupPrivilegeValueA(NULL, szPrivilege, &Luid))
		return false;

	TOKEN_PRIVILEGES TokenPrivileges = {
		.PrivilegeCount = 1,
		.Privileges = { {.Luid = Luid, .Attributes = bEnablePrivilege ? (DWORD)SE_PRIVILEGE_ENABLED : 0 } }
	};

	return AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), 0, 0) &&
		GetLastError() == ERROR_SUCCESS;
};

template <typename... T>
constexpr auto make_array(T&&... t) ->
std::array<std::decay_t<std::common_type_t<T...>>, sizeof...(T)> {
	return { {std::forward<T>(t)...} };
}

#define HASH_CASE(Hash, ...)\
case Hash : return __VA_ARGS__

#define HASH_DEFAULT(...)\
default : return __VA_ARGS__

#define MAKE_GETTER(Value, Name) const decltype(Value)& Name = Value

void SetThreadAffinityMaskWrapper(auto f) {
	const HANDLE hCurrentThread = GetCurrentThread();
	DWORD_PTR OldAffinityMask = SetThreadAffinityMask(hCurrentThread, 1);
	f();
	SetThreadAffinityMask(hCurrentThread, OldAffinityMask);
}
#pragma once

#include <windows.h>
#include <stdio.h>
#include "encrypt_string.hpp"

#ifdef _WINDLL
//#define DPRINT
#else
#define DPRINT
#endif

#ifdef DPRINT
#define dprintf(...) [&]{ printf(__VA_ARGS__); putchar('\n'); }()
#else
#define dprintf(...) []{}()
#endif

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ " : " S2(__LINE__)

static void error(const char* szMsg, const char* szTitle = "Error!"e) {
	while (MessageBoxA(0, szMsg, szTitle, MB_ICONERROR | MB_TOPMOST | MB_YESNO) == IDYES);
	TerminateProcess((HANDLE)-1, -1);
}

template<fixstr::basic_fixed_string path>
consteval auto ExtractOnlyFileName() {
	return path.substr<path.find_last_of('\\') + 1>();
}

#define verify(expression) \
[&]{ \
	if (expression) return; \
	error(EncryptedString<ExtractOnlyFileName<LOCATION "\n" #expression>()>(), "Assertion failed!"e); \
}()

static int CreateProcessCMD(const char* szPath) {
	char szCommand[0x100];
	sprintf(szCommand, "if exist \"%s\" start \"\" \"%s\""e, szPath, szPath);
	return system(szCommand);
}

static bool IsKeyPushing(int vKey) { return (GetAsyncKeyState(vKey) & 0x8000) != 0; }
static bool IsKeyPushed(int vKey) { return (GetAsyncKeyState(vKey) & 0x1) != 0; }
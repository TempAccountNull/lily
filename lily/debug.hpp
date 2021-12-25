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
#define dprintf(...)\
do {\
	printf(__VA_ARGS__);\
	putchar('\n');\
} while(0)
#else
#define dprintf(...) do {} while(0)
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

#define verify(expression)\
while(!(expression)) {\
	error(EncryptedString<ExtractOnlyFileName<LOCATION "\n" #expression>()>(), "Assertion failed!"e);\
}
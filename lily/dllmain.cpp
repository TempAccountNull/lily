#include <Windows.h>
#include "global.h"

void realmain();

bool ScrambleMemory(HMODULE hDLL) {
	srand((unsigned int)GetTickCount64());

	PBYTE ptr = (PBYTE)hDLL;
	DWORD i;

	for (i = 0; i < 0x1000; i++)
		ptr[i] = (BYTE)rand();

	while (1) {
		if (ptr[i] == 0xe9)			i += 5;
		else if (ptr[i] == 0xcc)		ptr[i++] = (BYTE)rand();
		else if (ptr[i] == 0x66)		ptr[i++] = (BYTE)rand();
		else if (ptr[i] == 0x90)		ptr[i++] = (BYTE)rand();
		else						break;
	}

	while (1) {
		if (ptr[i] == 0)				ptr[i++] = (BYTE)rand();
		else						break;
	}

	if (i % 0x1000 == 0) return true;
	return false;
}

bool ErasePE(HMODULE hDLL) {
	srand(GetCurrentThreadId());
	return ExceptionHandler::TryExcept([&] {
		for (unsigned i = 0; i < 0x1000; i++)
			((PBYTE)hDLL)[i] = (BYTE)rand();
		});
}

BOOL WINAPI DllMain(HMODULE hDLL, DWORD dwReason, char* szParam) {
	if (dwReason != DLL_PROCESS_ATTACH)
		return FALSE;

	if (szParam)
		VirtualFree(szParam, 0, MEM_RELEASE);

	bool IsInjectorValid = [&] {
		if (!ErasePE(hDLL))
			return false;
		MEMORY_BASIC_INFORMATION MemInfo;
		if (!VirtualQuery(DllMain, &MemInfo, sizeof(MemInfo)))
			return false;
		if (MemInfo.Protect != PAGE_READWRITE)
			return false;
		const uintptr_t Base = (uintptr_t)MemInfo.AllocationBase & ~0xFFF;
		const uintptr_t Size = (uintptr_t)MemInfo.BaseAddress - (uintptr_t)MemInfo.AllocationBase + MemInfo.RegionSize;
		verify(Base == (uintptr_t)hDLL);
		Global::SetModuleInfo(Base, Size);

		return true;
	}();

	verify(IsInjectorValid);
	realmain();
}
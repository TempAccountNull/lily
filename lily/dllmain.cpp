﻿#include "init.h"
#include <TlHelp32.h>

DWORD CALLBACK MainThread(PVOID pParam) {
	return TerminateProcess((HANDLE)-1, Init());
}

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

void ErasePE(HMODULE hDLL) {
	srand(GetTickCount());

	DWORD dwOld;
	VirtualProtectEx(GetCurrentProcess(), hDLL, 0x1000, PAGE_READWRITE, &dwOld);
	for (int i = 0; i < 0x1000; i++)
		(PBYTE(hDLL))[i] = BYTE(rand() % 0xFF);
	VirtualProtectEx(GetCurrentProcess(), hDLL, 0x1000, dwOld, &dwOld);
}

void BypassedCreateThread2(LPVOID lpStart, LPVOID pParam) {
	auto dosHeader = (PIMAGE_DOS_HEADER)GetModuleHandleA(0);
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return;

	auto peHeader = (PIMAGE_NT_HEADERS)((DWORD64)dosHeader + dosHeader->e_lfanew);
	if (peHeader->Signature != IMAGE_NT_SIGNATURE)
		return;

	LPVOID EntryPoint = LPVOID(peHeader->OptionalHeader.ImageBase + peHeader->OptionalHeader.AddressOfEntryPoint);

	BYTE HookCode[] = {
		0x48, 0x8B, 0x01,			//mov rax,[rcx]
		0x48, 0x8B, 0x49, 0x08,		//mov rcx,[rcx+08]
		0xFF, 0xE0,					//jmp rax
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	BYTE OriginalCode[sizeof(HookCode)];

	DWORD dwOldProtect;
	//VirtualProtect is blocked
	VirtualProtectEx(GetCurrentProcess(), (PVOID)EntryPoint, sizeof(HookCode), PAGE_EXECUTE_READWRITE, &dwOldProtect);

	memcpy(OriginalCode, EntryPoint, sizeof(HookCode));

	*(LPVOID*)(HookCode + 9) = lpStart;
	*(LPVOID*)(HookCode + 17) = pParam;
	memcpy(EntryPoint, HookCode, sizeof(HookCode));

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)EntryPoint, LPVOID((DWORD64)EntryPoint + 9), 0, 0);
	Sleep(1);

	memcpy(EntryPoint, OriginalCode, sizeof(HookCode));

	//VirtualProtect is blocked
	VirtualProtectEx(GetCurrentProcess(), (PVOID)EntryPoint, sizeof(HookCode), dwOldProtect, &dwOldProtect);
}

void BypassedCreateThread(LPVOID lpStart, LPVOID pParam) {
	PBYTE pStub = (PBYTE)VirtualAlloc(0, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	BYTE Code[] = {
		0x48, 0x8B, 0x01,			//mov rax,[rcx]
		0x48, 0x8B, 0x49, 0x08,		//mov rcx,[rcx+08]
		0xFF, 0xE0					//jmp rax
	};
	memcpy(pStub, Code, sizeof(Code));
	*(LPVOID*)(pStub + 0x100) = lpStart;
	*(LPVOID*)(pStub + 0x108) = pParam;

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)pStub, pStub + 0x100, 0, 0);
	Sleep(1);
	VirtualFree(pStub, 0, MEM_RELEASE);
}

BOOL WINAPI DllMain(HMODULE hDLL, DWORD dwReason, const char* szParam) {
	/*
	if (strlen(szParam) < 20)
		TerminateProcess(GetCurrentProcess(), 0);
	strcpy(g_Password, szParam);
	*/

	ErasePE(hDLL);
	BypassedCreateThread2(MainThread, 0);
	//CreateThread(0, 0x100000, MainThread, 0, 0, 0);
	return TRUE;
}
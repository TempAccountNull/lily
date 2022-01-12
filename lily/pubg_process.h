#pragma once
#include <Windows.h>
#include "process.h"
#include "exception.h"
#include "dump.h"

extern "C" uintptr_t XenuineCall(uintptr_t Seed, uintptr_t Value, uintptr_t rax, uintptr_t pFunc);

class PubgProcess;
inline PubgProcess* g_Pubg;

class PubgProcess : public Process {
private:
	static constexpr uintptr_t XenuineEncryptFuncAddress = 0x724FB20;
	static constexpr uintptr_t XenuineDecryptFuncAddress = XenuineEncryptFuncAddress + 0x8;

	HWND hGameWnd = 0;
	uintptr_t xenuine_start_rax_encrypt = 0;
	uintptr_t xenuine_start_rax_decrypt = 0;
	uintptr_t pXenuineEncryptMapped = 0;
	uintptr_t pXenuineDecryptMapped = 0;

	bool InitEncrypt() {
		uintptr_t P;
		if (!ReadBase(XenuineEncryptFuncAddress, &P))
			return false;

		uint16_t Code;
		if (!Read(P, &Code) && Code != 0x8D48)
			return false;

		unsigned Offset;
		if (!Read(P + 0x3, &Offset))
			return false;

		xenuine_start_rax_encrypt = Offset + P + 0x7;
		pXenuineEncryptMapped = kernel.GetMappedAddress(P + 0x7);
		return true;
	}
	bool InitDecrypt() {
		uintptr_t P;
		if (!ReadBase(XenuineDecryptFuncAddress, &P))
			return false;

		uint16_t Code;
		if (!Read(P, &Code) && Code != 0x8D48)
			return false;

		unsigned Offset;
		if (!Read(P + 0x3, &Offset))
			return false;

		xenuine_start_rax_decrypt = Offset + P + 0x7;
		pXenuineDecryptMapped = kernel.GetMappedAddress(P + 0x7);
		return true;
	}

public:
	PubgProcess(Kernel& kernel, HWND hWnd) : Process(kernel, GetPIDFromHWND(hWnd)), hGameWnd(hWnd) {
		//DumpAll(process);
		//Dump(*this);
		//exit(0);

		g_Pubg = this;
		verify(InitEncrypt());
		verify(InitDecrypt());
	}

	HWND GetHwnd() const { return hGameWnd; }

	uintptr_t Encrypt(uintptr_t Value, uintptr_t Seed = 0) const {
		uintptr_t Result = 0;
		for (unsigned i = 0; i < 10 && !
			ExceptionHandler::TryExcept([&]() { Result = XenuineCall(Seed, Value, xenuine_start_rax_encrypt, pXenuineEncryptMapped); });
			i++, kernel.SetCustomCR3()) {
		}
		return Result;
	}

	uintptr_t Decrypt(uintptr_t Value, uintptr_t Seed = 0) const {
		uintptr_t Result = 0;
		for (unsigned i = 0; i < 10 && !
			ExceptionHandler::TryExcept([&]() { Result = XenuineCall(Seed, Value, xenuine_start_rax_encrypt, pXenuineDecryptMapped); });
			i++, kernel.SetCustomCR3()) {
		}
		return Result;
	}
};

class XenuinePtr {
private:
	const uintptr_t P;
public:
	XenuinePtr(uintptr_t P) : P(g_Pubg->Encrypt(P)) {}
	operator uintptr_t() const { return g_Pubg->Decrypt(P); }
};
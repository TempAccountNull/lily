#pragma once

#include <Windows.h>
#include "process.h"
#include "exception.h"

extern "C" uintptr_t XenuineCall(uintptr_t Seed, uintptr_t Value, uintptr_t rax, uintptr_t pFunc);

class Xenuine;
inline Xenuine* gXenuine;

class Xenuine {
private:
	static constexpr uintptr_t XenuineEncryptFuncAddress = 0x7246720;
	static constexpr uintptr_t XenuineDecryptFuncAddress = XenuineEncryptFuncAddress + 0x8;
	uintptr_t xenuine_start_rax_encrypt;
	uintptr_t xenuine_start_rax_decrypt;
	uintptr_t pXenuineEncryptMapped;
	uintptr_t pXenuineDecryptMapped;

	bool InitEncrypt() {
		uintptr_t P;
		if (!process.GetBaseValue(XenuineEncryptFuncAddress, &P))
			return false;

		uint16_t Code;
		if (!process.GetValue(P, &Code) && Code != 0x8D48)
			return false;

		unsigned Offset;
		if (!process.GetValue(P + 0x3, &Offset))
			return false;

		xenuine_start_rax_encrypt = Offset + P + 0x7;
		pXenuineEncryptMapped = process.kernel.GetMappedAddress(P + 0x7);
		return true;
	}

	bool InitDecrypt() {
		uintptr_t P;
		if (!process.GetBaseValue(XenuineDecryptFuncAddress, &P))
			return false;

		uint16_t Code;
		if (!process.GetValue(P, &Code) && Code != 0x8D48)
			return false;

		unsigned Offset;
		if (!process.GetValue(P + 0x3, &Offset))
			return false;

		xenuine_start_rax_decrypt = Offset + P + 0x7;
		pXenuineDecryptMapped = process.kernel.GetMappedAddress(P + 0x7);
		return true;
	}

public:
	Process& process;

	Xenuine(Process& process) : process(process) {
		gXenuine = this;
		verify(InitEncrypt());
		verify(InitDecrypt());
	}

	uintptr_t Encrypt(uintptr_t Value, uintptr_t Seed = 0) const {
		uintptr_t Result = 0;

		while (!ExceptionHandler::TryExcept([&]() {
			Result = XenuineCall(Seed, Value, xenuine_start_rax_encrypt, pXenuineEncryptMapped);
			})) {
			process.kernel.SetCustomCR3();
		}

		return Result;
	}

	uintptr_t Decrypt(uintptr_t Value, uintptr_t Seed = 0) const {
		uintptr_t Result = 0;

		while (!ExceptionHandler::TryExcept([&]() {
			Result = XenuineCall(Seed, Value, xenuine_start_rax_decrypt, pXenuineDecryptMapped);
			})) {
			process.kernel.SetCustomCR3();
		}

		return Result;
	}
};

class XenuinePtr {
private:
	uintptr_t P;
public:
	XenuinePtr(uintptr_t P) : P(gXenuine->Encrypt(P)) {}
	operator uintptr_t() const { return gXenuine->Decrypt(P); }
};
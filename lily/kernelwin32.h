#pragma once
#include "kernel.h"

using HDEV = HDC;

class KernelWin32 : public Kernel {
private:
	using tUserGetHDEV = HDEV(*)();
	tUserGetHDEV UserGetHDEV;

public:
	KernelWin32(const DBVM& dbvm) : Kernel(dbvm) {
		UserGetHDEV = (tUserGetHDEV)GetKernelProcAddress("win32kbase.sys"e, "UserGetHDEV"e);
		verify(UserGetHDEV);
	}

	HDEV GetUserHDEV() const {
		HDEV Result = 0;
		KernelExecute([&] { Result = KernelCall(UserGetHDEV); });
		return Result;
	}
};
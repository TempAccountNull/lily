#pragma once

#include <Windows.h>
#include <string>

#include "pubg_process.h"
#include "pubg_struct.h"
#include "common/compiletime.h"
#include "pubg_class.h"

class TNameEntryArray
{
private:
	static constexpr uintptr_t ADDRESS_GNAMES = 0x8D19538;
	static constexpr UINT32 ElementsPerChunk = 0x3FD8;

	//NumElements can be increased when name added to list
	//Don't use this for check index range -> "if(ID.ComparisonIndex >= NumElements)"
	//DWORD64 NumElements;
	uintptr_t BasePtr;

	static constexpr size_t NAME_SIZE = 0x200;
	struct FNameEntry
	{
	public:
		char pad[0x10];
		union
		{
			char	AnsiName[NAME_SIZE];
			wchar_t	WideName[NAME_SIZE];
		};
	};
public:
	TNameEntryArray() {
		EncryptedPtr<uintptr_t> P;
		g_Pubg->ReadBase(ADDRESS_GNAMES, &P);
		g_Pubg->Read(P, &P);
		g_Pubg->Read(P, &P);
		//////////////////////////////////////////////////////////////////
		EncryptedPtr<uintptr_t> P2;
		g_Pubg->Read(P + 0, &P2); BasePtr = P2;
		//gXenuine->process.GetValue(P + 8, &P2); NumElements = P2;
	}

	//DWORD64 GetNumElements() const { return NumElements; }

	bool GetNameByID(FName ID, char* szBuf, size_t SizeMax) const {
		if (ID.ComparisonIndex <= 0)
			return false;

		uintptr_t Ptr = BasePtr + sizeof(UINT_PTR) * (ID.ComparisonIndex / ElementsPerChunk);
		g_Pubg->Read(Ptr, &Ptr);
		if (!Ptr)
			return false;

		Ptr = Ptr + sizeof(UINT_PTR) * (ID.ComparisonIndex % ElementsPerChunk);
		g_Pubg->Read(Ptr, &Ptr);
		if (!Ptr)
			return false;

		if (!g_Pubg->ReadProcessMemory(Ptr + offsetof(FNameEntry, AnsiName), szBuf, std::clamp(SizeMax, (size_t)0, NAME_SIZE)))
			return false;

		return true;
	}

	unsigned GetNameHashByID(FName ID) const {
		char szBuf[NAME_SIZE];
		if (!GetNameByID(ID, szBuf, sizeof(szBuf)))
			return 0;
		return CompileTime::StrHash(szBuf);
	}

	unsigned GetNameHashByObject(NativePtr<UObject> Ptr) const {
		UObject Obj;
		if (!Ptr.Read(Obj))
			return 0;
		return GetNameHashByID(Obj.GetFName());
	}

	void DumpAllNames() const {
		FILE* out = fopen("out.txt"e, "w"e);

		char szBuf[NAME_SIZE];

		for (int i = 1; i < 0x100000; i++) {
			if (!GetNameByID({ i++, 0 }, szBuf, sizeof(szBuf)))
				continue;
			fprintf(out, "%06X %s\n"e, i, szBuf);
		}
		fclose(out);
		verify(0);
	}
};
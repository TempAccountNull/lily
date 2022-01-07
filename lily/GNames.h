#pragma once

#include <Windows.h>
#include <string>

#include "xenuine.h"
#include "pubg_struct.h"
#include "compiletime.h"
#include "pubg_class.h"

class TNameEntryArray
{
private:
	static constexpr DWORD64 ADDRESS_GNAMES = 0x8D402C8;
	static constexpr UINT32 ElementsPerChunk = 0x4134;

	//NumElements can be increased when name added to list
	//Don't use this for check index range -> "if(ID.ComparisonIndex >= NumElements)"
	//DWORD64 NumElements;
	DWORD64 BasePtr;

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
		EncryptedObjectPtr<DWORD64> P;
		gXenuine->process.GetBaseValue(ADDRESS_GNAMES, &P);
		gXenuine->process.GetValue(P, &P);
		gXenuine->process.GetValue(P, &P);
		//////////////////////////////////////////////////////////////////
		EncryptedObjectPtr<DWORD64> P2;
		gXenuine->process.GetValue(P + 0, &P2); BasePtr = P2;
		//gXenuine->process.GetValue(P + 8, &P2); NumElements = P2;
	}

	//DWORD64 GetNumElements() const { return NumElements; }

	bool GetNameByID(FName ID, char* szBuf, size_t SizeMax) const {
		if (ID.ComparisonIndex <= 0)
			return false;

		DWORD64 Ptr = BasePtr + sizeof(UINT_PTR) * (ID.ComparisonIndex / ElementsPerChunk);
		gXenuine->process.GetValue(Ptr, &Ptr);
		if (!Ptr)
			return false;

		Ptr = Ptr + sizeof(UINT_PTR) * (ID.ComparisonIndex % ElementsPerChunk);
		gXenuine->process.GetValue(Ptr, &Ptr);
		if (!Ptr)
			return false;

		if (!gXenuine->process.GetValueWithSize(Ptr + offsetof(FNameEntry, AnsiName), szBuf, std::min(SizeMax, NAME_SIZE)))
			return false;

		return true;
	}

	unsigned GetNameHashByID(FName ID) const {
		char szBuf[NAME_SIZE];
		if (!GetNameByID(ID, szBuf, sizeof(szBuf)))
			return 0;
		return CompileTime::Hash(szBuf);
	}

	unsigned GetNameHashByObject(ObjectPtr<UObject> Ptr) const {
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
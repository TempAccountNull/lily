#pragma once

#include <Windows.h>

#include "ida_defs.h"
#include "xenuine.h"
#include "pubg_struct.h"

class TNameEntryArray;

class UObject
{
private:
	BYTE pad[0x40];
public:
	//DWORD64 GetClass() {
	//	__int64 a1 = (__int64)this;
	//	__int64 v2; // rax

	//	v2 = __ROL8__(*(_QWORD*)(a1 + 0x18) ^ 0x7753CBD5413FEAD7i64, 0x17);
	//	return v2 ^ (v2 << 0x20) ^ 0xA7950E40FAB9478i64;
	//}

	FName GetFName() {
		__int64 a1 = (__int64)this;
		int v1;
		int v2;
		unsigned int v3;
		int v4;
		__int64 v37;
		char result;

		v1 = *(_DWORD*)(a1 + 0x38) ^ 0xDAED6428;
		v2 = __ROL4__(*(_DWORD*)(a1 + 0x34) ^ 0x54AF9111, 4);
		v3 = v2 ^ (v2 << 0x10) ^ 0xBEC56428;
		LODWORD(v37) = v3;
		v4 = __ROR4__(v1, 4);
		result = v4 ^ 0x11;
		HIDWORD(v37) = v4 ^ (v4 << 0x10) ^ 0xC5BE9111;

		FName name;
		name.ComparisonIndex = LODWORD(v37);
		name.Number = HIDWORD(v37);

		return name;
	}

	//DWORD GetIndex() {
	//	__int64 a2 = (__int64)this;
	//	int v2;

	//	v2 = __ROL4__(*(_DWORD*)(a2 + 8) ^ 0x45D2C49B, 9);
	//	return v2 ^ (v2 << 0x10) ^ 0x5C80CBD3;
	//}

	//DWORD GetObjectFlags() {
	//	unsigned __int64 v2 = (unsigned __int64)this;
	//	int v3;

	//	v3 = __ROR4__(*(_DWORD*)(v2 + 0x30) ^ 0xD1911304, 8);
	//	return (v3 ^ (v3 << 0x10) ^ 0x354055CB);
	//}

	//DWORD64 GetOuter() {
	//	unsigned __int64 v2 = (unsigned __int64)this;
	//	__int64 v4;

	//	v4 = __ROR8__(*(_QWORD*)(v2 + 0x28) ^ 0x1555DE0E8CFD7EE3i64, 3);
	//	v2 = v4 ^ (v4 << 0x20) ^ 0x3811FD43E3F5A7C4i64;
	//	return v2;
	//}
};

class FUObjectItem
{
public:
	char Padding[0x8];
	DWORD64 ObjectPtr;
	char Padding2[0x20];
};

class FUObjectArray
{
private:
	static constexpr DWORD64 ADDRESS_GOBJECTS = 0x8D45178;

	DWORD64 BaseAddress;
	DWORD NumElements;

public:
	FUObjectArray() {
		EncryptedObjectPtr<DWORD64> P;
		gXenuine->process.GetBaseValue(ADDRESS_GOBJECTS + 0x18, &P);
		BaseAddress = P;
		gXenuine->process.GetBaseValue(ADDRESS_GOBJECTS + 0x28, &NumElements);
	}

	ObjectPtr<UObject> GetObjectPtrById(DWORD Index) const {
		if (Index >= NumElements)
			return 0;

		FUObjectItem ObjectItem;
		if (gXenuine->process.GetValue(BaseAddress + Index * sizeof(ObjectItem), &ObjectItem) == 0)
			return 0;

		return ObjectItem.ObjectPtr;
	}

	DWORD GetNumElements() const { return NumElements; }
	void DumpObject(const TNameEntryArray& NameArr) const;
};
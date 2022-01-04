#pragma once
#include <string>
#include <vector>
#include "injectorUI.h"
#include "remoteprocess.h"

#define IMR_RELTYPE(x)				((x >> 12) & 0xF)
#define IMR_RELOFFSET(x)			(x & 0xFFF)

class Injector {
private:
	RemoteProcess process;
	mutable std::string strMsg;
	std::vector<uint8_t> Binary;

	template<class Type>
	void GetBinaryData(uintptr_t Offset, size_t Index, Type& Data) const {
		std::copy_n(Binary.begin() + Offset + sizeof(Type) * Index, sizeof(Type), (uint8_t*)&Data);
	}

	template<class Type>
	void SetBinaryData(uintptr_t Offset, size_t Index, const Type& Data) {
		std::copy_n((const uint8_t*)&Data, sizeof(Type), Binary.begin() + Offset + sizeof(Type) * Index);
	}

	HMODULE RemoteGetModuleHandleA(const char* szModuleName) const {
		HMODULE Result = 0;

		if (szModuleName)
			process.VirtualAllocWrapper(szModuleName, strlen(szModuleName) + 1, [&](const void* pszModuleName) {
			process.RemoteCall(GetModuleHandleA, Result, pszModuleName, 0, 0, 0); });
		else
			process.RemoteCall(GetModuleHandleA, Result, 0, 0, 0, 0);

		return Result;
	}

	HMODULE RemoteLoadLibraryExA(const char* szFileName, DWORD dwFlags = 0) const {
		HMODULE Result = 0;

		if (szFileName)
			process.VirtualAllocWrapper(szFileName, strlen(szFileName) + 1, [&](const void* pszFileName) {
			process.RemoteCall(LoadLibraryExA, Result, pszFileName, 0, dwFlags, 0); });
		else
			process.RemoteCall(LoadLibraryExA, Result, 0, 0, dwFlags, 0);

		return Result;
	}

	FARPROC RemoteGetProcAddress(HMODULE hModule, const char* szFuncName) const {
		FARPROC Result = 0;

		if (HIWORD(szFuncName))
			process.VirtualAllocWrapper(szFuncName, strlen(szFuncName) + 1, [&](const void* pszFuncName) {
			process.RemoteCall(GetProcAddress, Result, hModule, pszFuncName, 0, 0); });
		else
			process.RemoteCall(GetProcAddress, Result, hModule, szFuncName, 0, 0);

		return Result;
	}

	size_t GetOffsetOptionalHeader(size_t OffsetNtHeader, const IMAGE_NT_HEADERS& ntHd) const {
		return OffsetNtHeader + offsetof(IMAGE_NT_HEADERS, OptionalHeader) + ntHd.FileHeader.SizeOfOptionalHeader;
	}

	bool GetSectionHeaderInRVA(size_t OffsetNtHeader, const IMAGE_NT_HEADERS& ntHd, uintptr_t RVA, IMAGE_SECTION_HEADER& OutSectionHeader) const {
		size_t OffsetOptionalHeader = GetOffsetOptionalHeader(OffsetNtHeader, ntHd);
		if (!OffsetOptionalHeader)
			return false;

		for (unsigned i = 0; i < ntHd.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER SectionHeader;
			GetBinaryData(OffsetOptionalHeader, i, SectionHeader);

			size_t SectionSize = SectionHeader.Misc.VirtualSize;
			if (!SectionSize)
				SectionSize = SectionHeader.SizeOfRawData;

			if (RVA >= SectionHeader.VirtualAddress && RVA < (SectionHeader.VirtualAddress + SectionSize)) {
				OutSectionHeader = SectionHeader;
				return true;
			}
		}
		return false;
	}

	uintptr_t GetOffsetFromRVA(size_t OffsetNtHeader, const IMAGE_NT_HEADERS& ntHd, uintptr_t RVA) const {
		IMAGE_SECTION_HEADER SectionHeader;
		if (!GetSectionHeaderInRVA(OffsetNtHeader, ntHd, RVA, SectionHeader))
			return 0;

		return RVA - SectionHeader.VirtualAddress + SectionHeader.PointerToRawData;
	}

	bool FixImports(size_t OffsetNtHeader, const IMAGE_NT_HEADERS& ntHd, size_t OffsetImportDesc, const char* szDLLDir) {
		char szSystemDirectory[MAX_PATH];
		if (!GetSystemDirectoryA(szSystemDirectory, sizeof(szSystemDirectory))) {
			strMsg << "GetSystemDirectoryA Failed"e;
			return false;
		}

		for (unsigned ImportDescIndex = 0;; ImportDescIndex++) {
			IMAGE_IMPORT_DESCRIPTOR ImportDesc;
			GetBinaryData(OffsetImportDesc, ImportDescIndex, ImportDesc);

			uintptr_t NameOffset = GetOffsetFromRVA(OffsetNtHeader, ntHd, ImportDesc.Name);
			if (!NameOffset)
				break;

			const std::string strDLLName = (const char*)&Binary[NameOffset];
			if (!strDLLName.size())
				break;

			HMODULE hRemoteDLL = RemoteLoadLibraryExA(strDLLName.c_str());
			if (!hRemoteDLL) {
				//System32 folder
				std::string strSystemPathDLL = szSystemDirectory;
				strSystemPathDLL += "\\"e;
				strSystemPathDLL += strDLLName;
				hRemoteDLL = RemoteLoadLibraryExA(strSystemPathDLL.c_str());
			}
			if (!hRemoteDLL) {
				//Injector folder
				std::string strPathDLL = szDLLDir;
				strPathDLL += "\\"e;
				strPathDLL += strDLLName;
				hRemoteDLL = RemoteLoadLibraryExA(strPathDLL.c_str());
			}
			if (!hRemoteDLL) {
				strMsg << "RemoteLoadLibrary failed : "e;
				strMsg += strDLLName;
				return false;
			}

			//fix the time/date stamp
			//ImportDesc.TimeDateStamp = ntHd.FileHeader.TimeDateStamp;

			size_t OffsetImageThunkData = GetOffsetFromRVA(OffsetNtHeader, ntHd, ImportDesc.FirstThunk);

			for (unsigned ImageThunkDataIndex = 0;; ImageThunkDataIndex++) {
				IMAGE_THUNK_DATA ImageThunkData;
				GetBinaryData(OffsetImageThunkData, ImageThunkDataIndex, ImageThunkData);

				if (!ImageThunkData.u1.Ordinal)
					break;

				if (IMAGE_SNAP_BY_ORDINAL(ImageThunkData.u1.Ordinal)) {
					uint16_t Ordinal = IMAGE_ORDINAL(ImageThunkData.u1.Ordinal);

					FARPROC pRemoteFuncAddress = RemoteGetProcAddress(hRemoteDLL, (const char*)Ordinal);
					if (!pRemoteFuncAddress) {
						strMsg << "RemoteGetProcAddress failed : "e;
						strMsg += strDLLName;
						strMsg += " ordinal "e;
						strMsg += std::to_string(Ordinal);
						return false;
					}
					ImageThunkData.u1.Function = (uintptr_t)pRemoteFuncAddress;
					SetBinaryData(OffsetImageThunkData, ImageThunkDataIndex, ImageThunkData);
				}
				else {
					size_t OffsetImageImportByName = GetOffsetFromRVA(OffsetNtHeader, ntHd, ImageThunkData.u1.AddressOfData);
					OffsetImageImportByName += offsetof(IMAGE_IMPORT_BY_NAME, Name);

					std::string strFuncName;
					for (unsigned i = 0; Binary[OffsetImageImportByName + i]; i++)
						strFuncName.append(1, (char)Binary[OffsetImageImportByName + i]);

					FARPROC pRemoteFuncAddress = RemoteGetProcAddress(hRemoteDLL, strFuncName.c_str());
					if (!pRemoteFuncAddress) {
						strMsg << "RemoteGetProcAddress failed : "e;
						strMsg += strDLLName;
						strMsg += " "e;
						strMsg += strFuncName.c_str();
						return false;
					}
					ImageThunkData.u1.Function = (uintptr_t)pRemoteFuncAddress;
					SetBinaryData(OffsetImageThunkData, ImageThunkDataIndex, ImageThunkData);
				}
			}
		}

		return true;
	}

	bool FixRelocs(size_t OffsetNtHeader, const IMAGE_NT_HEADERS& ntHd, uintptr_t RemoteImageBase, size_t OffsetBaseRelocation, size_t Size) {
		const size_t Delta = RemoteImageBase - ntHd.OptionalHeader.ImageBase;

		for (size_t nBytes = 0; nBytes < Size; ) {
			IMAGE_BASE_RELOCATION BaseRelocation;
			GetBinaryData(OffsetBaseRelocation, 0, BaseRelocation);

			const size_t OffsetVA = GetOffsetFromRVA(OffsetNtHeader, ntHd, BaseRelocation.VirtualAddress);
			const size_t NumRelocs = (BaseRelocation.SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);

			size_t OffsetRelocationData = OffsetBaseRelocation + sizeof(IMAGE_BASE_RELOCATION);
			for (unsigned i = 0; i < NumRelocs; i++) {
				uint16_t RelocationData;
				GetBinaryData(OffsetRelocationData, i, RelocationData);

				if (!(IMR_RELTYPE(RelocationData) & IMAGE_REL_BASED_HIGHLOW))
					continue;

				uintptr_t pFunc;
				GetBinaryData(OffsetVA + IMR_RELOFFSET(RelocationData), 0, pFunc);
				pFunc += Delta;
				SetBinaryData(OffsetVA + IMR_RELOFFSET(RelocationData), 0, pFunc);
			}

			nBytes += BaseRelocation.SizeOfBlock;
			OffsetBaseRelocation += sizeof(IMAGE_BASE_RELOCATION) + NumRelocs * sizeof(uint16_t);
		}

		return true;
	}

	bool InsertPEHeader(size_t SizeOfHeaders, uintptr_t RemoteImageBase, bool IsVirtualProtectNeeded) const {
		DWORD dwOldProtect;
		if (!process.RemoteWriteProcessMemory((void*)RemoteImageBase, &Binary[0], SizeOfHeaders, 0))
			return false;
		if (IsVirtualProtectNeeded && !process.RemoteVirtualProtect((void*)RemoteImageBase, SizeOfHeaders, PAGE_EXECUTE_READ, &dwOldProtect))
			return false;
		return true;
	}

	bool InsertBinary(size_t OffsetNtHeader, const IMAGE_NT_HEADERS& ntHd, uintptr_t RemoteImageBase, bool IsVirtualProtectNeeded) const {
		DWORD dwOldProtect;
		size_t OffsetImageSectionHeader = GetOffsetOptionalHeader(OffsetNtHeader, ntHd);

		for (unsigned i = 0; i < ntHd.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER ImageSectionHeader;
			GetBinaryData(OffsetImageSectionHeader, i, ImageSectionHeader);

			if (!ImageSectionHeader.SizeOfRawData)
				continue;

			if (!process.RemoteVirtualProtect((void*)(RemoteImageBase + ImageSectionHeader.VirtualAddress),
				ImageSectionHeader.SizeOfRawData, PAGE_READWRITE, &dwOldProtect))
				return false;

			if (!process.RemoteWriteProcessMemory((void*)(RemoteImageBase + ImageSectionHeader.VirtualAddress),
				&Binary[ImageSectionHeader.PointerToRawData], ImageSectionHeader.SizeOfRawData, 0))
				return false;

			if (!IsVirtualProtectNeeded)
				continue;

			DWORD dwProtect;
			switch (ImageSectionHeader.Characteristics & 0xF0000000) {
			case IMAGE_SCN_MEM_EXECUTE:
				dwProtect = PAGE_EXECUTE;
				break;
			case IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ:
				dwProtect = PAGE_EXECUTE_READ;
				break;
			case IMAGE_SCN_MEM_READ:
				dwProtect = PAGE_READONLY;
				break;
			case IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE:
				dwProtect = PAGE_READWRITE;
				break;
			default:
				dwProtect = PAGE_EXECUTE_READWRITE;
				break;
			}

			if (!process.RemoteVirtualProtect((void*)(RemoteImageBase + ImageSectionHeader.VirtualAddress),
				ImageSectionHeader.Misc.VirtualSize, dwProtect, &dwOldProtect))
				return false;
		}

		return true;
	}

	HMODULE MapRemoteModuleA(DWORD dwPid, bool bCallDllMain,
		const char* szParam, EInjectionType InjectionType, const char* szIntoDLL, const char* szDLLDir) {

		IMAGE_DOS_HEADER dosHd;
		GetBinaryData(0, 0, dosHd);
		if (dosHd.e_magic != IMAGE_DOS_SIGNATURE) {
			strMsg << "Invalid DOS signature"e;
			return 0;
		}

		IMAGE_NT_HEADERS ntHd;
		size_t OffsetNtHeader = dosHd.e_lfanew;
		GetBinaryData(OffsetNtHeader, 0, ntHd);
		if (ntHd.Signature != IMAGE_NT_SIGNATURE) {
			strMsg << "Invalid NT signature"e;
			return 0;
		}

		size_t BinaryImageSize = ntHd.OptionalHeader.SizeOfImage;

		if (!(ntHd.FileHeader.Characteristics & IMAGE_FILE_DLL)) {
			strMsg << "File is not DLL"e;
			return 0;
		}

#ifdef _WIN64
		if (ntHd.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
			strMsg << "File is not 64bit application"e;
			return 0;
		}
#else
		if (ntHd.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
			strMsg << "File is not 32bit application"e;
			return 0;
		}
#endif

		if (!process.IsPlatformMatched()) {
#ifdef _WIN64
			strMsg << "Target process is not 64bit process"e;
			return 0;
#else
			strMsg << "Target process is not 32bit process"e;
			return 0;
#endif
		}

		void* pRemoteImageBase = 0;

		switch (InjectionType) {
		case EInjectionType::Normal:
		case EInjectionType::NxBitSwap: {
			pRemoteImageBase = process.RemoteVirtualAlloc(0, ntHd.OptionalHeader.SizeOfImage,
				MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (!pRemoteImageBase) {
				strMsg << "RemoteVirtualAlloc failed"e;
				return 0;
			}

			if (InjectionType == EInjectionType::NxBitSwap) {
				DWORD dwOldProtect;
				if (!process.RemoteVirtualProtect(pRemoteImageBase, ntHd.OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &dwOldProtect) ||
					!process.RemoteVirtualProtect(pRemoteImageBase, ntHd.OptionalHeader.SizeOfImage, PAGE_READWRITE, &dwOldProtect)) {
					strMsg << "RemoteVirtualProtect failed"e;
					return 0;
				}
			}
			break;
		}
		case EInjectionType::IntoDLL: {
			if (!szIntoDLL || !strlen(szIntoDLL)) {
				strMsg << "No DLL specified"e;
				return 0;
			}
			if (RemoteGetModuleHandleA(szIntoDLL)) {
				strMsg = szIntoDLL;
				strMsg += " is already loaded in target process"e;
				return 0;
			}

			pRemoteImageBase = RemoteLoadLibraryExA(szIntoDLL, DONT_RESOLVE_DLL_REFERENCES);
			if (!pRemoteImageBase) {
				strMsg << "RemoteLoadLibrary failed : "e;
				strMsg += szIntoDLL;
				return 0;
			}

			MEMORY_BASIC_INFORMATION MemInfo;
			if (!process.RemoteVirtualQuery((void*)((uintptr_t)pRemoteImageBase + 0x1000), &MemInfo, sizeof(MemInfo))) {
				strMsg << "VirtualQueryEx Failed"e;
				return 0;
			}

			if (MemInfo.RegionSize < BinaryImageSize) {
				strMsg << "Not enough .text section size\nCurrent size : "e;
				strMsg += std::to_string(MemInfo.RegionSize);
				strMsg += "\nRequired size : "e;
				strMsg += std::to_string(BinaryImageSize);
				return 0;
			}

			if (!process.BypassCFG()) {
				strMsg << "BypassCFG Failed"e;
				return 0;
			}
			break;
		}
		}

		if (ntHd.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
			size_t OffsetImportDesc = GetOffsetFromRVA(OffsetNtHeader, ntHd,
				ntHd.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

			if (!FixImports(OffsetNtHeader, ntHd, OffsetImportDesc, szDLLDir))
				return 0;
		}

		if (ntHd.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) {
			size_t OffsetBaseRelocation = GetOffsetFromRVA(OffsetNtHeader, ntHd,
				ntHd.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

			FixRelocs(OffsetNtHeader, ntHd, (uintptr_t)pRemoteImageBase, OffsetBaseRelocation,
				ntHd.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);
		}

		bool IsVirtualProtectNeeded = (InjectionType != EInjectionType::NxBitSwap);
		
		size_t SizeWithoutHeader = ntHd.OptionalHeader.SizeOfImage - 0x1000;
		std::vector<uint8_t> ZeroFill(SizeWithoutHeader, 0);
		if (!process.RemoteWriteProcessMemory((void*)((uintptr_t)pRemoteImageBase + 0x1000), ZeroFill.data(), SizeWithoutHeader, 0)) {
			strMsg << "Initializing space failed"e;
			return 0;
		}

		if (InjectionType != EInjectionType::IntoDLL) {
			if (!InsertPEHeader(ntHd.OptionalHeader.SizeOfHeaders, (uintptr_t)pRemoteImageBase, IsVirtualProtectNeeded)) {
				strMsg << "Inserting PE header failed"e;
				return 0;
			}
		}

		if (!InsertBinary(OffsetNtHeader, ntHd, (uintptr_t)pRemoteImageBase, IsVirtualProtectNeeded)) {
			strMsg << "Inserting binary failed"e;
			return 0;
		}

		if (!bCallDllMain)
			return (HMODULE)pRemoteImageBase;

		bool bSuccess = false;

		process.VirtualAllocWrapper(szParam, strlen(szParam) + 1, [&](const void* pData) {

			auto RemoteCallDLLMain = [&] {
				BOOL Result;
				if (!process.RemoteCall((void*)((uintptr_t)pRemoteImageBase + ntHd.OptionalHeader.AddressOfEntryPoint),
					Result, pRemoteImageBase, DLL_PROCESS_ATTACH, pData, 0))
					return false;
				return true;
			};

			if (InjectionType != EInjectionType::NxBitSwap) {
				bSuccess = RemoteCallDLLMain();
				return;
			}

			if (!process.RemoveNXBit(pRemoteImageBase, ntHd.OptionalHeader.SizeOfImage))
				return;

			ShellCode_Ret1 shellcode_ret1;
			if (!process.WriteProcessMemoryWrapper(VirtualProtect, &shellcode_ret1, sizeof(ShellCode_Ret1), [&] { bSuccess = RemoteCallDLLMain(); })) {
				bSuccess = false;
				return;
			}

			});

		if (!bSuccess) {
			strMsg << "Calling entrypoint failed"e;
			return 0;
		}

		return (HMODULE)pRemoteImageBase;
	}


public:
	std::string GetErrorMsg() const { return strMsg; }

	Injector(const RemoteProcess& process) : process(process) {}
	~Injector() {}

	HMODULE MapRemoteModuleAFromFileName(DWORD dwPid, const char* szModule, bool bCallDLLMain,
		const char* szParam, EInjectionType InjectionType, const char* szIntoDLL) {

		HANDLE hFile = CreateFileA(szModule, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE)
			return 0;

		DWORD dwFileSize = GetFileSize(hFile, 0);
		if (!dwFileSize)
			return 0;

		Binary.resize(dwFileSize);
		if (!ReadFile(hFile, Binary.data(), dwFileSize, 0, 0))
			return 0;

		char szDLLPath[MAX_PATH];
		GetFinalPathNameByHandleA(hFile, szDLLPath, MAX_PATH, FILE_NAME_OPENED);
		std::string strDLLDir = szDLLPath;
		strDLLDir = strDLLDir.substr(0, strDLLDir.find_last_of("\\\\"e));

		CloseHandle(hFile);

		return MapRemoteModuleA(dwPid, bCallDLLMain, szParam, InjectionType, szIntoDLL, strDLLDir.c_str());
	}
};
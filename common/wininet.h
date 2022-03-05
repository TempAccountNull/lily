#pragma once
#include "util.h"
#include <vector>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")

class CWinInet {
private:
	const HINTERNET hInternet = InternetOpenA("HTTPS"e, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
public:
	std::vector<uint8_t> GetVector(std::wstring Url) {
		const HINTERNET hOpenUrl = InternetOpenUrlW(hInternet, Url.c_str(), 0, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);
		if (!hOpenUrl)
			return {};

		std::vector<uint8_t> Result;

		uint8_t Buffer[0x1000];
		DWORD dwBytesRead = 0;
		while (InternetReadFile(hOpenUrl, Buffer, sizeof(Buffer), &dwBytesRead) && dwBytesRead)
			Result.insert(Result.end(), Buffer, Buffer + dwBytesRead);

		InternetCloseHandle(hOpenUrl);
		return Result;
	}

	~CWinInet() { InternetCloseHandle(hInternet); }
};
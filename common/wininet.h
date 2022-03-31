#pragma once
#include "util.h"
#include <vector>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")

class CWinInet {
private:
	const HINTERNET hInternet = InternetOpenA("HTTPS"e, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
public:
	CWinInet() {
		DWORD dwTimeOut = 500;
		InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut));
	}

	std::vector<uint8_t> GetVector(std::wstring Url) const {
		const HINTERNET hOpenUrl = InternetOpenUrlW(hInternet, Url.c_str(), 0, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
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

	std::vector<uint8_t> GetVectorVerified(std::wstring Url) const {
		std::vector<uint8_t> Result = GetVector(Url);
		if (Result.empty())
			error(Url.c_str());
		return Result;
	}

	~CWinInet() { InternetCloseHandle(hInternet); }
};
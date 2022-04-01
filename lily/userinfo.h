#pragma once
#include "common/download.h"
#include "common/render.h"
#include "common/json.hpp"

struct tUserInfo {
	int rankPoint = -1;
};

class CUserInfo {
private:
	Download download;
	Render& render;

	float LastUpdateTime = 0.0f;
	constexpr static float InvalidateTime = 60.0f * 60.0f;	//1hour
	constexpr static float RefreshWaitTime = 4.0f;

	const std::string TempPath = [&] {
		char szTempPath[0x100];
		GetTempPathA(sizeof(szTempPath), szTempPath);
		return (std::string)szTempPath;
	}();

public:
	enum class Status {
		None,
		RetrySync,
		WaitSync,
		WaitInfo,
		Done,
		Error
	};

	struct User {
		bool bKakao = false;
		Status Code = Status::None;
		float WaitTime = 0.0f;
		float DoneTime = 0.0f;
	};

	std::map<std::string, User> UserList;
	std::map<unsigned, tUserInfo> InfoSteamSolo;
	std::map<unsigned, tUserInfo> InfoSteamSquad;
	std::map<unsigned, tUserInfo> InfoSteamSquadFPP;
	std::map<unsigned, tUserInfo> InfoKakaoSquad;
	std::map<unsigned, tUserInfo> InfoEmpty;

	CUserInfo(Render& render) : render(render) {}

	void AddUser(std::string UserName, bool bKakao) {
		if (UserName.empty())
			return;

		if (UserList.find(UserName) != UserList.end()) {
			auto& User = UserList[UserName];
			if (User.Code != Status::Done)
				return;
			if (render.TimeSeconds < User.DoneTime + InvalidateTime)
				return;
		}

		UserList[UserName] = { bKakao, Status::None, 0.0f, 0.0f };
	}

	void Update() {
		if (LastUpdateTime == render.TimeSeconds)
			return;
		LastUpdateTime = render.TimeSeconds;

		download.UpdateDownloadStatus();

		for (auto& Elem : UserList) {
			std::string UserName = Elem.first;
			auto& User = Elem.second;

			User.WaitTime = std::clamp(User.WaitTime - render.TimeDelta, 0.0f, FLT_MAX);

			const std::string Platform = User.bKakao ? (std::string)"kakao/"e : (std::string)"steam/"e;
			const std::string SyncUrl = (std::string)"https://pubg.dakgg.io/api/v1/rpc/player-sync/"e + Platform + UserName;
			const std::string InfoUrl = (std::string)"https://pubg.dakgg.io/api/v1/players/"e + Platform + UserName +
				(std::string)"/seasons/division.bro.official.pc-2018-16"e;

			const std::string InfoPath = TempPath + UserName + (std::string)"_info_1004"e;
			const std::string SyncPath = TempPath + UserName + (std::string)"_sync_1004"e;

			switch (User.Code) {
			case Status::None:
			{
				download.AddDownload(SyncUrl, SyncPath);
				User.Code = Status::WaitSync;
				break;
			}
			case Status::RetrySync:
			{
				if (User.WaitTime == 0.0f)
					User.Code = Status::None;
				break;
			}
			case Status::WaitSync:
			{
				switch (download.GetExitCode(SyncUrl, SyncPath)) {
				case -1:
					break;
				case 0:
				{
					std::vector<uint8_t> JsonData = LoadFromFile(SyncPath.c_str());
					DeleteFileA(SyncPath.c_str());
					std::string JsonString(JsonData.begin(), JsonData.end());

					auto Parsed = json::JSON::Load(JsonString);
					if (Parsed.hasKey("retryAfter"e)) {
						User.WaitTime = RefreshWaitTime;
						User.Code = Status::RetrySync;
						break;
					}

					download.AddDownload(InfoUrl, InfoPath);
					User.Code = Status::WaitInfo;
					break;
				}
				default:
					User.Code = Status::Error;
					break;
				}
			}
			case Status::WaitInfo:
			{
				switch (download.GetExitCode(InfoUrl, InfoPath)) {
				case -1:
					break;
				case 0:
				{
					std::vector<uint8_t> JsonData = LoadFromFile(InfoPath.c_str());
					DeleteFileA(InfoPath.c_str());
					std::string JsonString(JsonData.begin(), JsonData.end());

					const unsigned NameHash = CompileTime::StrHash(UserName.c_str());
					auto Parsed = json::JSON::Load(JsonString);
					if (Parsed.hasKey("rankedStats"e)) {
						auto& RankedStats = Parsed["rankedStats"e];
						if (User.bKakao)
							InfoKakaoSquad[NameHash] = { RankedStats.hasKey("squad"e) ? RankedStats["squad"e]["currentRankPoint"e].ToInt() : 0 };
						else {
							InfoSteamSolo[NameHash] = { RankedStats.hasKey("solo"e) ? RankedStats["solo"e]["currentRankPoint"e].ToInt() : 0 };
							InfoSteamSquad[NameHash] = { RankedStats.hasKey("squad"e) ? RankedStats["squad"e]["currentRankPoint"e].ToInt() : 0 };
							InfoSteamSquadFPP[NameHash] = { RankedStats.hasKey("squad-fpp"e) ? RankedStats["squad-fpp"e]["currentRankPoint"e].ToInt() : 0 };
						}
					}
					else {
						InfoKakaoSquad[NameHash] =
							InfoSteamSolo[NameHash] =
							InfoSteamSquad[NameHash] =
							InfoSteamSquadFPP[NameHash] =
						{ -1 };
					}
					User.DoneTime = render.TimeSeconds;
					User.Code = Status::Done;
					break;
				}
				default:
					User.Code = Status::Error;
					break;
				}
				break;
			}
			case Status::Done:
				break;
			case Status::Error:
				error(UserName.c_str());
				break;
			}
		}
	}
};
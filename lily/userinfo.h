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

	constexpr static float InvalidateTime = 60.0f * 60.0f;	//1hour
	constexpr static float RefreshWaitTime = 5.0f;

public:
	enum class Status {
		None,
		Retry,
		WaitSync,
		WaitInfo,
		Done,
	};

	struct User {
		bool bKakao = false;
		Status Code = Status::None;
		float RetryTime = 0.0f;
		float DoneTime = 0.0f;
	};

	std::map<std::string, User> UserList;
	std::map<unsigned, tUserInfo> InfoSteamSolo;
	std::map<unsigned, tUserInfo> InfoSteamSquad;
	std::map<unsigned, tUserInfo> InfoSteamSquadFPP;
	std::map<unsigned, tUserInfo> InfoKakaoSquad;
	std::map<unsigned, tUserInfo> InfoEmpty;

	CUserInfo(Render& render) {}

	void AddUser(std::string UserName, bool bKakao) {
		if (UserName.empty())
			return;

		if (UserList.find(UserName) != UserList.end()) {
			auto& User = UserList[UserName];
			if (User.Code != Status::Done)
				return;
			if (GetTimeSeconds() < User.DoneTime + InvalidateTime)
				return;
		}

		UserList[UserName] = { bKakao, Status::None, 0.0f, 0.0f };
	}

	void Update() {
		float TimeSeconds = GetTimeSeconds();
		download.Update();

		for (auto& Elem : UserList) {
			std::string UserName = Elem.first;
			auto& User = Elem.second;

			const std::string Platform = User.bKakao ? (std::string)"kakao/"e : (std::string)"steam/"e;
			const std::string SyncUrl = (std::string)"https://pubg.dakgg.io/api/v1/rpc/player-sync/"e + Platform + UserName;
			const std::string InfoUrl = (std::string)"https://pubg.dakgg.io/api/v1/players/"e + Platform + UserName +
				(std::string)"/seasons/division.bro.official.pc-2018-16"e;

			switch (User.Code) {
			case Status::None:
			{
				download.Add(SyncUrl);
				User.Code = Status::WaitSync;
				break;
			}
			case Status::Retry:
			{
				if (TimeSeconds > User.RetryTime)
					User.Code = Status::None;
				break;
			}
			case Status::WaitSync:
			{
				switch (download.GetStatus(SyncUrl)) {
				case DownloadStatus::WaitOpenUrl:
				case DownloadStatus::WaitRead:
					break;
				case DownloadStatus::Done:
				{
					std::vector<uint8_t> JsonData = download.GetData(SyncUrl);
					download.RemoveData(SyncUrl);
					std::string JsonString(JsonData.begin(), JsonData.end());

					auto Parsed = json::JSON::Load(JsonString);
					if (Parsed.hasKey("retryAfter"e)) {
						User.RetryTime = TimeSeconds + RefreshWaitTime;
						User.Code = Status::Retry;
						break;
					}

					download.Add(InfoUrl);
					User.Code = Status::WaitInfo;
					break;
				}
				case DownloadStatus::Failed:
					User.RetryTime = TimeSeconds + RefreshWaitTime;
					User.Code = Status::Retry;
					break;
				default:
					error(UserName.c_str());
					break;
				}
				break;
			}
			case Status::WaitInfo:
			{
				switch (download.GetStatus(InfoUrl)) {
				case DownloadStatus::WaitOpenUrl:
				case DownloadStatus::WaitRead:
					break;
				case DownloadStatus::Done:
				{
					std::vector<uint8_t> JsonData = download.GetData(InfoUrl);
					download.RemoveData(InfoUrl);
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
					User.DoneTime = TimeSeconds;
					User.Code = Status::Done;
					break;
				}
				case DownloadStatus::Failed:
					User.RetryTime = TimeSeconds + RefreshWaitTime;
					User.Code = Status::Retry;
					break;
				default:
					error(UserName.c_str());
					break;
				}
				break;
			}
			case Status::Done:
				break;
			}
		}
	}
};
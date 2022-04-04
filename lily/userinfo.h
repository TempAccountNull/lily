#pragma once
#include "common/download.h"
#include "common/json.hpp"

struct tUserInfo {
	bool bValid = false;
	bool bExist = false;
	unsigned rankPoint = 0;
	float Damage = 0.0f;

	tUserInfo() {}
	tUserInfo(json::JSON& rankedStats, std::string Key) {
		bValid = true;
		if (!rankedStats.hasKey(Key))
			return;
		bExist = true;
		rankPoint = rankedStats[Key]["currentRankPoint"e].ToInt();

		float TotalDamage =
			rankedStats[Key]["damageDealt"e].JSONType() == json::JSON::Class::Floating ?
			(float)rankedStats[Key]["damageDealt"e].ToFloat() :
			(float)rankedStats[Key]["damageDealt"e].ToInt();
		int RoundsPlayed = rankedStats[Key]["roundsPlayed"e].ToInt();
		Damage = RoundsPlayed ? TotalDamage / RoundsPlayed : 0;
	}
};

class CUserInfo {
private:
	Download download;

	constexpr static float InvalidateTime = 60.0f * 60.0f;	//1hour
	constexpr static float WaitTime = 5.0f;

	enum class Status {
		Reset,
		StartSync,
		StartInfo,
		WaitSync,
		WaitInfo,
		Done,
	};

	struct User {
		bool bKakao = false;
		Status Code = Status::StartInfo;
		float WaitUntil = 0.0f;
		float SyncTime = -FLT_MAX;
	};

	std::map<std::string, User> UserList;

public:
	std::map<unsigned, tUserInfo> InfoSteamSolo;
	std::map<unsigned, tUserInfo> InfoSteamSquad;
	std::map<unsigned, tUserInfo> InfoSteamSquadFPP;
	std::map<unsigned, tUserInfo> InfoKakaoSquad;
	std::map<unsigned, tUserInfo> InfoEmpty;

	void Invalidate(std::string UserName, bool bKakao) {
		if (UserName.empty())
			return;
		if (UserList.find(UserName) != UserList.end())
			UserList[UserName].SyncTime = -FLT_MAX;
	}

	void AddUser(std::string UserName, bool bKakao) {
		if (UserName.empty())
			return;

		if (UserList.find(UserName) != UserList.end()) {
			auto& User = UserList[UserName];
			if (User.Code == Status::Done && GetTimeSeconds() > User.SyncTime + InvalidateTime)
				UserList[UserName] = { bKakao, Status::StartSync };
			return;
		}

		UserList[UserName] = { bKakao, Status::StartInfo };
	}

	void UpdateInfoFromJson(std::string UserName, bool bKakao, std::string JsonString) {
		if (UserName.empty())
			return;

		const unsigned NameHash = CompileTime::StrHash(UserName.c_str());
		auto Parsed = json::JSON::Load(JsonString);
		if (!Parsed.hasKey("rankedStats"e)) {
			if (bKakao)
				InfoKakaoSquad[NameHash] = {};
			else {
				InfoSteamSolo[NameHash] = {};
				InfoSteamSquad[NameHash] = {};
				InfoSteamSquadFPP[NameHash] = {};
			}
			return;
		}

		auto& RankedStats = Parsed["rankedStats"e];
		if (bKakao)
			InfoKakaoSquad[NameHash] = { RankedStats, "squad"e };
		else {
			InfoSteamSolo[NameHash] = { RankedStats, "solo"e };
			InfoSteamSquad[NameHash] = { RankedStats, "squad"e };
			InfoSteamSquadFPP[NameHash] = { RankedStats, "squad-fpp"e };
		}
	}

	void Update() {
		float TimeSeconds = GetTimeSeconds();
		download.Update();

		for (auto& Elem : UserList) {
			std::string UserName = Elem.first;
			auto& User = Elem.second;

			if (TimeSeconds < User.WaitUntil)
				continue;

			const std::string Platform = User.bKakao ? (std::string)"kakao/"e : (std::string)"steam/"e;
			const std::string SyncUrl = (std::string)"https://pubg.dakgg.io/api/v1/rpc/player-sync/"e + Platform + UserName;
			const std::string InfoUrl = (std::string)"https://pubg.dakgg.io/api/v1/players/"e + Platform + UserName +
				(std::string)"/seasons/division.bro.official.pc-2018-16"e;

			switch (User.Code) {
			case Status::Reset:
			{
				User = { User.bKakao, Status::StartInfo, 0.0f, 0.0f };
				break;
			}
			case Status::StartSync:
			{
				download.Add(SyncUrl);
				User.Code = Status::WaitSync;
				break;
			}
			case Status::StartInfo:
			{
				download.Add(InfoUrl);
				User.Code = Status::WaitInfo;
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
						User.WaitUntil = TimeSeconds + WaitTime;
						User.Code = Status::StartSync;
						break;
					}

					User.SyncTime = TimeSeconds;
					User.Code = Status::StartInfo;
					break;
				}
				case DownloadStatus::Failed:
					User.WaitUntil = TimeSeconds + WaitTime;
					User.Code = Status::Reset;
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
					UpdateInfoFromJson(UserName, User.bKakao, JsonString);
					User.Code = Status::Done;
					break;
				}
				case DownloadStatus::Failed:
					User.WaitUntil = TimeSeconds + WaitTime;
					User.Code = Status::Reset;
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
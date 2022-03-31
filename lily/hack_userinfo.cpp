#include "hack.h"
#include "common/json.hpp"

std::map<unsigned, RankInfo> GetRankInfo(const CWinInet& WinInet, std::wstring BaseUrl) {
	std::map<unsigned, RankInfo> Result;

	for (unsigned i = 1; i <= 10; i++) {
		std::wstring Url = BaseUrl + (std::wstring)L"?page="e + std::to_wstring(i);
		std::vector<uint8_t> JsonData = WinInet.GetVectorVerified(Url);
		std::string JsonString(JsonData.begin(), JsonData.end());

		auto Parsed = json::JSON::Load(JsonString);

		for (auto& Elem : Parsed["ranks"e].ArrayRange()) {
			unsigned NameHash = CompileTime::StrHash(Elem["name"e].ToString().c_str());
			unsigned rankPoint = Elem["rankPoint"e].ToInt();
			Result[NameHash].rankPoint = rankPoint;
		}
	}

	return Result;
}

void Hack::UpdateRankInfo() {
	std::wstring BaseUrl = L"https://pubg.dakgg.io/api/v1/ranks/"e;
	RankInfoSteamSolo = GetRankInfo(WinInet, BaseUrl + (std::wstring)L"steam/solo"e);
	RankInfoSteamSquad = GetRankInfo(WinInet, BaseUrl + (std::wstring)L"steam/squad"e);
	RankInfoSteamSquadFPP = GetRankInfo(WinInet, BaseUrl + (std::wstring)L"steam/squad-fpp"e);
	RankInfoKakaoSquad = GetRankInfo(WinInet, BaseUrl + (std::wstring)L"kakao/squad"e);
}

void Hack::UpdateUserInfo(const char* szUserName, bool bKakao) {
	const unsigned NameHash = CompileTime::StrHash(szUserName);
	const std::wstring UserName = s2ws(szUserName);

	std::wstring Url = L"https://pubg.dakgg.io/api/v1/rpc/player-sync/"e;
	Url += bKakao ? (std::wstring)L"kakao/"e : L"steam/"e;
	Url += UserName;
	WinInet.GetVector(Url);

	Url = (std::wstring)L"https://pubg.dakgg.io/api/v1/players/"e;
	Url += bKakao ? (std::wstring)L"kakao/"e : L"steam/"e;
	Url += UserName;
	Url += (std::wstring)L"/seasons/division.bro.official.pc-2018-16"e;
	std::vector<uint8_t> JsonData = WinInet.GetVectorVerified(Url);
	std::string JsonString(JsonData.begin(), JsonData.end());

	auto Parsed = json::JSON::Load(JsonString);
	auto& RankedStats = Parsed["rankedStats"e];

	if (bKakao) {
		if (RankedStats.hasKey("squad"e))
			RankInfoKakaoSquad[NameHash].rankPoint = RankedStats["squad"e]["currentRankPoint"e].ToInt();
	}
	else {
		if (RankedStats.hasKey("solo"e))
			RankInfoSteamSolo[NameHash].rankPoint = RankedStats["solo"e]["currentRankPoint"e].ToInt();
		if (RankedStats.hasKey("squad"e))
			RankInfoSteamSquad[NameHash].rankPoint = RankedStats["squad"e]["currentRankPoint"e].ToInt();
		if (RankedStats.hasKey("squad-fpp"e))
			RankInfoSteamSquadFPP[NameHash].rankPoint = RankedStats["squad-fpp"e]["currentRankPoint"e].ToInt();
	}
}

void Hack::OpenWebUserInfo(const char* szUserName) {
	std::string url = (std::string)"https://pubg.op.gg/user/"e + szUserName;
	ShellExecuteA(0, "open"e, url.c_str(), 0, 0, SW_SHOWNA);
}
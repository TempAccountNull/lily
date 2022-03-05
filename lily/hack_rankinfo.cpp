#include "hack.h"
#include "common/wininet.h"
#include "common/json.hpp"

#define URL_RanksSteamSolo L"https://pubg.dakgg.io/api/v1/ranks/steam/solo"e
#define URL_RanksSteamSquad L"https://pubg.dakgg.io/api/v1/ranks/steam/squad"e
#define URL_RanksSteamSquadFPP L"https://pubg.dakgg.io/api/v1/ranks/steam/squad-fpp"e
#define URL_RanksKakaoSquad L"https://pubg.dakgg.io/api/v1/ranks/kakao/squad"e

std::map<unsigned, RankInfo> GetRankInfo(std::wstring BaseUrl) {
	CWinInet WinInet;
	std::map<unsigned, RankInfo> Result;

	for (unsigned i = 1; i <= 10; i++) {
		std::wstring Url = BaseUrl + (std::wstring)L"?page="e + std::to_wstring(i);
		std::vector<uint8_t> JsonData = WinInet.GetVector(Url);
		if (JsonData.empty())
			error(Url.c_str());

		using namespace json;

		std::string JsonString(JsonData.begin(), JsonData.end());

		auto Parsed = JSON::Load(JsonString);

		for (auto& Elem : Parsed["ranks"e].ArrayRange()) {
			unsigned NameHash = CompileTime::StrHash(Elem["name"e].ToString().c_str());
			unsigned rankPoint = Elem["rankPoint"e].ToInt();
			Result[NameHash].rankPoint = rankPoint;
		}
	}

	return Result;
}

void Hack::UpdateRankInfo() {
	RankInfoSteamSolo = GetRankInfo(URL_RanksSteamSolo);
	RankInfoSteamSquad = GetRankInfo(URL_RanksSteamSquad);
	RankInfoSteamSquadFPP = GetRankInfo(URL_RanksSteamSquadFPP);
	RankInfoKakaoSquad = GetRankInfo(URL_RanksKakaoSquad);
}
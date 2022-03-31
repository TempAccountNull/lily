#include "hack.h"
#include "common/wininet.h"
#include "common/json.hpp"

std::map<unsigned, RankInfo> GetRankInfo(std::wstring BaseUrl) {
	std::map<unsigned, RankInfo> Result;

	for (unsigned i = 1; i <= 10; i++) {
		std::wstring Url = BaseUrl + (std::wstring)L"?page="e + std::to_wstring(i);
		std::vector<uint8_t> JsonData = CWinInet().GetVectorVerified(Url);
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
	RankInfoSteamSolo = GetRankInfo(BaseUrl + (std::wstring)L"steam/solo"e);
	RankInfoSteamSquad = GetRankInfo(BaseUrl + (std::wstring)L"steam/squad"e);
	RankInfoSteamSquadFPP = GetRankInfo(BaseUrl + (std::wstring)L"steam/squad-fpp"e);
	RankInfoKakaoSquad = GetRankInfo(BaseUrl + (std::wstring)L"kakao/squad"e);
}

bool Hack::RefreshUserInfo(const char* szUserName, bool bKakao) {
	std::wstring Url = L"https://pubg.dakgg.io/api/v1/rpc/player-sync/"e;
	Url += bKakao ? (std::wstring)L"kakao/"e : L"steam/"e;
	Url += s2ws(szUserName);
	std::vector<uint8_t> JsonData = CWinInet().GetVectorVerified(Url);
	std::string JsonString(JsonData.begin(), JsonData.end());

	auto Parsed = json::JSON::Load(JsonString);
	return !Parsed.hasKey("retryAfter"e);
}

void Hack::UpdateUserInfo(const char* szUserName, bool bKakao) {
	std::wstring Url = (std::wstring)L"https://pubg.dakgg.io/api/v1/players/"e;
	Url += bKakao ? (std::wstring)L"kakao/"e : L"steam/"e;
	Url += s2ws(szUserName);
	Url += (std::wstring)L"/seasons/division.bro.official.pc-2018-16"e;
	std::vector<uint8_t> JsonData = CWinInet().GetVectorVerified(Url);
	std::string JsonString(JsonData.begin(), JsonData.end());

	auto Parsed = json::JSON::Load(JsonString);
	verify(Parsed.hasKey("rankedStats"e));
	auto& RankedStats = Parsed["rankedStats"e];

	const unsigned NameHash = CompileTime::StrHash(szUserName);
	if (bKakao)
		RankInfoKakaoSquad[NameHash].rankPoint = RankedStats.hasKey("squad"e) ? RankedStats["squad"e]["currentRankPoint"e].ToInt() : 0;
	else {
		RankInfoSteamSolo[NameHash].rankPoint = RankedStats.hasKey("solo"e) ? RankedStats["solo"e]["currentRankPoint"e].ToInt() : 0;
		RankInfoSteamSquad[NameHash].rankPoint = RankedStats.hasKey("squad"e) ? RankedStats["squad"e]["currentRankPoint"e].ToInt() : 0;
		RankInfoSteamSquadFPP[NameHash].rankPoint = RankedStats.hasKey("squad-fpp"e) ? RankedStats["squad-fpp"e]["currentRankPoint"e].ToInt() : 0;
	}
}

void Hack::OpenWebUserInfo(const char* szUserName) {
	std::string url = (std::string)"https://pubg.op.gg/user/"e + szUserName;
	ShellExecuteA(0, "open"e, url.c_str(), 0, 0, SW_SHOW);
}
#pragma once
#include "common/util.h"

static fixstr::basic_fixed_string<char, 0x100> GetWeaponName(unsigned Hash) {
	switch (Hash) {
		HASH_DEFAULT(""e);

		//Projectile
		HASH_CASE("WeapFlashBang_C"h, "Flash"e);
		HASH_CASE("WeapGrenade_C"h, "Grenade"e);
		HASH_CASE("WeapStickyGrenade_C"h, "Bomb"e);
		HASH_CASE("WeapMolotov_C"h, "Molotov"e);
		HASH_CASE("WeapSmokeBomb_C"h, "Smoke"e);
		HASH_CASE("WeapC4_C"h, "C4"e);
		HASH_CASE("WeapDecoyGrenade_C"h, "Decoy"e);
		HASH_CASE("WeapBluezoneGrenade_C"h, "BlueBomb"e);

		//Melee
		HASH_CASE("WeapCowbar_C"h, "Crawbar"e);
		HASH_CASE("WeapCowbarProjectile_C"h, "Crawbar"e);
		HASH_CASE("WeapMachete_C"h, "Machete"e);
		HASH_CASE("WeapMacheteProjectile_C"h, "Machete"e);
		HASH_CASE("WeapPan_C"h, "Pan"e);
		HASH_CASE("WeapPanProjectile_C"h, "Pan"e);
		HASH_CASE("WeapSickle_C"h, "Sickle"e);
		HASH_CASE("WeapSickleProjectile_C"h, "Sickle"e);

		//Normal
		HASH_CASE("WeapLunchmeatsAK47_C"h, "AK"e);
		HASH_CASE("WeapAK47_C"h, "AK"e);
		HASH_CASE("WeapAWM_C"h, "AWM"e);
		HASH_CASE("WeapBerreta686_C"h, "S686"e);
		HASH_CASE("WeapCrossbow_1_C"h, "CBow"e);
		HASH_CASE("WeapG18_C"h, "P18C"e);
		HASH_CASE("WeapGroza_C"h, "Groza"e);
		HASH_CASE("WeapDuncansHK416_C"h, "M4"e);
		HASH_CASE("WeapHK416_C"h, "M4"e);
		HASH_CASE("WeapJuliesKar98k_C"h, "K98"e);
		HASH_CASE("WeapKar98k_C"h, "K98"e);
		HASH_CASE("WeapM16A4_C"h, "M16"e);
		HASH_CASE("WeapM1911_C"h, "P1911"e);
		HASH_CASE("WeapJuliesM24_C"h, "M24"e);
		HASH_CASE("WeapM24_C"h, "M24"e);
		HASH_CASE("WeapM249_C"h, "M249"e);
		HASH_CASE("WeapM9_C"h, "P92"e);
		HASH_CASE("WeapMini14_C"h, "Mini"e);
		HASH_CASE("WeapMk14_C"h, "Mk14"e);
		HASH_CASE("WeapNagantM1895_C"h, "R1895"e);
		HASH_CASE("WeapSaiga12_C"h, "S12K"e);
		HASH_CASE("WeapSCAR-L_C"h, "SCAR"e);
		HASH_CASE("WeapSKS_C"h, "SKS"e);
		HASH_CASE("WeapThompson_C"h, "Tom"e);
		HASH_CASE("WeapUMP_C"h, "UMP"e);
		HASH_CASE("WeapUZI_C"h, "UZI"e);
		HASH_CASE("WeapUziPro_C"h, "UZI"e);
		HASH_CASE("WeapVector_C"h, "Vec"e);
		HASH_CASE("WeapVSS_C"h, "VSS"e);
		HASH_CASE("WeapWinchester_C"h, "S1897"e);
		HASH_CASE("WeapAUG_C"h, "AUG"e);
		HASH_CASE("WeapBerylM762_C"h, "Beryl"e);
		HASH_CASE("WeapBizonPP19_C"h, "Bizon"e);
		HASH_CASE("WeapDesertEagle_C"h, "Deagle"e);
		HASH_CASE("WeapDP28_C"h, "DP28"e);
		HASH_CASE("WeapFNFal_C"h, "SLR"e);
		HASH_CASE("WeapMadsFNFal_C"h, "SLR"e);
		HASH_CASE("WeapG36C_C"h, "G36C"e);
		HASH_CASE("WeapMk47Mutant_C"h, "Mutant"e);
		HASH_CASE("WeapMP5K_C"h, "MP5K"e);
		HASH_CASE("WeapMadsQBU88_C"h, "QBU"e);
		HASH_CASE("WeapQBU88_C"h, "QBU"e);
		HASH_CASE("WeapQBZ95_C"h, "QBZ"e);
		HASH_CASE("WeapRhino_C"h, "R45"e);
		HASH_CASE("WeapSawnoff_C"h, "Sawnoff"e);
		HASH_CASE("Weapvz61Skorpion_C"h, "Skorpion"e);
		HASH_CASE("WeapWin94_C"h, "Win94"e);
		HASH_CASE("WeapDP12_C"h, "DBS"e);
		HASH_CASE("WeapMG3_C"h, "MG3"e);
		HASH_CASE("WeapL6_C"h, "Lynx"e);
		HASH_CASE("WeapMosinNagant_C"h, "Mosin"e);
		HASH_CASE("WeapK2_C"h, "K2"e);
		HASH_CASE("WeapMk12_C"h, "Mk12"e);
		HASH_CASE("WeapP90_C"h, "P90"e);
		HASH_CASE("WeapM79_C"h, "M79"e);

		//Special
		HASH_CASE("WeapMortar_C"h, "Mortar"e);
		HASH_CASE("WeapSpikeTrap_C"h, "Trap"e);
		HASH_CASE("WeapFlareGun_C"h, "FlareGun"e);
		HASH_CASE("WeapPanzerFaust100M1_C"h, "Rocket"e);
		HASH_CASE("WeapJerryCan_C"h, "Fuel"e);
		HASH_CASE("WeapDrone_C"h, "Drone"e);
		HASH_CASE("WeapTraumaBag_C"h, "TraumaBag"e);
	}
}
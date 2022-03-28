#pragma once
#include "common/util.h"

struct ProjInfo {
	fixstr::basic_fixed_string<char, 0x100> Name;
	bool IsLong;
};

static ProjInfo GetProjInfo(unsigned Hash) {
	switch (Hash) {
		HASH_DEFAULT({ ""e, false });

		HASH_CASE("ProjGrenade_C"h, { "Grenade"e, false });
		HASH_CASE("ProjFlashBang_C"h, { "Flash"e, false });
		HASH_CASE("ProjMolotov_C"h, { "Molotov"e, false });
		HASH_CASE("ProjSmokeBomb_v2_C"h, { "Smoke"e, false });
		HASH_CASE("ProjSpiketrap_C"h, { "Spike"e, false });
		HASH_CASE("BP_Spiketrap_C"h, { "Spike"e, false });
		HASH_CASE("ProjStickyGrenade_C"h, { "Bomb"e, false });
		HASH_CASE("PanzerFaust100M_Projectile_C"h, { "Rocket"e, false });
		HASH_CASE("ProjC4_C"h, { "C4"e, false });
		HASH_CASE("ProjDecoyGrenade_C"h, { "Decoy"e, false });
		HASH_CASE("BP_Projectile_40mm_Smoke_C"h, { "Smoke"e, false });
		HASH_CASE("ProjBluezoneBomb_C"h, { "BlueBomb"e, false });
		HASH_CASE("WeapMortar_C"h, { "Mortar"e, false });
		HASH_CASE("Mortar_Projectile_C"h, { "Shell"e, true });
		HASH_CASE("BP_Drone_C"h, { "Drone"e, false });
		HASH_CASE("Flare_Projectile_C"h, { "Flare"e, true });

		HASH_CASE("BP_KillTruck_C"h, { "KillTruck"e, true });
		HASH_CASE("BP_Helicopter_C"h, { "Helicopter"e, true });
	}
}
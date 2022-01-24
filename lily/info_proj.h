#pragma once
#include "common/util.h"

static fixstr::basic_fixed_string<char, 0x100> GetProjName(unsigned Hash) {
	switch (Hash) {
		HASH_DEFAULT(""e);

		HASH_CASE("ProjGrenade_C"h, "Grenade"e);
		HASH_CASE("ProjFlashBang_C"h, "Flash"e);
		HASH_CASE("ProjMolotov_C"h, "Molotov"e);
		HASH_CASE("ProjSmokeBomb_v2_C"h, "Smoke"e);
		HASH_CASE("ProjSpiketrap_C"h, "Spike"e);
		HASH_CASE("BP_Spiketrap_C"h, "Spike"e);
		HASH_CASE("ProjStickyGrenade_C"h, "Bomb"e);
		HASH_CASE("PanzerFaust100M_Projectile_C"h, "Rocket"e);
		HASH_CASE("ProjC4_C"h, "C4"e);
		HASH_CASE("ProjDecoyGrenade_C"h, "Decoy"e);
		HASH_CASE("BP_Projectile_40mm_Smoke_C"h, "Smoke"e);
		HASH_CASE("ProjBluezoneBomb_C"h, "BlueBomb"e);
		HASH_CASE("ProjMortar_Explosion_C"h, "Mortar"e);
		HASH_CASE("ProjMortar_Trail_C"h, "Mortar"e);
		HASH_CASE("WeapMortar_C"h, "Mortar"e);
		HASH_CASE("BP_Drone_C"h, "Drone"e);
	}
}
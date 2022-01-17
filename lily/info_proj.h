#pragma once
#include "common/encrypt_string.h"
#include "common/compiletime.h"
#include "common/hash_switch.h"
#include "global.h"

GenerateHashSwitch(GetProjName, char* Buf,
	HashCase("ProjGrenade_C"h, "Grenade"eg)
	HashCase("ProjFlashBang_C"h, "Flash"eg)
	HashCase("ProjMolotov_C"h, "Molotov"eg)
	HashCase("ProjSmokeBomb_v2_C"h, "Smoke"eg)
	HashCase("ProjSpiketrap_C"h, "Spike"eg)
	HashCase("BP_Spiketrap_C"h, "Spike"eg)
	HashCase("ProjStickyGrenade_C"h, "Bomb"eg)
	HashCase("PanzerFaust100M_Projectile_C"h, "Rocket"eg)
	HashCase("ProjC4_C"h, "C4"eg)
	HashCase("ProjDecoyGrenade_C"h, "Decoy"eg)
	HashCase("BP_Projectile_40mm_Smoke_C"h, "Smoke"eg)
	HashCase("ProjBluezoneBomb_C"h, "BlueBomb"eg)
	HashCase("ProjMortar_Explosion_C"h, "Mortar"eg)
	HashCase("ProjMortar_Trail_C"h, "Mortar"eg)
	HashCase("WeapMortar_C"h, "Mortar"eg)
	HashDefault(""eg)
)
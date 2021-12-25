#pragma once
#include "encrypt_string.hpp"
#include "compiletime.hpp"
#include "hash_switch.hpp"
#include "global.h"

GenerateHashSwitch(GetPackageName, char* Buf,
	HashCase("DeathDropItemPackage_C"h, "Box"eg)
	HashCase("Carapackage_RedBox_C"h, "Supply"eg)
	HashCase("Carapackage_FlareGun_C"h, "Supply"eg)
	HashCase("BP_Loot_AmmoBox_C"h, "Ammo"eg)
	HashCase("BP_Loot_MilitaryCrate_C"h, "Crate"eg)
	HashCase("BP_Loot_TruckHaul_C"h, "LootTruck"eg)
	HashCase("Carapackage_SmallPackage_C"h, "Small"eg)
	HashDefault(""eg)
)
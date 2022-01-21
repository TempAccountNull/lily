#pragma once
#include "common/util.h"

static fixstr::basic_fixed_string<char, 0x100> GetPackageName(unsigned Hash) {
	switch (Hash) {
		HASH_DEFAULT(""e);

		HASH_CASE("DeathDropItemPackage_C"h, "Box"e);
		HASH_CASE("Carapackage_RedBox_C"h, "Supply"e);
		HASH_CASE("Carapackage_FlareGun_C"h, "Supply"e);
		HASH_CASE("BP_Loot_AmmoBox_C"h, "Ammo"e);
		HASH_CASE("BP_Loot_MilitaryCrate_C"h, "Crate"e);
		HASH_CASE("BP_Loot_TruckHaul_C"h, "LootTruck"e);
		HASH_CASE("Carapackage_SmallPackage_C"h, "Small"e);
	}
}
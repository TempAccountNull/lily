#pragma once
#include "encrypt_string.hpp"
#include "compiletime.hpp"
#include "hash_switch.hpp"
#include "global.h"

enum class VehicleType1 {
	Wheeled,
	Floating,
	None
};

enum class VehicleType2 {
	Destructible,
	Invincible
};

enum class VehicleType3 {
	Normal,
	Special
};

GenerateHashSwitch(GetVehicleInfo, char* Buf,
	HashCase("BP_KillTruck_C"h, "KillTruck"eg, VehicleType1::None, VehicleType2::Invincible, VehicleType3::Special)
	HashCase("BP_EmergencyPickupVehicle_C"h, "Pickup"eg, VehicleType1::None, VehicleType2::Invincible, VehicleType3::Special)
	HashCase("TransportAircraft_Chimera_C"h, "Helicopter"eg, VehicleType1::None, VehicleType2::Invincible, VehicleType3::Special)
	HashCase("BP_Bicycle_C"h, "BikeM"eg, VehicleType1::None, VehicleType2::Invincible, VehicleType3::Normal)

	HashCase("BP_BRDM_C"h, "BRDM"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Special)
	HashCase("BP_Mirado_Open_05_C"h, "Mirado"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Special)
	HashCase("BP_Motorglider_C"h, "Glider"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Special)
	HashCase("BP_Motorglider_Green_C"h, "Glider"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Special)
	HashCase("BP_LootTruck_C"h, "LootTruck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Special)

	HashCase("AquaRail_A_01_C"h, "Aquarail"eg, VehicleType1::Floating, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("AquaRail_A_02_C"h, "Aquarail"eg, VehicleType1::Floating, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("AquaRail_A_03_C"h, "Aquarail"eg, VehicleType1::Floating, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Boat_PG117_C"h, "Boat"eg, VehicleType1::Floating, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("PG117_A_01_C"h, "Boat"eg, VehicleType1::Floating, VehicleType2::Destructible, VehicleType3::Normal)

	HashCase("BP_M_Rony_A_01_C"h, "Rony"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_M_Rony_A_02_C"h, "Rony"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_M_Rony_A_03_C"h, "Rony"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Mirado_A_02_C"h, "Mirado"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Mirado_A_03_Esports_C"h, "Mirado"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Mirado_Open_03_C"h, "Mirado"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Mirado_Open_04_C"h, "Mirado"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Motorbike_04_C"h, "Motor"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Motorbike_04_Desert_C"h, "Motor"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Motorbike_Solitario_C"h, "Motor"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Motorbike_04_SideCar_C"h, "MotorS"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Motorbike_04_SideCar_Desert_C"h, "MotorS"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_01_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_02_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_03_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_04_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_05_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_06_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_07_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Niva_Esports_C"h, "Zima"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_A_01_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_A_02_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_A_03_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_A_04_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_A_05_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_A_esports_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_B_01_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_B_02_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_B_03_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_B_04_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PickupTruck_B_05_C"h, "Truck"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_TukTukTuk_A_01_C"h, "Tukshai"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_TukTukTuk_A_02_C"h, "Tukshai"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_TukTukTuk_A_03_C"h, "Tukshai"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Van_A_01_C"h, "Van"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Van_A_02_C"h, "Van"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Van_A_03_C"h, "Van"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Scooter_01_A_C"h, "Scooter"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Scooter_02_A_C"h, "Scooter"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Scooter_03_A_C"h, "Scooter"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Scooter_04_A_C"h, "Scooter"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Snowbike_01_C"h, "Snowmobile"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Snowbike_02_C"h, "Snowmobile"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Snowmobile_01_C"h, "Snowmobile"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Snowmobile_02_C"h, "Snowmobile"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Snowmobile_03_C"h, "Snowmobile"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Buggy_A_01_C"h, "Buggy"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Buggy_A_02_C"h, "Buggy"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Buggy_A_03_C"h, "Buggy"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Buggy_A_04_C"h, "Buggy"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Buggy_A_05_C"h, "Buggy"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Buggy_A_06_C"h, "Buggy"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Dacia_A_01_v2_C"h, "Dacia"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Dacia_A_01_v2_snow_C"h, "Dacia"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Dacia_A_02_v2_C"h, "Dacia"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Dacia_A_03_v2_C"h, "Dacia"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Dacia_A_03_v2_Esports_C"h, "Dacia"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Dacia_A_04_v2_C"h, "Dacia"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Uaz_A_01_C"h, "UAZ"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Uaz_Armored_C"h, "UAZ"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Uaz_B_01_C"h, "UAZ"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Uaz_B_01_esports_C"h, "UAZ"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("Uaz_C_01_C"h, "UAZ"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_MiniBus_C"h, "Bus"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Dirtbike_C"h, "DirtBike"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_CoupeRB_C"h, "Coupe"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_ATV_C"h, "Quad"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_PonyCoupe_C"h, "Pony"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashCase("BP_Porter_C"h, "Porter"eg, VehicleType1::Wheeled, VehicleType2::Destructible, VehicleType3::Normal)
	HashDefault(""eg, VehicleType1::None, VehicleType2::Destructible, VehicleType3::Normal)
)
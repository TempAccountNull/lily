#include "hack.h"
#include <map>

#include "GNames.h"
#include "GObjects.h"

#include "info_bone.h"
#include "info_item.h"
#include "info_vehicle.h"
#include "info_proj.h"
#include "info_package.h"
#include "info_character.h"

void Hack::Loop() {
	const HWND hGameWnd = pubg.GetHwnd();
	const CR3 mapCR3 = kernel.GetMapCR3();
	const TNameEntryArray NameArr;
	//FUObjectArray ObjectArr;
	//NameArr.DumpAllNames();
	//ObjectArr.DumpObject(NameArr);

	//f3 0f 11 ? ? ? ? ? ? f3 0f 11 ? ? ? ? ? ? f3 44 0f ? ? ? ? ? ? ? f3 0f 10 ? ? ? ? ? e9
	constexpr uintptr_t HookBaseAddress = 0x1192348;
	uint8_t OriginalByte = 0;
	pubg.ReadBase(HookBaseAddress, &OriginalByte);
	const uintptr_t AimHookAddressVA = pubg.GetBaseAddress() + HookBaseAddress;
	const PhysicalAddress AimHookAddressPA = dbvm.GetPhysicalAddress(AimHookAddressVA, mapCR3);
	verify(AimHookAddressPA && OriginalByte);

	NativePtr<ATslCharacter> CachedMyTslCharacterPtr = 0;
	NativePtr<ATslCharacter> LockTargetPtr = 0;

	struct CharacterInfo {
		struct PosInfo {
			uint64_t Time = 0;
			FVector Pos;
		};
		std::vector<PosInfo> Info;
		FVector Velocity;
	};

	std::map<uint64_t, CharacterInfo> CharacterMap;

	while (IsWindow(hGameWnd)) {
		const HWND hForeWnd = GetForegroundWindow();
		if (hGameWnd == hForeWnd)
			ProcessHotkey();

		NoticeTimeRemain = std::clamp(NoticeTimeRemain - render.TimeDelta, 0.0f, NOTICE_TIME);

		auto FuncInRenderArea = [&]() {
			ProcessImGui();
			DrawHotkey();
			DrawFPS(render.FPS, Render::COLOR_TEAL);
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			bool IsNeedToHookAim = false;
			TArray<NativePtr<AActor>> Actors;
			FVector CameraLocation;
			FRotator CameraRotation;
			FMatrix CameraRotationMatrix;
			float CameraFOV;
			float DefaultFOV = 0.0f;

			auto WorldToScreen = [&](const FVector& WorldLocation) {
				return this->WorldToScreen(WorldLocation, CameraRotationMatrix, CameraLocation, CameraFOV);
			};
			auto DrawRatioBox = [&](const FVector& ScreenPos, float CameraDistance, float BarLength3D, float Ratio, ImColor ColorRemain, ImColor ColorDamaged, ImColor ColorEdge) {
				FVector ZeroLocation;
				FMatrix ZeroRotationMatrix = FRotator().GetMatrix();
				FVector ScreenPos1 = this->WorldToScreen({ CameraDistance, BarLength3D / 2.0f, 0.0f }, ZeroRotationMatrix, ZeroLocation, CameraFOV);
				FVector ScreenPos2 = this->WorldToScreen({ CameraDistance, -BarLength3D / 2.0f, 0.0f }, ZeroRotationMatrix, ZeroLocation, CameraFOV);

				float ScreenLengthX = std::clamp(ScreenPos1.X - ScreenPos2.X, render.Width / 64.0f, render.Width / 16.0f);
				float ScreenLengthY = std::clamp(ScreenLengthX / 6.0f, 5.0f, 6.0f);

				render.DrawRatioBox(
					{ ScreenPos.X - ScreenLengthX / 2.0f, ScreenPos.Y - ScreenLengthY / 2.0f, ScreenPos.Z },
					{ ScreenPos.X + ScreenLengthX / 2.0f, ScreenPos.Y + ScreenLengthY / 2.0f, ScreenPos.Z },
					Ratio, ColorRemain, ColorDamaged, ColorEdge);

				return ImVec2{ ScreenLengthX, ScreenLengthY };
			};
			auto DrawItem = [&](NativePtr<UItem> ItemPtr, FVector ItemLocation) {
				UItem Item;
				if (!ItemPtr.Read(Item))
					return false;

				const unsigned ItemHash = NameArr.GetNameHashByID(Item.GetItemID());
				if (!ItemHash)
					return false;

				const auto ItemInfo = GetItemInfo(ItemHash);
				const char* szItemName = ItemInfo.ItemName.data();
				const int ItemPriority = ItemInfo.ItemPriority;

				if (ItemPriority < nItem && !bDebug)
					return false;

				const ImColor ItemColor = [&] {
					if (ItemPriority < nItem)
						return Render::COLOR_WHITE;
					switch (ItemPriority) {
					case 1: return Render::COLOR_YELLOW;
					case 2: return Render::COLOR_ORANGE;
					case 3: return Render::COLOR_PURPLE;
					case 4: return Render::COLOR_TEAL;
					case 5: return Render::COLOR_BLACK;
					default:return Render::COLOR_WHITE;
					}
				}();

				DrawString(ItemLocation, szItemName, ItemColor, false);
				return true;
			};

			constexpr float BallisticDragScale = 1.0f;
			constexpr float BallisticDropScale = 1.0f;

			NativePtr<UObject> MyPawnPtr = 0;
			FVector MyLocation;
			FVector GunLocation;
			FRotator GunRotation;

			[&] {
				UWorld World;
				if (!UWorld::GetUWorld(World))
					return;

				ULevel Level;
				if (!World.CurrentLevel.Read(Level))
					return;

				if (!Level.Actors.Read(Actors))
					return;

				UGameInstance GameInstance;
				if (!World.GameInstance.Read(GameInstance))
					return;

				EncryptedPtr<ULocalPlayer> LocalPlayerPtr;
				if (!GameInstance.LocalPlayers.GetValue(0, LocalPlayerPtr))
					return;

				ULocalPlayer LocalPlayer;
				if (!LocalPlayerPtr.Read(LocalPlayer))
					return;

				ATslPlayerController PlayerController;
				if (!LocalPlayer.PlayerController.ReadOtherType(PlayerController))
					return;

				DefaultFOV = PlayerController.DefaultFOV;

				if (PlayerController.Character)
					MyPawnPtr = (uintptr_t)PlayerController.Character;
				else if (CachedMyTslCharacterPtr)
					MyPawnPtr = (uintptr_t)CachedMyTslCharacterPtr;
				else if (PlayerController.SpectatorPawn)
					MyPawnPtr = (uintptr_t)PlayerController.SpectatorPawn;
				else
					MyPawnPtr = (uintptr_t)PlayerController.Pawn;
				CachedMyTslCharacterPtr = 0;

				APlayerCameraManager PlayerCameraManager;
				if (!PlayerController.PlayerCameraManager.Read(PlayerCameraManager))
					return;

				GunLocation = MyLocation = CameraLocation = PlayerCameraManager.CameraCache_POV_Location;
				GunRotation = CameraRotation = PlayerCameraManager.CameraCache_POV_Rotation;
				CameraRotationMatrix = PlayerCameraManager.CameraCache_POV_Rotation.GetMatrix();
				CameraFOV = PlayerCameraManager.CameraCache_POV_FOV;
			}();
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (IsNearlyZero(CameraFOV))
				return;
			if (IsNearlyZero(DefaultFOV))
				DefaultFOV = 90.0f;

			const float FOVRatio = DefaultFOV / CameraFOV;
			const float CircleFov = ConvertToRadians(CircleFovInDegrees);
			const float AimbotCircleSize =
				tanf(CircleFov) * render.Height *
				powf(1.6f, log2f(FOVRatio)) *
				(nAimbot >= 2 ? 1.25f : 1.0f);

			float AimbotDistant = AimbotCircleSize;
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			const unsigned MyCharacterNameHash = NameArr.GetNameHashByObject(MyPawnPtr);
			if (!MyCharacterNameHash)
				return;

			int MyTeamNum = -1;
			int SpectatedCount = 0;
			bool IsWeaponed = false;
			bool IsScoping = false;

			float ZeroingDistance = FLT_MIN;
			NativePtr<UCurveVector> BallisticCurve = 0;
			float Gravity = -9.8f;
			float BDS = 1.0f;
			float VDragCoefficient = 1.0f;
			float SimulationSubstepTime = 0.016f;
			float InitialSpeed = 800.0f;
			float BulletDropAdd = 7.0f;

			//MyCharacterInfo
			[&] {
				if (!IsPlayerCharacter(MyCharacterNameHash))
					return;

				ATslCharacter MyTslCharacter;
				if (!MyPawnPtr.ReadOtherType(MyTslCharacter))
					return;

				CachedMyTslCharacterPtr = (uintptr_t)MyPawnPtr;
				MyTeamNum = MyTslCharacter.LastTeamNum;
				SpectatedCount = MyTslCharacter.SpectatedCount;

				if (MyTslCharacter.Health <= 0.0f)
					return;

				//MyCharacter Mesh Info
				[&] {
					USkeletalMeshComponent Mesh;
					if (!MyTslCharacter.Mesh.Read(Mesh))
						return;

					GunLocation = MyLocation = Mesh.ComponentToWorld.Translation;

					UTslAnimInstance TslAnimInstance;
					if (!Mesh.AnimScriptInstance.ReadOtherType(TslAnimInstance))
						return;

					IsScoping = TslAnimInstance.bIsScoping_CP;
					FRotator Recoil_CP = TslAnimInstance.RecoilADSRotation_CP;
					Recoil_CP.Yaw += (TslAnimInstance.LeanRightAlpha_CP - TslAnimInstance.LeanLeftAlpha_CP) * Recoil_CP.Pitch / 3.0f;
					GunRotation = TslAnimInstance.ControlRotation_CP + Recoil_CP;
				}();

				//MyCharacter Weapon Info
				[&] {
					ATslWeapon_Trajectory TslWeapon;
					if (!MyTslCharacter.GetTslWeapon(TslWeapon))
						return;

					IsWeaponed = true;
					Gravity = TslWeapon.TrajectoryGravityZ;
					if (IsScoping)
						ZeroingDistance = TslWeapon.GetZeroingDistance();

					UWeaponTrajectoryData WeaponTrajectoryData;
					if (TslWeapon.WeaponTrajectoryData.Read(WeaponTrajectoryData)) {
						BDS = WeaponTrajectoryData.TrajectoryConfig.BDS;
						VDragCoefficient = WeaponTrajectoryData.TrajectoryConfig.VDragCoefficient;
						SimulationSubstepTime = WeaponTrajectoryData.TrajectoryConfig.SimulationSubstepTime;
						BallisticCurve = WeaponTrajectoryData.TrajectoryConfig.BallisticCurve;
						InitialSpeed = WeaponTrajectoryData.TrajectoryConfig.InitialSpeed;
					}

					UWeaponMeshComponent WeaponMesh;
					if (TslWeapon.Mesh3P.Read(WeaponMesh)) {
						FTransform GunTransform = WeaponMesh.GetSocketTransform(TslWeapon.FiringAttachPoint, RTS_World);

						GunLocation = GunTransform.Translation;
						if (IsScoping)
							GunRotation = GunTransform.Rotation;

						BulletDropAdd = WeaponMesh.GetScopingAttachPointRelativeZ(TslWeapon.ScopingAttachPoint) -
							GunTransform.GetRelativeTransform(WeaponMesh.ComponentToWorld).Translation.Z;
					}
				}();
			}();

			if (!CachedMyTslCharacterPtr)
				CharacterMap.clear();
			
			auto IsLockTargetValid = [&] {
				if (!CachedMyTslCharacterPtr)
					return false;

				if (!bPushingMouseM)
					return false;

				ATslCharacter TslCharacter;
				if (!LockTargetPtr.Read(TslCharacter))
					return false;

				if (TslCharacter.Health > 0.0f)
					return true;

				if (TslCharacter.GroggyHealth <= 0.0f || !bPushingCTRL)
					return false;

				return true;
			};
			if (!IsLockTargetValid())
				LockTargetPtr = 0;

			NativePtr<ATslCharacter> CurrentTargetPtr = 0;
			bool bFoundTarget = false;
			FVector TargetPos;

			//Actor loop
			for (const auto& ActorPtr : Actors.GetVector()) {
				TSet<NativePtr<UActorComponent>> OwnedComponents;
				FVector ActorVelocity;
				FVector ActorLocation;
				FVector ActorLocationScreen;
				float DistanceToActor = 0.0f;
				unsigned ActorNameHash = 0;

				auto GetValidActorInfo = [&]() {
					AActor Actor;
					if (!ActorPtr.Read(Actor))
						return false;

					OwnedComponents = Actor.OwnedComponents;

					USceneComponent RootComponent;
					if (!Actor.RootComponent.Read(RootComponent))
						return false;

					ActorVelocity = RootComponent.ComponentVelocity;
					ActorLocation = RootComponent.ComponentToWorld.Translation;
					ActorLocationScreen = WorldToScreen(ActorLocation);
					DistanceToActor = MyLocation.Distance(ActorLocation) / 100.0f;

					if (nRange != 1000 && DistanceToActor > nRange)
						return false;

					USceneComponent AttachParent;
					//LocalPawn is vehicle(Localplayer is in vehicle)
					if (RootComponent.AttachParent.Read(AttachParent) && AttachParent.Owner == MyPawnPtr)
						return false;

					if (!NameArr.GetNameByID(Actor.GetFName(), szBuf, sizeof(szBuf)))
						return false;

					ActorNameHash = CompileTime::StrHash(szBuf);

					if (bDebug) {
						FVector v2_DebugLoc = ActorLocationScreen;
						v2_DebugLoc.Y += 15.0;
						DrawString(v2_DebugLoc, szBuf, Render::COLOR_WHITE, false);
					}

					return true;
				};
				if (!GetValidActorInfo())
					continue;

				//DrawProj
				[&] {
					if (DistanceToActor > 300.0f)
						return;

					const char* szProjName = GetProjName(ActorNameHash).data();
					if (!*szProjName)
						return;

					sprintf(szBuf, "%s\n%.0fM"e, szProjName, DistanceToActor);
					DrawString(ActorLocationScreen, szBuf, Render::COLOR_RED, false);
				}();

				//DrawVehicle
				[&] {
					if (!bVehicle || bFighterMode)
						return;

					if (ActorNameHash == "BP_VehicleDrop_BRDM_C"h) {
						sprintf(szBuf, "BRDM\n%.0fM"e, DistanceToActor);
						DrawString(ActorLocationScreen, szBuf, Render::COLOR_TEAL, false);
						return;
					}

					auto VehicleInfo = GetVehicleInfo(ActorNameHash);
					const char* szVehicleName = VehicleInfo.VehicleName.data();
					if (!*szVehicleName)
						return;

					float Health = 100.0f;
					float HealthMax = 100.0f;
					float Fuel = 100.0f;
					float FuelMax = 100.0f;

					switch (VehicleInfo.Type1) {
					case VehicleType1::Wheeled:
					{
						ATslWheeledVehicle WheeledVehicle;
						if (!ActorPtr.ReadOtherType(WheeledVehicle))
							break;

						UTslVehicleCommonComponent VehicleComponent;
						if (!WheeledVehicle.VehicleCommonComponent.Read(VehicleComponent))
							break;

						Health = VehicleComponent.Health;
						HealthMax = VehicleComponent.HealthMax;
						Fuel = VehicleComponent.Fuel;
						FuelMax = VehicleComponent.FuelMax;
						break;
					}
					case VehicleType1::Floating:
					{
						ATslFloatingVehicle WheeledVehicle;
						if (!ActorPtr.ReadOtherType(WheeledVehicle))
							break;

						UTslVehicleCommonComponent VehicleComponent;
						if (!WheeledVehicle.VehicleCommonComponent.Read(VehicleComponent))
							break;

						Health = VehicleComponent.Health;
						HealthMax = VehicleComponent.HealthMax;
						Fuel = VehicleComponent.Fuel;
						FuelMax = VehicleComponent.FuelMax;
						break;
					}
					}

					if (ActorNameHash == "BP_LootTruck_C"h && Health <= 0.0f)
						return;

					bool IsDestructible = (VehicleInfo.Type2 == VehicleType2::Destructible);

					ImColor Color = Render::COLOR_BLUE;
					if (VehicleInfo.Type3 == VehicleType3::Special)
						Color = Render::COLOR_TEAL;
					if (Health <= 0.0f || Fuel <= 0.0f)
						Color = Render::COLOR_GRAY;

					sprintf(szBuf, "%s\n%.0fM"e, szVehicleName, DistanceToActor);
					DrawString(ActorLocationScreen, szBuf, Color, false);

					//Draw vehicle health, fuel
					[&] {
						if (!IsDestructible)
							return;

						FVector VehicleBarScreenPos = ActorLocationScreen;
						VehicleBarScreenPos.Y += render.GetTextSize(FONTSIZE, szBuf).y / 2.0f + 4.0f;
						const float CameraDistance = CameraLocation.Distance(ActorLocation) / 100.0f;
						if (Health <= 0.0f)
							return;

						const float HealthBarScreenLengthY = DrawRatioBox(VehicleBarScreenPos, CameraDistance, 1.0f,
							Health / HealthMax, Render::COLOR_GREEN, Render::COLOR_RED, Render::COLOR_BLACK).y;
						VehicleBarScreenPos.Y += HealthBarScreenLengthY - 1.0f;
						DrawRatioBox(VehicleBarScreenPos, CameraDistance, 1.0f,
							Fuel / FuelMax, Render::COLOR_BLUE, Render::COLOR_GRAY, Render::COLOR_BLACK).y;
					}();
				}();

				//DrawBox
				[&] {
					if (!bBox || bFighterMode)
						return;

					const char* szPackageName = GetPackageName(ActorNameHash).data();
					if (!*szPackageName)
						return;

					sprintf(szBuf, "%s\n%.0fM"e, szPackageName, DistanceToActor);
					DrawString(ActorLocationScreen, szBuf, Render::COLOR_TEAL, false);

					//DrawBoxContents
					[&] {
						if (!bPushingMouseM || nItem == 0)
							return;

						AItemPackage ItemPackage;
						if (!ActorPtr.ReadOtherType(ItemPackage))
							return;

						FVector PackageLocationScreen = ActorLocationScreen;

						const float TextLineHeight = render.GetTextSize(FONTSIZE, ""e).y;
						PackageLocationScreen.Y += TextLineHeight * 1.5f;

						for (const auto& ItemPtr : ItemPackage.Items.GetVector()) {
							if (!DrawItem(ItemPtr, PackageLocationScreen))
								continue;
							PackageLocationScreen.Y += TextLineHeight - 1.0f;
						}
					}();
				}();

				//DrawDropptedItem
				[&] {
					if (nItem == 0 || bFighterMode || ActorNameHash != "DroppedItem"h)
						return;

					ADroppedItem DroppedItem;
					if (!ActorPtr.ReadOtherType(DroppedItem))
						return;

					DrawItem((uintptr_t)DroppedItem.Item, ActorLocationScreen);
				}();

				//DrawDroppedItemGroup
				[&] {
					if (nItem == 0 || bFighterMode || ActorNameHash != "DroppedItemGroup"h)
						return;

					for (const auto& Element : OwnedComponents.GetVector()) {
						UDroppedItemInteractionComponent ItemComponent;
						if (!Element.Value.ReadOtherType(ItemComponent))
							continue;

						unsigned ItemComponentHash = NameArr.GetNameHashByID(ItemComponent.GetFName());
						if (!ItemComponentHash)
							continue;

						if (ItemComponentHash != "DroppedItemInteractionComponent"h && ItemComponentHash != "DestructibleItemInteractionComponent"h)
							continue;

						FVector ItemLocationScreen = WorldToScreen(ItemComponent.ComponentToWorld.Translation);
						DrawItem(ItemComponent.Item, ItemLocationScreen);
					}
				}();

				//Character
				[&] {
					if (!IsPlayerCharacter(ActorNameHash) && !IsAICharacter(ActorNameHash))
						return;

					if (MyPawnPtr == ActorPtr)
						return;

					ATslCharacter TslCharacter;
					if (!ActorPtr.ReadOtherType(TslCharacter))
						return;

					const float Health = TslCharacter.Health;
					const float HealthMax = TslCharacter.HealthMax;
					const float GroggyHealth = TslCharacter.GroggyHealth;
					const float GroggyHealthMax = TslCharacter.GroggyHealthMax;
					const bool IsPlayerDead = (Health <= 0.0f && GroggyHealth <= 0.0f);

					int NumKills = 0;
					float DamageDealtOnEnemy = 0.0f;
					bool IsInVehicle = false;

					//GetPlayerState
					[&] {
						if (!TslCharacter.PlayerState)
							return;

						ATslPlayerState TslPlayerState;
						if (!TslCharacter.PlayerState.ReadOtherType(TslPlayerState))
							return;

						NumKills = TslPlayerState.PlayerStatistics_NumKills;
						DamageDealtOnEnemy = TslPlayerState.DamageDealtOnEnemy;
					}();

					//GetPlayerVehicleInfo
					[&] {
						UVehicleRiderComponent VehicleRiderComponent;
						if (!TslCharacter.VehicleRiderComponent.Read(VehicleRiderComponent))
							return;

						if (VehicleRiderComponent.SeatIndex == -1)
							return;

						APawn LastVehiclePawn;
						if (!VehicleRiderComponent.LastVehiclePawn.Read(LastVehiclePawn))
							return;

						IsInVehicle = true;
						ActorVelocity = LastVehiclePawn.ReplicatedMovement.LinearVelocity;

						ATslPlayerState TslPlayerState;
						if (!LastVehiclePawn.PlayerState.ReadOtherType(TslPlayerState))
							return;

						NumKills = TslPlayerState.PlayerStatistics_NumKills;
						DamageDealtOnEnemy = TslPlayerState.DamageDealtOnEnemy;
					}();

					//DrawRadar
					[&] {
						if (IsPlayerDead)
							return;

						if (TslCharacter.LastTeamNum == MyTeamNum)
							return;

						FVector ALfromME = ActorLocation - MyLocation;
						int RadarX = (int)round(ALfromME.X / 100);
						int RadarY = (int)round(ALfromME.Y / 100);

						//when ememy in the radar radius.
						if (RadarX > 200 || RadarX < -200 || RadarY > 200 || RadarY < -200)
							return;

						FVector v;
						v.X = (render.Width * (0.9807f + 0.8474f) / 2.0f) + (RadarX / 200.0f * render.Width * (0.9807f - 0.8474f) / 2.0f);
						v.Y = (render.Height * (0.9722f + 0.7361f) / 2.0f) + (RadarY / 200.0f * render.Height * (0.9722f - 0.7361f) / 2.0f);

						ImColor Color = Render::COLOR_RED;

						if (IsInVehicle)
							Color = Render::COLOR_BLUE;
						if (GroggyHealth > 0.0f && Health <= 0.0f)
							Color = Render::COLOR_GRAY;

						render.DrawCircleFilled(v, 4.0f, Color);
					}();

					//Character Mesh stuff
					[&] {
						USkeletalMeshComponent Mesh;
						if (!TslCharacter.Mesh.Read(Mesh))
							return;

						//GetVelocity
						[&] {
							CharacterMap[ActorPtr].Velocity = ActorVelocity;
							auto& Info = CharacterMap[ActorPtr].Info;

							if (IsPlayerDead || !Mesh.IsVisible()) {
								Info.clear();
								return;
							}

							Info.push_back({ render.TimeInMicroSeconds, Mesh.ComponentToWorld.Translation });

							float SumTimeDelta = 0.0f;
							FVector SumTargetPosDif;
							for (size_t i = 1; i < Info.size(); i++) {
								const float DeltaTime = (Info[i].Time - Info[i - 1].Time) / 1000000.0f;
								if (DeltaTime > 0.5f) {
									Info.clear();
									return;
								}

								const FVector DeltaPos = Info[i].Pos - Info[i - 1].Pos;
								if (DeltaPos.Length() / 100.0f > 1.0f) {
									Info.clear();
									return;
								}

								SumTimeDelta = SumTimeDelta + DeltaTime;
								SumTargetPosDif = SumTargetPosDif + DeltaPos;
							}

							if (SumTimeDelta < 0.1f)
								return;

							if (SumTimeDelta > 0.15f)
								Info.erase(Info.begin());

							CharacterMap[ActorPtr].Velocity = SumTargetPosDif * (1.0f / SumTimeDelta);
						}();

						if (IsPlayerDead)
							return;

						std::map<int, FVector> BonesPos, BonesScreenPos;

						//GetBonesPosition
						[&] {
							auto BoneSpaceTransforms = Mesh.BoneSpaceTransforms.GetVector();
							size_t BoneSpaceTransformsSize = BoneSpaceTransforms.size();
							if (!BoneSpaceTransformsSize)
								return;

							for (auto BoneIndex : GetBoneIndexArray()) {
								verify(BoneIndex < BoneSpaceTransformsSize);
								FVector Pos = (BoneSpaceTransforms[BoneIndex] * Mesh.ComponentToWorld).Translation;
								BonesPos[BoneIndex] = Pos;
								BonesScreenPos[BoneIndex] = WorldToScreen(Pos);
							}
						}();

						bool IsInCircle = false;
						//Aimbot
						[&] {
							if (!IsWeaponed)
								return;
							if (!Mesh.IsVisible())
								return;
							if (!bTeamKill && TslCharacter.LastTeamNum == MyTeamNum)
								return;
							if (!bPushingCTRL && Health <= 0.0f)
								return;

							FVector VecEnemy = (BonesPos[neck_01] + BonesPos[spine_02]) * 0.5f;
							if (bPushingShift)
								VecEnemy = BonesPos[forehead];

							FVector VecEnemy2D = WorldToScreen(VecEnemy);
							if (VecEnemy2D.Z < 0.0f)
								return;

							VecEnemy2D.Z = 0.0f;
							FVector Center2D = { render.Width / 2.0f, render.Height / 2.0f, 0 };

							float DistanceFromCenter = Center2D.Distance(VecEnemy2D);

							if (DistanceFromCenter < AimbotCircleSize)
								IsInCircle = true;

							if (DistanceFromCenter > AimbotDistant)
								return;

							if (LockTargetPtr && LockTargetPtr != ActorPtr)
								return;

							bFoundTarget = true;
							CurrentTargetPtr = (uintptr_t)ActorPtr;
							AimbotDistant = DistanceFromCenter;
							TargetPos = VecEnemy;
						}();

						//DrawCharacter
						[&] {
							//GetColor
							ImColor Color = [&] {
								if (ActorPtr == LockTargetPtr)
									return Render::COLOR_PURPLE;
								if (TslCharacter.LastTeamNum == MyTeamNum)
									return Render::COLOR_GREEN;
								if (IsInCircle)
									return Render::COLOR_YELLOW;
								if (IsInVehicle)
									return Render::COLOR_BLUE;
								if (Health <= 0.0f)
									return Render::COLOR_GRAY;
								if (IsAICharacter(ActorNameHash))
									return Render::COLOR_TEAL;
								return (DamageDealtOnEnemy > 510.0) ? Render::COLOR_RED : IM_COL32(255, 255 - int(DamageDealtOnEnemy / 2), 255 - int(DamageDealtOnEnemy / 2), 255);
							}();

							//Draw Skeleton
							for (auto DrawPair : GetDrawPairArray())
								render.DrawLine(BonesScreenPos[DrawPair.first], BonesScreenPos[DrawPair.second], Color);

							//Draw HealthBar
							FVector HealthBarPos = BonesPos[neck_01];
							HealthBarPos.Z += 35.0f;
							FVector HealthBarScreenPos = WorldToScreen(HealthBarPos);
							HealthBarScreenPos.Y -= 5.0f;

							float HealthBarScreenLengthY = 0.0f;
							const float CameraDistance = CameraLocation.Distance(HealthBarPos) / 100.0f;
							if (Health > 0.0)
								HealthBarScreenLengthY = DrawRatioBox(HealthBarScreenPos, CameraDistance, 0.7f,
									Health / HealthMax, Render::COLOR_GREEN, Render::COLOR_RED, Render::COLOR_BLACK).y;
							else if (GroggyHealth > 0.0)
								HealthBarScreenLengthY = DrawRatioBox(HealthBarScreenPos, CameraDistance, 0.7f,
									GroggyHealth / GroggyHealthMax, Render::COLOR_RED, Render::COLOR_GRAY, Render::COLOR_BLACK).y;

							//Draw CharacterInfo
							wchar_t PlayerName[0x100];
							TslCharacter.CharacterName.GetValues(*PlayerName, 0x100);

							if (TslCharacter.LastTeamNum >= 200 || TslCharacter.LastTeamNum == MyTeamNum)
								sprintf(szBuf, "%ws %d\n%.0fM %.0f"e, PlayerName, NumKills, DistanceToActor, DamageDealtOnEnemy);
							else
								sprintf(szBuf, "%ws %d\n%.0fM %d %.0f"e, PlayerName, NumKills, DistanceToActor, TslCharacter.LastTeamNum, DamageDealtOnEnemy);

							DrawString(
								{ HealthBarScreenPos.X, HealthBarScreenPos.Y - HealthBarScreenLengthY - render.GetTextSize(FONTSIZE, szBuf).y / 2.0f, HealthBarScreenPos.Z },
								szBuf, Color, true);
						}();
					}();
				}();
			}

			float CustomTimeDilation = 1.0f;

			//TargetCharacter
			[&] {
				if (!bFoundTarget)
					return;

				if (bPushingMouseM && !LockTargetPtr)
					LockTargetPtr = CurrentTargetPtr;

				if (nAimbot == 2 || nAimbot == 3 || !IsScoping)
					ZeroingDistance = FLT_MIN;

				auto Result = GetBulletDropAndTravelTime(
					GunLocation, GunRotation, TargetPos, ZeroingDistance, BulletDropAdd, InitialSpeed,
					Gravity, BallisticDragScale, BallisticDropScale, BDS, SimulationSubstepTime, VDragCoefficient, BallisticCurve);

				float BulletDrop = Result.first;
				float TravelTime = Result.second;

				FVector PredictedPos = TargetPos;
				PredictedPos.Z += BulletDrop;
				PredictedPos = PredictedPos + (CharacterMap[CurrentTargetPtr].Velocity * (TravelTime / CustomTimeDilation));

				FVector TargetScreenPos = WorldToScreen(TargetPos);
				FVector AimScreenPos = WorldToScreen(PredictedPos);

				const float LineLen = std::clamp(AimScreenPos.Y - WorldToScreen({ TargetPos.X, TargetPos.Y, TargetPos.Z + 10.0f }).Y, 4.0f, 8.0f);
				const float LineThickness = 2.0f;

				render.DrawLine(TargetScreenPos, AimScreenPos, Render::COLOR_RED, LineThickness);
				render.DrawLine(
					{ AimScreenPos.X - LineLen, AimScreenPos.Y - LineLen, AimScreenPos.Z },
					{ AimScreenPos.X + LineLen, AimScreenPos.Y + LineLen, AimScreenPos.Z }, Render::COLOR_RED, LineThickness);
				render.DrawLine(
					{ AimScreenPos.X + LineLen, AimScreenPos.Y - LineLen, AimScreenPos.Z },
					{ AimScreenPos.X - LineLen, AimScreenPos.Y + LineLen, AimScreenPos.Z }, Render::COLOR_RED, LineThickness);

				if (!IsWeaponed || !bPushingMouseM)
					return;

				auto AImbot_MouseMove = [&] {
					if (hGameWnd != hForeWnd)
						return;

					const FVector LocTarget = ::WorldToScreen(PredictedPos, CameraRotationMatrix, CameraLocation, CameraFOV, 1.0f, 1.0f);
					const float DistanceToTarget = CameraLocation.Distance(PredictedPos) / 100.0f;
					const FVector GunCenterPos = CameraLocation + GunRotation.GetUnitVector() * DistanceToTarget;
					const FVector LocCenter = ::WorldToScreen(GunCenterPos, CameraRotationMatrix, CameraLocation, CameraFOV, 1.0f, 1.0f);

					const float MouseX = RemainMouseX + AimbotSpeedX * (LocTarget.X - LocCenter.X) * render.TimeDelta * 100000.0f;
					const float MouseY = RemainMouseY + AimbotSpeedY * (LocTarget.Y - LocCenter.Y) * render.TimeDelta * 100000.0f;
					const float TruncedMouseX = truncf(MouseX);
					const float TruncedMouseY = truncf(MouseY);
					RemainMouseX = MouseX - TruncedMouseX;
					RemainMouseY = MouseY - TruncedMouseY;
					kernel.PostRawMouseInput(hGameWnd, { .usFlags = MOUSE_MOVE_RELATIVE,
						.lLastX = (int)TruncedMouseX, .lLastY = (int)TruncedMouseY });
				};

				switch (nAimbot) {
				case 1:
					AImbot_MouseMove();
					break;
				case 2:
					if (!IsScoping) {
						AImbot_MouseMove();
						break;
					}
				case 3:
					FVector DirectionInput = PredictedPos - GunLocation;
					DirectionInput.Normalize();

					ChangeRegOnBPInfo Info{};
					Info.changeXMM7_0 = true;
					Info.changeXMM6_0 = true;
					Info.changeXMM8_0 = true;
					Info.XMM7.Float_0 = DirectionInput.X;
					Info.XMM6.Float_0 = DirectionInput.Y;
					Info.XMM8.Float_0 = DirectionInput.Z;
					dbvm.ChangeRegisterOnBP(AimHookAddressPA, Info);
					IsNeedToHookAim = true;
					break;
				}
			}();

			if (!IsNeedToHookAim)
				dbvm.RemoveChangeRegisterOnBP(AimHookAddressPA);

			if (SpectatedCount > 0)
				DrawSpectatedCount(SpectatedCount, Render::COLOR_RED);

			if (!IsNearlyZero(ZeroingDistance))
				DrawZeroingDistance(ZeroingDistance, Render::COLOR_TEAL);

			if (IsWeaponed)
				render.DrawCircle({ render.Width / 2.0f, render.Height / 2.0f, 0.0f }, AimbotCircleSize, Render::COLOR_WHITE);
		};

		render.RenderArea(hGameWnd, Render::COLOR_CLEAR, [&] { 
			if (!ExceptionHandler::TryExcept(FuncInRenderArea))
				printlog("Error : %d"e, ExceptionHandler::GetLastExceptionCode());
			});
	}
}

void FUObjectArray::DumpObject(const TNameEntryArray& NameArr) const {
	for (unsigned i = 0; i < GetNumElements(); i++) {
		UObject obj;
		auto Ptr = GetNativePtrById(i);
		if (!Ptr.Read(obj))
			continue;

		char szName[0x200];
		if (!NameArr.GetNameByID(obj.GetFName(), szName, sizeof(szName)))
			continue;

		if (_stricmp(szName, "World"e) == 0) {
			dprintf("%I64X %s\n"e, (uintptr_t)Ptr, szName);
		}
		if (_stricmp(szName, "PlayerState"e) == 0) {
			dprintf("%I64X %s\n"e, (uintptr_t)Ptr, szName);
		}
		if (_stricmp(szName, "ForceLayoutPrepass"e) == 0) {
			dprintf("%I64X %s\n"e, (uintptr_t)Ptr, szName);
		}
	}
}
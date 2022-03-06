#include "hack.h"
#include <map>

#include "GNames.h"
#include "GObjects.h"

#include "info_bone.h"
#include "info_vehicle.h"
#include "info_proj.h"
#include "info_package.h"
#include "info_character.h"

enum class CharacterState {
	Dead,
	Alive,
	Groggy
};

void Hack::Loop() {
	UpdateRankInfo();
	LoadBlackList();

	const HWND hGameWnd = pubg.hGameWnd;
	const CR3 mapCR3 = kernel.GetMapCR3();
	//FUObjectArray ObjectArr;
	//ObjectArr.DumpObject(NameArr);

	const FName KeyMouseX = pubg.NameArr.FindName("MouseX"e);
	verify(KeyMouseX.ComparisonIndex);
	const FName KeyMouseY = pubg.NameArr.FindName("MouseY"e);
	verify(KeyMouseY.ComparisonIndex);

	//41 0f ? ? 73 ? f3 0f 10 ? ? ? ? ? f3 0f 11 ? ? ? ? 00 00
	constexpr uintptr_t HookBaseAddress = 0x3F1BB6;
	uint8_t OriginalByte = 0;
	pubg.ReadBase(HookBaseAddress, &OriginalByte);
	const uintptr_t AimHookAddressVA = pubg.GetBaseAddress() + HookBaseAddress;
	const PhysicalAddress AimHookAddressPA = dbvm.GetPhysicalAddress(AimHookAddressVA, mapCR3);
	verify(AimHookAddressPA && OriginalByte);

	float TimeDeltaAcc = 0.0f;
	float LastAimUpdateTime = 0.0f;
	float RemainMouseX = 0.0f;
	float RemainMouseY = 0.0f;
	NativePtr<ATslCharacter> CachedMyTslCharacterPtr = 0;
	NativePtr<ATslCharacter> LockTargetPtr = 0;
	NativePtr<ATslCharacter> EnemyFocusingMePtr = 0;
	bool bPushedCapsLock = false;
	bool IsFPPOnly = true;

	struct CharacterInfo {
		NativePtr<ATslCharacter> Ptr;
		int Team = -1;
		int SpectatedCount = 0;
		bool IsBlackListed = false;
		bool IsFPP = false;
		bool IsWeaponed = false;
		bool IsFiring = false;
		FRotator FiringRotation;
		bool IsScoping = false;
		bool IsVisible = false;
		bool IsAI = false;
		float Health = 0.0f;
		float GroggyHealth = 0.0f;
		CharacterState State = CharacterState::Dead;
		float ZeroingDistance = 100.0f;
		NativePtr<UCurveVector> BallisticCurve = 0;
		float Gravity = -9.8f;
		float BDS = 1.0f;
		float VDragCoefficient = 1.0f;
		float SimulationSubstepTime = 0.016f;
		float InitialSpeed = 800.0f;
		float BulletDropAdd = 7.0f;
		FVector Location;
		FVector AimPoint;
		bool IsLocked = false;
		FRotator Recoil;
		FVector GunLocation;
		FRotator GunRotation;
		FVector AimLocation;
		FRotator AimRotation;
		FVector Velocity;

		bool IsInVehicle = false;
		int NumKills = 0;
		float Damage = 0.0f;
		std::map<int, FVector> BonesPos, BonesScreenPos;
		std::string PlayerName;
		std::string WeaponName;
		int Ammo = -1;
	};

	struct tMapInfo {
		struct {
			struct PosInfo {
				uint64_t Time = 0;
				FVector Pos;
			};
			std::vector<PosInfo> Info;
		}PosInfo;

		struct {
			int Ammo = -1;
			float RemainTime = 0.0f;
			FRotator GunRotation;
		}FiringInfo;

		struct {
			bool IsLocked = false;
			FVector AimPoint;
			FVector Velocity;
			CharacterState State = CharacterState::Dead;
			uint64_t Time = 0;
			bool IsInVehicle = false;
		}AimbotInfo;

		float FocusTime = 0.0f;
	};

	std::map<uint64_t, tMapInfo> EnemyInfoMap;

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
			float TimeSeconds = 0.0f;
			bool IsNeedToHookAim = false;
			TArray<NativePtr<AActor>> Actors;
			FVector CameraLocation;
			FRotator CameraRotation;
			FMatrix CameraRotationMatrix;
			float CameraFOV = 0.0f;
			float DefaultFOV = 0.0f;
			float MouseXSensitivity = 0.2f;
			float MouseYSensitivity = 0.2f;

			CharacterInfo MyInfo;

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

				auto ItemInfo = Item.GetInfo();
				auto& ItemName = ItemInfo.Name;

				if (!ItemName[0] && !pubg.NameArr.GetNameByID(Item.GetItemID(), ItemName.data(), sizeof(ItemName)))
					return false;

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

				DrawString(ItemLocation, ItemName.data(), ItemColor, false);
				return true;
			};
			auto GetCharacterInfo = [&](NativePtr<ATslCharacter> CharacterPtr, CharacterInfo& Info) -> bool {
				const unsigned NameHash = pubg.NameArr.GetNameHashByObjectPtr(CharacterPtr);
				if (!IsPlayerCharacter(NameHash) && !IsAICharacter(NameHash))
					return false;

				ATslCharacter TslCharacter;
				if (!CharacterPtr.ReadOtherType(TslCharacter))
					return false;

				USkeletalMeshComponent Mesh;
				if (!TslCharacter.Mesh.Read(Mesh))
					return false;

				UTslAnimInstance TslAnimInstance;
				if (!Mesh.AnimScriptInstance.ReadOtherType(TslAnimInstance))
					return false;

				//Bones
				auto BoneSpaceTransforms = Mesh.BoneSpaceTransforms.GetVector();
				size_t BoneSpaceTransformsSize = BoneSpaceTransforms.size();
				if (!BoneSpaceTransformsSize)
					return false;

				for (auto BoneIndex : GetBoneIndexArray()) {
					verify(BoneIndex < BoneSpaceTransformsSize);
					FVector Pos = (BoneSpaceTransforms[BoneIndex] * Mesh.ComponentToWorld).Translation;
					Info.BonesPos[BoneIndex] = Pos;
					Info.BonesScreenPos[BoneIndex] = WorldToScreen(Pos);
				}

				Info.Location = Mesh.ComponentToWorld.Translation;
				Info.Velocity = Mesh.ComponentVelocity;
				Info.IsVisible = Mesh.IsVisible();

				Info.Ptr = CharacterPtr;
				Info.IsAI = IsAICharacter(NameHash);
				Info.Health = TslCharacter.Health / TslCharacter.HealthMax;
				Info.GroggyHealth = TslCharacter.GroggyHealth / TslCharacter.GroggyHealthMax;
				Info.Team = TslCharacter.LastTeamNum;
				Info.SpectatedCount = TslCharacter.SpectatedCount;
				Info.IsScoping = TslAnimInstance.bIsScoping_CP;
				Info.Recoil = TslAnimInstance.RecoilADSRotation_CP;
				Info.Recoil.Yaw += (TslAnimInstance.LeanRightAlpha_CP - TslAnimInstance.LeanLeftAlpha_CP) * Info.Recoil.Pitch / 3.0f;
				Info.IsFPP = TslAnimInstance.bLocalFPP_CP;
				Info.State =
					Info.Health > 0.0f ? CharacterState::Alive :
					Info.GroggyHealth > 0.0f ? CharacterState::Groggy :
					CharacterState::Dead;

				Info.AimPoint = bPushingShift ? Info.BonesPos[forehead] : (Info.BonesPos[neck_01] + Info.BonesPos[spine_02]) * 0.5f;

				wchar_t PlayerName[0x100];
				if (TslCharacter.CharacterName.GetValues(*PlayerName, 0x100))
					Info.PlayerName = ws2s(PlayerName);

				Info.IsBlackListed = IsUserBlackListed(Info.PlayerName.c_str());

				EnemyInfoMap[CharacterPtr].FiringInfo.RemainTime =
					std::clamp(EnemyInfoMap[CharacterPtr].FiringInfo.RemainTime - render.TimeDelta, 0.0f, 1.0f);

				//Velocity
				[&] {
					auto& PosInfo = EnemyInfoMap[CharacterPtr].PosInfo.Info;

					if (Info.State == CharacterState::Dead || !Info.IsVisible) {
						PosInfo.clear();
						return;
					}

					PosInfo.push_back({ render.TimeInMicroSeconds, Info.Location });

					float SumTimeDelta = 0.0f;
					FVector SumPosDif;
					for (size_t i = 1; i < PosInfo.size(); i++) {
						const float DeltaTime = (PosInfo[i].Time - PosInfo[i - 1].Time) / 1000000.0f;
						if (DeltaTime > 0.5f) {
							PosInfo.clear();
							return;
						}

						const FVector DeltaPos = PosInfo[i].Pos - PosInfo[i - 1].Pos;
						if (DeltaPos.Length() / 100.0f > 1.0f) {
							PosInfo.clear();
							return;
						}

						SumTimeDelta = SumTimeDelta + DeltaTime;
						SumPosDif = SumPosDif + DeltaPos;
					}

					if (SumTimeDelta < 0.1f)
						return;

					if (SumTimeDelta > 0.15f)
						PosInfo.erase(PosInfo.begin());

					Info.Velocity = SumPosDif * (1.0f / SumTimeDelta);
				}();

				//PlayerState
				[&] {
					if (!TslCharacter.PlayerState)
						return;

					ATslPlayerState TslPlayerState;
					if (!TslCharacter.PlayerState.ReadOtherType(TslPlayerState))
						return;

					Info.NumKills = TslPlayerState.PlayerStatistics_NumKills;
					Info.Damage = TslPlayerState.DamageDealtOnEnemy;
				}();

				//Vehicle
				[&] {
					UVehicleRiderComponent VehicleRiderComponent;
					if (!TslCharacter.VehicleRiderComponent.Read(VehicleRiderComponent))
						return;

					if (VehicleRiderComponent.SeatIndex == -1)
						return;

					APawn LastVehiclePawn;
					if (!VehicleRiderComponent.LastVehiclePawn.Read(LastVehiclePawn))
						return;

					Info.IsInVehicle = true;
					Info.Velocity = LastVehiclePawn.ReplicatedMovement.LinearVelocity;

					ATslPlayerState TslPlayerState;
					if (!LastVehiclePawn.PlayerState.ReadOtherType(TslPlayerState))
						return;

					Info.NumKills = TslPlayerState.PlayerStatistics_NumKills;
					Info.Damage = TslPlayerState.DamageDealtOnEnemy;
				}();

				//WeaponInfo
				[&] {
					ATslWeapon TslWeapon;
					if (!TslCharacter.GetTslWeapon(TslWeapon))
						return;

					Info.WeaponName = TslWeapon.GetWeaponName();
					if (Info.WeaponName.empty() && bDebug)
						Info.WeaponName = TslWeapon.GetName();

					Info.Ammo = [&] {
						ATslWeapon_Trajectory TslWeapon;
						if (!TslCharacter.GetTslWeapon_Trajectory(TslWeapon))
							return -1;
						if (HIBYTE(TslWeapon.CurrentAmmoData))
							return -1;
						return (int)TslWeapon.CurrentAmmoData;
					}();
				}();

				//Weapon
				[&] {
					ATslWeapon_Trajectory TslWeapon;
					if (!TslCharacter.GetTslWeapon_Trajectory(TslWeapon))
						return;

					Info.IsWeaponed = true;
					Info.Gravity = TslWeapon.TrajectoryGravityZ;
					if (Info.IsScoping)
						Info.ZeroingDistance = TslWeapon.GetZeroingDistance();

					UWeaponTrajectoryData WeaponTrajectoryData;
					if (TslWeapon.WeaponTrajectoryData.Read(WeaponTrajectoryData)) {
						Info.BDS = WeaponTrajectoryData.TrajectoryConfig.BDS;
						Info.VDragCoefficient = WeaponTrajectoryData.TrajectoryConfig.VDragCoefficient;
						Info.SimulationSubstepTime = WeaponTrajectoryData.TrajectoryConfig.SimulationSubstepTime;
						Info.BallisticCurve = WeaponTrajectoryData.TrajectoryConfig.BallisticCurve;
						Info.InitialSpeed = WeaponTrajectoryData.TrajectoryConfig.InitialSpeed;
					}

					UWeaponMeshComponent WeaponMesh;
					if (TslWeapon.Mesh3P.Read(WeaponMesh)) {
						FTransform GunTransform = WeaponMesh.GetSocketTransform(TslWeapon.FiringAttachPoint, RTS_World);
						Info.GunLocation = GunTransform.Translation;
						Info.GunRotation = GunTransform.Rotation;
						Info.BulletDropAdd = WeaponMesh.GetScopingAttachPointRelativeZ(TslWeapon.ScopingAttachPoint) -
							GunTransform.GetRelativeTransform(WeaponMesh.ComponentToWorld).Translation.Z;
					}
				}();

				//AimLocation, AimRotation
				//////////////////////////////////

				Info.AimLocation = Info.GunLocation.Length() > 0.0f ? Info.GunLocation : Info.Location;
				if (Info.IsScoping)
					Info.AimRotation = Info.GunRotation;
				else
					Info.AimRotation = TslAnimInstance.ControlRotation_CP + Info.Recoil;

				//FiringInfo
				//////////////////////////////////

				int PrevAmmo = EnemyInfoMap[CharacterPtr].FiringInfo.Ammo;
				if (Info.Ammo != -1 && PrevAmmo != -1 && Info.Ammo == PrevAmmo - 1) {
					EnemyInfoMap[CharacterPtr].FiringInfo.RemainTime = FiringTime;
					EnemyInfoMap[CharacterPtr].FiringInfo.GunRotation = Info.GunRotation;
				}

				Info.IsFiring = (EnemyInfoMap[CharacterPtr].FiringInfo.RemainTime > 0.0f);
				Info.FiringRotation = EnemyInfoMap[CharacterPtr].FiringInfo.GunRotation;

				EnemyInfoMap[CharacterPtr].FiringInfo.Ammo = Info.Ammo;

				//AimbotInfo
				//////////////////////////////////

				auto& AimbotInfo = EnemyInfoMap[CharacterPtr].AimbotInfo;

				if (CharacterPtr == LockTargetPtr) {
					if (AimbotInfo.IsLocked) {
						if (AimbotInfo.Time != render.TimeInMicroSeconds && AimbotInfo.IsInVehicle) {
							AimbotInfo.AimPoint = AimbotInfo.AimPoint + AimbotInfo.Velocity * render.TimeDelta;
							AimbotInfo.Time = render.TimeInMicroSeconds;
						}
						Info.AimPoint = AimbotInfo.AimPoint;
						Info.Velocity = AimbotInfo.Velocity;
						Info.IsVisible = true;
					}
					else {
						const bool bLockAimPoint = [&] {
							if (AimbotInfo.State == CharacterState::Alive && Info.State == CharacterState::Groggy)
								return true;
							if (AimbotInfo.State == CharacterState::Alive && Info.State == CharacterState::Dead)
								return true;
							if (AimbotInfo.State == CharacterState::Groggy && Info.State == CharacterState::Dead)
								return true;

							return false;
						}();

						if (bLockAimPoint) {
							AimbotInfo.AimPoint = Info.AimPoint;
							AimbotInfo.IsLocked = true;
						}
						else {
							AimbotInfo.Velocity = Info.Velocity;
							AimbotInfo.IsInVehicle = Info.IsInVehicle;
						}
					}
				}
				else
					AimbotInfo.IsLocked = false;

				Info.IsLocked = AimbotInfo.IsLocked;
				AimbotInfo.State = Info.State;

				return true;
			};

			constexpr float BallisticDragScale = 1.0f;
			constexpr float BallisticDropScale = 1.0f;

			NativePtr<UObject> MyPawnPtr = 0;

			[&] {
				UWorld World;
				if (!UWorld::GetUWorld(World))
					return;

				TimeSeconds = World.TimeSeconds;

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

				UPlayerInput PlayerInput;
				if (!PlayerController.PlayerInput.Read(PlayerInput))
					return;

				FInputAxisProperties InputAxisProperties;
				if (PlayerInput.AxisProperties.GetValue(KeyMouseX, InputAxisProperties))
					MouseXSensitivity = InputAxisProperties.Sensitivity;
				if (PlayerInput.AxisProperties.GetValue(KeyMouseY, InputAxisProperties))
					MouseYSensitivity = InputAxisProperties.Sensitivity;

				APlayerCameraManager PlayerCameraManager;
				if (!PlayerController.PlayerCameraManager.Read(PlayerCameraManager))
					return;

				MyInfo.Location = CameraLocation = PlayerCameraManager.CameraCache_POV_Location;
				CameraRotation = PlayerCameraManager.CameraCache_POV_Rotation;
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
			auto GetMouseXY = [&](FRotator RotationInput) {
				RotationInput.Clamp();
				return POINT{
					LONG(RotationInput.Yaw / MouseXSensitivity * 0.4f * FOVRatio),
					LONG(-RotationInput.Pitch / MouseYSensitivity * 0.4f * FOVRatio) };
			};

			const float AimbotCircleSize = tanf(ConvertToRadians(AimbotFOV)) * render.Height * powf(1.5f, log2f(FOVRatio));
			const float SilentCircleSize = tanf(ConvertToRadians(SilentFOV)) * render.Height * powf(1.5f, log2f(FOVRatio));
			float AimbotDistant = bAimbot ? AimbotCircleSize : SilentCircleSize;

			if (GetCharacterInfo((uintptr_t)MyPawnPtr, MyInfo))
				CachedMyTslCharacterPtr = (uintptr_t)MyPawnPtr;

			if (!MyInfo.IsFPP)
				IsFPPOnly = false;

			if (!CachedMyTslCharacterPtr) {
				EnemyInfoMap.clear();
				LockTargetPtr = 0;
				IsFPPOnly = true;
			}

			if (!bPushingMouseM)
				LockTargetPtr = 0;

			CharacterInfo LockedTargetInfo;
			if (!GetCharacterInfo(LockTargetPtr, LockedTargetInfo))
				LockTargetPtr = 0;

			NativePtr<ATslCharacter> CurrentTargetPtr = 0;

			auto ProcessTslCharacter = [&](uint64_t ActorPtr) {
				if (MyPawnPtr == ActorPtr)
					return;

				CharacterInfo Info;
				if (!GetCharacterInfo((uintptr_t)ActorPtr, Info))
					return;

				float Distance = MyInfo.Location.Distance(Info.Location) / 100.0f;

				bool bFocusingMe = false;
				//Get bFocusingMe
				[&] {
					float AccTime = EnemyInfoMap[Info.Ptr].FocusTime;
					EnemyInfoMap[Info.Ptr].FocusTime = 0.0f;

					if (MyInfo.Health <= 0.0f)
						return;
					if (Info.Health <= 0.0f)
						return;
					if (!Info.IsWeaponed)
						return;
					if (Info.GunRotation.Length() == 0.0f)
						return;
					if (!bTeamKill && Info.Team == MyInfo.Team)
						return;

					auto Result = GetBulletDropAndTravelTime(
						Info.GunLocation,
						Info.GunRotation,
						MyInfo.Location,
						Info.ZeroingDistance,
						Info.BulletDropAdd,
						Info.InitialSpeed,
						Info.Gravity,
						BallisticDragScale,
						BallisticDropScale,
						Info.BDS,
						Info.SimulationSubstepTime,
						Info.VDragCoefficient,
						Info.BallisticCurve);

					float BulletDrop = Result.first;
					float TravelTime = Result.second;

					float MinPitch = FLT_MAX;
					float MaxPitch = -FLT_MAX;
					float MinYaw = FLT_MAX;
					float MaxYaw = -FLT_MAX;

					for (auto BoneIndex : GetBoneIndexArray()) {
						FVector PredictedPos = MyInfo.BonesPos[BoneIndex];
						PredictedPos.Z += BulletDrop;
						PredictedPos = PredictedPos + MyInfo.Velocity * TravelTime;
						FRotator Rotator = (PredictedPos - Info.GunLocation).GetDirectionRotator();
						Rotator.Clamp();
						MinPitch = std::clamp(Rotator.Pitch, -FLT_MAX, MinPitch);
						MaxPitch = std::clamp(Rotator.Pitch, MaxPitch, FLT_MAX);
						MinYaw = std::clamp(Rotator.Yaw, -FLT_MAX, MinYaw);
						MaxYaw = std::clamp(Rotator.Yaw, MaxYaw, FLT_MAX);
					}

					float CenterYaw = (MinYaw + MaxYaw) / 2.0f;
					float RangeYaw = (MaxYaw - MinYaw) / 2.0f;
					RangeYaw += std::clamp(RangeYaw * 0.5f, 0.5f, FLT_MAX);

					FRotator GunRotation = Info.GunRotation;
					GunRotation.Clamp();

					if (GunRotation.Yaw > CenterYaw - RangeYaw &&
						GunRotation.Yaw < CenterYaw + RangeYaw) {
						EnemyInfoMap[Info.Ptr].FocusTime = AccTime + render.TimeDelta;
						if (EnemyInfoMap[Info.Ptr].FocusTime >= MinFocusTime)
							bFocusingMe = true;
					}
				}();

				//DrawRadar
				[&] {
					if (!bRadar)
						return;

					if (Info.State == CharacterState::Dead)
						return;

					const FVector RadarPos = (Info.Location - MyInfo.Location) * 0.01f;
					const FVector RadarScreenPos = {
						((1.0f + RadarPos.X / 200.0f) * RadarSize.x / 2.0f) * render.Width,
						((1.0f + RadarPos.Y / 200.0f) * RadarSize.y / 2.0f) * render.Height,
						0.0f
					};

					//GetColor
					ImColor Color = [&]()->ImColor {
						if (Info.Team == MyInfo.Team)
							return Render::COLOR_GREEN;
						if (Info.Health <= 0.0f)
							return Render::COLOR_GRAY;
						if (bFocusingMe)
							return Render::COLOR_RED;
						if (Info.IsAI)
							return Render::COLOR_TEAL;
						if (Info.IsInVehicle)
							return Render::COLOR_BLUE;
						if (Info.IsBlackListed)
							return Render::COLOR_BLACK;
						if (Info.Team < 51)
							return TeamColors[Info.Team];

						return Render::COLOR_WHITE;
					}();

					ImGui::SetNextWindowPos({ RadarFrom.x * render.Width, RadarFrom.y * render.Height });
					ImGui::SetNextWindowSize({ RadarSize.x * render.Width, RadarSize.y * render.Height });
					ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
					ImGui::Begin("Radar"e, 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
					ImGui::PopStyleColor();

					float Size = 4.0f;

					if (Info.GunRotation.Length() > 0.0f) {
						FVector GunDir = FRotator(0.0f, Info.GunRotation.Yaw, 0.0f).GetUnitVector();

						float Degree90 = ConvertToRadians(90.0f);
						FVector Dir1 = {
							GunDir.X * cosf(Degree90) - GunDir.Y * sinf(Degree90),
							GunDir.X * sinf(Degree90) + GunDir.Y * cosf(Degree90),
							0.0f };

						FVector Dir2 = {
							GunDir.X * cosf(-Degree90) - GunDir.Y * sinf(-Degree90),
							GunDir.X * sinf(-Degree90) + GunDir.Y * cosf(-Degree90),
							0.0f };

						FVector p1 = RadarScreenPos + GunDir * (Size + 1.0f) * 2.0f;
						FVector p2 = RadarScreenPos + Dir1 * (Size + 1.0f);
						FVector p3 = RadarScreenPos + Dir2 * (Size + 1.0f);
						render.DrawTriangle(p1, p2, p3, render.COLOR_BLACK);
						render.DrawCircle(RadarScreenPos, Size + 1.0f, render.COLOR_BLACK);

						p1 = RadarScreenPos + GunDir * Size * 2.0f;
						p2 = RadarScreenPos + Dir1 * Size;
						p3 = RadarScreenPos + Dir2 * Size;
						render.DrawTriangleFilled(p1, p2, p3, Color);
						render.DrawCircleFilled(RadarScreenPos, Size, Color);

						if (Info.IsFiring)
							render.DrawLine(RadarScreenPos, RadarScreenPos + Info.FiringRotation.GetUnitVector() * 500.0f, Color);
					}
					else {
						render.DrawCircle(RadarScreenPos, Size + 1.0f, render.COLOR_BLACK);
						render.DrawCircleFilled(RadarScreenPos, Size, Color);
					}

					ImGui::End();
				}();

				bool IsInCircle = false;
				//Aimbot
				[&] {
					if (!MyInfo.IsWeaponed)
						return;
					if (!Info.IsVisible)
						return;
					if (!bTeamKill && Info.Team == MyInfo.Team)
						return;
					if (!Info.IsLocked) {
						if (Info.State == CharacterState::Dead)
							return;
						if (Info.State == CharacterState::Groggy && !bPushingCTRL)
							return;
					}

					FVector AimPoint2D = WorldToScreen(Info.AimPoint);
					if (AimPoint2D.Z < 0.0f)
						return;

					AimPoint2D.Z = 0.0f;
					FVector Center2D = { render.Width / 2.0f, render.Height / 2.0f, 0 };

					float DistanceFromCenter = Center2D.Distance(AimPoint2D);
					if (DistanceFromCenter < AimbotCircleSize)
						IsInCircle = true;

					if (DistanceFromCenter > AimbotDistant)
						return;

					CurrentTargetPtr = (uintptr_t)ActorPtr;
					AimbotDistant = DistanceFromCenter;
				}();

				//DrawCharacter
				[&] {
					if (!bPlayer)
						return;

					if (Info.State == CharacterState::Dead)
						return;

					//GetColor
					ImColor Color = [&]()->ImColor {
						if (ActorPtr == LockTargetPtr)
							return Render::COLOR_PURPLE;
						if (Info.Team == MyInfo.Team)
							return Render::COLOR_GREEN;
						if (Info.Health <= 0.0f)
							return Render::COLOR_GRAY;
						if (bFocusingMe)
							return Render::COLOR_RED;
						if (Info.IsAI)
							return Render::COLOR_TEAL;
						if (IsInCircle)
							return Render::COLOR_YELLOW;
						if (Info.IsInVehicle)
							return Render::COLOR_BLUE;
						if (Info.IsBlackListed)
							return Render::COLOR_BLACK;
						if (Info.Team < 51)
							return TeamColors[Info.Team];

						return Render::COLOR_WHITE;
					}();

					//Draw Skeleton
					if (ESP_PlayerSetting.bSkeleton) {
						for (auto DrawPair : GetDrawPairArray())
							render.DrawLine(Info.BonesScreenPos[DrawPair.first], Info.BonesScreenPos[DrawPair.second], Color);
					}

					//Draw HealthBar
					FVector HealthBarPos = Info.BonesPos[neck_01];
					HealthBarPos.Z += 35.0f;
					FVector HealthBarScreenPos = WorldToScreen(HealthBarPos);
					HealthBarScreenPos.Y -= 5.0f;

					float HealthBarScreenLengthY = 0.0f;
					if (ESP_PlayerSetting.bHealth) {
						const float CameraDistance = CameraLocation.Distance(HealthBarPos) / 100.0f;
						if (Info.Health > 0.0)
							HealthBarScreenLengthY = DrawRatioBox(HealthBarScreenPos, CameraDistance, 0.7f,
								Info.Health, Render::COLOR_GREEN, Render::COLOR_RED, Render::COLOR_BLACK).y;
						else if (Info.GroggyHealth > 0.0)
							HealthBarScreenLengthY = DrawRatioBox(HealthBarScreenPos, CameraDistance, 0.7f,
								Info.GroggyHealth, Render::COLOR_RED, Render::COLOR_GRAY, Render::COLOR_BLACK).y;
					}

					//Draw CharacterInfo
					std::string PlayerInfo;
					std::string Line;

					if (ESP_PlayerSetting.bNickName)
						Line += Info.PlayerName;

					const unsigned NameHash = CompileTime::StrHash(Info.PlayerName.c_str());

					if (ESP_PlayerSetting.bRanksPoint && !Info.IsAI) {
						std::map<unsigned, RankInfo>& RankMap = *[&] {
							const bool bSolo = !(Info.Team < 200);

							if (ESP_PlayerSetting.bKakao && !bSolo && !IsFPPOnly)
								return &RankInfoKakaoSquad;

							if (IsFPPOnly && !bSolo)
								return &RankInfoSteamSquadFPP;

							if (!IsFPPOnly && bSolo)
								return &RankInfoSteamSolo;

							if (!IsFPPOnly && !bSolo)
								return &RankInfoSteamSquad;

							return &RankInfoEmpty;
						}();

						if (RankMap.find(NameHash) != RankMap.end()) {
							Line += (std::string)"("e;
							Line += std::to_string(RankMap[NameHash].rankPoint);
							Line += (std::string)")"e;
						}
					}

					if (ESP_PlayerSetting.bTeam) {
						if (Info.Team < 200 && Info.Team != MyInfo.Team) {
							if (!Line.empty())
								Line += (std::string)" "e;
							Line += std::to_string(Info.Team);
						}
					}

					if (!Line.empty()) {
						PlayerInfo += Line;
						PlayerInfo += (std::string)"\n"e;
						Line = {};
					}

					if (ESP_PlayerSetting.bWeapon) {
						if (Info.WeaponName.size()) {
							if (Info.Ammo == -1)
								Line += Info.WeaponName;
							else
								Line += Info.WeaponName + (std::string)"("e + std::to_string(Info.Ammo) + (std::string)")"e;
						}
					}

					if (!Line.empty()) {
						PlayerInfo += Line;
						PlayerInfo += (std::string)"\n"e;
						Line = {};
					}

					if (ESP_PlayerSetting.bDistance) {
						Line += std::to_string((int)Distance);
						Line += (std::string)"M"e;
					}

					if (ESP_PlayerSetting.bKill) {
						if (!Line.empty())
							Line += (std::string)" "e;
						Line += std::to_string(Info.NumKills);
					}

					if (ESP_PlayerSetting.bDamage) {
						if (!Line.empty())
							Line += (std::string)" "e;
						Line += std::to_string((int)Info.Damage);
					}

					if (!Line.empty()) {
						PlayerInfo += Line;
						PlayerInfo += (std::string)"\n"e;
						Line = {};
					}

					DrawString(
						{ HealthBarScreenPos.X, HealthBarScreenPos.Y - HealthBarScreenLengthY - render.GetTextSize(FONTSIZE, PlayerInfo.c_str()).y / 2.0f, HealthBarScreenPos.Z },
						PlayerInfo.c_str(), Color, true);
				}();
			};

			//Actor loop
			for (const auto& ActorPtr : Actors.GetVector()) {
				TSet<NativePtr<UActorComponent>> OwnedComponents;
				FVector ActorVelocity;
				FVector ActorLocation;
				FVector ActorLocationScreen;
				float DistanceToActor = 0.0f;
				unsigned ActorNameHash = 0;
				bool bAircraftCarePackage = false;

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
					DistanceToActor = MyInfo.Location.Distance(ActorLocation) / 100.0f;

					if (nRange != 1000 && DistanceToActor >= nRange)
						return false;

					USceneComponent AttachParent;
					//LocalPawn is vehicle(Localplayer is in vehicle)
					if (RootComponent.AttachParent.Read(AttachParent) && AttachParent.Owner == MyPawnPtr)
						return false;

					if (!Actor.GetName(szBuf, sizeof(szBuf)))
						return false;

					bAircraftCarePackage = (strncmp(szBuf, "AircraftCarePackage"e, sizeof("AircraftCarePackage"e) - 1) == 0);
					ActorNameHash = Actor.GetNameHash();

					if (bDebug) {
						FVector v2_DebugLoc = ActorLocationScreen;
						v2_DebugLoc.Y += 15.0;
						DrawString(v2_DebugLoc, szBuf, Render::COLOR_WHITE, false);
					}

					return true;
				};
				if (!GetValidActorInfo())
					continue;

				//DrawAircraft
				[&] {
					if (!bAircraftCarePackage)
						return;

					sprintf(szBuf, "%s\n%.0fM"e, (const char*)"Aircraft"e, DistanceToActor);
					DrawString(ActorLocationScreen, szBuf, Render::COLOR_TEAL, false);
				}();

				//DrawProj
				[&] {
					auto ProjInfo = GetProjInfo(ActorNameHash);
					auto& ProjName = ProjInfo.Name;
					if (!ProjName[0])
						return;

					if (!ProjInfo.IsLong && DistanceToActor > 300.0f)
						return;

					sprintf(szBuf, "%s\n%.0fM"e, ProjName.data(), DistanceToActor);
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
					auto& VehicleName = VehicleInfo.Name;
					if (!VehicleName[0])
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

					sprintf(szBuf, "%s\n%.0fM"e, VehicleName.data(), DistanceToActor);
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

				//DrawPackage
				[&] {
					if (!bBox || bFighterMode)
						return;

					auto PackageInfo = GetPackageInfo(ActorNameHash);
					auto& PackageName = PackageInfo.Name;
					if (!PackageName[0])
						return;

					if (PackageInfo.IsSmall && DistanceToActor > 300.0f)
						return;

					if (PackageInfo.IsSmall)
						sprintf(szBuf, "%s"e, PackageName.data());
					else
						sprintf(szBuf, "%s\n%.0fM"e, PackageName.data(), DistanceToActor);
					
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

						unsigned ItemComponentHash = ItemComponent.GetNameHash();
						if (!ItemComponentHash)
							continue;

						if (ItemComponentHash != "DroppedItemInteractionComponent"h && ItemComponentHash != "DestructibleItemInteractionComponent"h)
							continue;

						FVector ItemLocationScreen = WorldToScreen(ItemComponent.ComponentToWorld.Translation);
						DrawItem(ItemComponent.Item, ItemLocationScreen);
					}
				}();

				ProcessTslCharacter(ActorPtr);
			}

			float CustomTimeDilation = 1.0f;

			//TargetCharacter
			[&] {
				if (!MyInfo.IsWeaponed)
					return;

				if (!CurrentTargetPtr)
					return;

				if (!LockTargetPtr)
					LockTargetPtr = CurrentTargetPtr;

				CharacterInfo TargetInfo;
				if (!GetCharacterInfo(LockTargetPtr, TargetInfo))
					return;

				const FVector TargetPos = TargetInfo.AimPoint;
				const FVector TargetVelocity = TargetInfo.Velocity;

				auto Result = GetBulletDropAndTravelTime(
					MyInfo.AimLocation,
					MyInfo.AimRotation,
					TargetPos,
					MyInfo.ZeroingDistance,
					MyInfo.BulletDropAdd,
					MyInfo.InitialSpeed,
					MyInfo.Gravity,
					BallisticDragScale,
					BallisticDropScale,
					MyInfo.BDS,
					MyInfo.SimulationSubstepTime,
					MyInfo.VDragCoefficient,
					MyInfo.BallisticCurve);

				float BulletDrop = Result.first;
				float TravelTime = Result.second;

				FVector PredictedPos = FVector(TargetPos.X, TargetPos.Y, TargetPos.Z + BulletDrop) + TargetVelocity * (TravelTime / CustomTimeDilation);
				FVector TargetScreenPos = WorldToScreen(TargetPos);
				FVector AimScreenPos = WorldToScreen(PredictedPos);

				const float LineLen = std::clamp(AimScreenPos.Y - WorldToScreen({ TargetPos.X, TargetPos.Y, TargetPos.Z + 10.0f }).Y, 4.0f, 8.0f);
				render.DrawLine(TargetScreenPos, AimScreenPos, Render::COLOR_RED, 2.0f);
				render.DrawX(AimScreenPos, LineLen, Render::COLOR_RED, 2.0f);

				auto AImbot_MouseMove_Old = [&] {
					TimeDeltaAcc += render.TimeDelta;
					if (TimeSeconds == LastAimUpdateTime)
						return;

					LastAimUpdateTime = TimeSeconds;
					const float TimeDelta = TimeDeltaAcc;
					TimeDeltaAcc = 0.0f;

					const FVector LocTarget = ::WorldToScreen(PredictedPos, CameraRotationMatrix, CameraLocation, CameraFOV, 1.0f, 1.0f);
					const float DistanceToTarget = CameraLocation.Distance(PredictedPos) / 100.0f;
					const FVector GunCenterPos = CameraLocation + MyInfo.AimRotation.GetUnitVector() * DistanceToTarget;
					const FVector LocCenter = ::WorldToScreen(GunCenterPos, CameraRotationMatrix, CameraLocation, CameraFOV, 1.0f, 1.0f);

					const float MouseX = RemainMouseX + AimSpeedX * (LocTarget.X - LocCenter.X) * TimeDelta * 100000.0f;
					const float MouseY = RemainMouseY + AimSpeedY * (LocTarget.Y - LocCenter.Y) * TimeDelta * 100000.0f;
					RemainMouseX = MouseX - truncf(MouseX);
					RemainMouseY = MouseY - truncf(MouseY);
					MoveMouse(hGameWnd, { (int)MouseX, (int)MouseY });
				};

				auto AImbot_MouseMove = [&] {
					TimeDeltaAcc += render.TimeDelta;
					if (TimeSeconds == LastAimUpdateTime)
						return;

					LastAimUpdateTime = TimeSeconds;
					const float TimeDelta = TimeDeltaAcc;
					TimeDeltaAcc = 0.0f;

					FRotator RotationInput = (PredictedPos - CameraLocation).GetDirectionRotator() - MyInfo.AimRotation;
					RotationInput.Clamp();
					const POINT MaxXY = GetMouseXY(RotationInput * AimSpeedMaxFactor);
					if (MaxXY.x == 0 && MaxXY.y == 0) {
						RemainMouseX = RemainMouseY = 0.0f;
						return;
					}

					FVector FMouseXY = { (float)MaxXY.x, (float)MaxXY.y, 0.0f };
					FMouseXY.Normalize();

					const float MouseX = RemainMouseX + std::clamp(AimSpeedX * TimeDelta * FMouseXY.X, -(float)abs(MaxXY.x), (float)abs(MaxXY.x));
					const float MouseY = RemainMouseY + std::clamp(AimSpeedY * TimeDelta * FMouseXY.Y, -(float)abs(MaxXY.y), (float)abs(MaxXY.y));
					RemainMouseX = MouseX - truncf(MouseX);
					RemainMouseY = MouseY - truncf(MouseY);
					MoveMouse(hGameWnd, { (int)MouseX, (int)MouseY });
				};

				if (!bPushingMouseM)
					return;

				if (hGameWnd != hForeWnd)
					return;

				if (bAimbot)
					AImbot_MouseMove();

				if (bSilentAim && (bSilentAim_DangerousMode || MyInfo.IsScoping)) {
					FVector AimPoint2D = WorldToScreen(TargetPos);
					if (AimPoint2D.Z < 0.0f)
						return;

					AimPoint2D.Z = 0.0f;
					FVector Center2D = { render.Width / 2.0f, render.Height / 2.0f, 0 };

					float DistanceFromCenter = Center2D.Distance(AimPoint2D);
					if (DistanceFromCenter > SilentCircleSize)
						return;

					FVector RandedTargetPos = TargetPos;
					float angle1 = randf(0.0f, PI);
					float angle2 = randf(0.0f, PI * 2.0f);
					float radious = randf(0.0f, RandSilentAim);
					RandedTargetPos.X += radious * sinf(angle1) * cosf(angle2);
					RandedTargetPos.Y += radious * sinf(angle1) * sinf(angle2);
					RandedTargetPos.Z += radious * cosf(angle1);

					Result = GetBulletDropAndTravelTime(
						MyInfo.AimLocation,
						MyInfo.AimRotation,
						RandedTargetPos,
						FLT_MIN,
						MyInfo.BulletDropAdd,
						MyInfo.InitialSpeed,
						MyInfo.Gravity,
						BallisticDragScale,
						BallisticDropScale,
						MyInfo.BDS,
						MyInfo.SimulationSubstepTime,
						MyInfo.VDragCoefficient,
						MyInfo.BallisticCurve);

					BulletDrop = Result.first;
					TravelTime = Result.second;
					PredictedPos = FVector(RandedTargetPos.X, RandedTargetPos.Y, RandedTargetPos.Z + BulletDrop) 
						+ TargetVelocity * (TravelTime / CustomTimeDilation);

					FVector DirectionInput = PredictedPos - MyInfo.AimLocation;
					DirectionInput.Normalize();

					ChangeRegOnBPInfo Info{};
					Info.changeXMM6_0 = true;
					Info.changeXMM7_0 = true;
					Info.changeXMM8_0 = true;
					Info.XMM6.Float_0 = DirectionInput.X;
					Info.XMM7.Float_0 = DirectionInput.Y;
					Info.XMM8.Float_0 = DirectionInput.Z;
					dbvm.ChangeRegisterOnBP(AimHookAddressPA, Info);
					IsNeedToHookAim = true;
				}
			}();

			if (!bPushingCapsLock) {
				EnemyFocusingMePtr = 0;
				bPushedCapsLock = false;
			}
			else if(!bPushedCapsLock) {
				bPushedCapsLock = true;

				switch (nCapsLockMode) {
				case 1:
					MoveMouse(hGameWnd, GetMouseXY({ 0.0f, 180.0f, 0.0f }));
					break;
				case 2:
					NativePtr<ATslCharacter> Ptr;

					for (auto Elem : EnemyInfoMap) {
						if (Elem.second.FocusTime > MinFocusTime) {
							if (Elem.first == EnemyFocusingMePtr)
								break;
							if (!Ptr)
								Ptr = Elem.first;
						}
						else if (Elem.first == EnemyFocusingMePtr)
							EnemyFocusingMePtr = 0;
					}

					CharacterInfo Info;
					if (!GetCharacterInfo(Ptr, Info))
						break;

					LockTargetPtr = Info.Ptr;
					EnemyFocusingMePtr = Info.Ptr;

					auto Result = GetBulletDropAndTravelTime(
						MyInfo.AimLocation,
						MyInfo.AimRotation,
						Info.BonesPos[forehead],
						MyInfo.ZeroingDistance,
						MyInfo.BulletDropAdd,
						MyInfo.InitialSpeed,
						MyInfo.Gravity,
						BallisticDragScale,
						BallisticDropScale,
						MyInfo.BDS,
						MyInfo.SimulationSubstepTime,
						MyInfo.VDragCoefficient,
						MyInfo.BallisticCurve);

					float BulletDrop = Result.first;
					float TravelTime = Result.second;

					FVector PredictedPos = FVector(Info.AimPoint.X, Info.AimPoint.Y, Info.AimPoint.Z + BulletDrop) + Info.Velocity * (TravelTime / CustomTimeDilation);
					FRotator RotationInput = (PredictedPos - CameraLocation).GetDirectionRotator() - MyInfo.AimRotation;
					RotationInput.Clamp();
					MoveMouse(hGameWnd, GetMouseXY(RotationInput));
					break;
				}
			}

			if (!IsNeedToHookAim)
				dbvm.RemoveChangeRegisterOnBP(AimHookAddressPA);

			if (MyInfo.SpectatedCount > 0)
				DrawSpectatedCount(MyInfo.SpectatedCount, Render::COLOR_RED);

			std::string Enemies;
			for (auto Elem : EnemyInfoMap) {
				if (Elem.second.FocusTime < MinFocusTime)
					continue;

				CharacterInfo Info;
				if (!GetCharacterInfo(Elem.first, Info))
					continue;

				float Distance = MyInfo.Location.Distance(Info.Location) / 100.0f;

				Enemies += Info.PlayerName + (std::string)" : "e + std::to_string((int)Distance) + (std::string)"M\n"e;
			}
			DrawEnemiesFocusingMe(Enemies.c_str(), Render::COLOR_RED);

			if (MyInfo.IsWeaponed) {
				if (bAimbot)
					render.DrawCircle({ render.Width / 2.0f, render.Height / 2.0f, 0.0f }, AimbotCircleSize, IM_COL32(255, 255, 255, 100));
				if (bSilentAim)
					render.DrawCircle({ render.Width / 2.0f, render.Height / 2.0f, 0.0f }, SilentCircleSize, IM_COL32(255, 255, 0, 100));
			}
		};

		render.RenderArea(hGameWnd, Render::COLOR_CLEAR, [&] {
			if (!ExceptionHandler::TryExcept(FuncInRenderArea))
				printlog("Error : %X\n"e, ExceptionHandler::GetLastExceptionCode());
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
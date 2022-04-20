#pragma once
#include "common/defclass.h"
#include "pubg_struct.h"
#include "GObjects.h"

#include "info_weapon.h"
#include "info_item.h"

class UWorld;
class ULevel;
class UGameInstance;
class ULocalPlayer;
class AController;
class APlayerController;
class UPlayerInput;
class APlayerCameraManager;
class AActor;
class APlayerState;
class APawn;
class ACharacter;
class UTslSettings;
class ATslCharacter;
class ATslWheeledVehicle;
class ATslFloatingVehicle;
class UTslVehicleCommonComponent;
class UActorComponent;
class USceneComponent;
class UPrimitiveComponent;
class UCapsuleComponent;
class UMeshComponent;
class USkinnedMeshComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class USkeletalMeshSocket;
class UStaticMeshSocket;
class USkeletalMesh;
class UStaticMesh;
class UWeaponMeshComponent;
class USkeleton;
class ADroppedItem;
class UDroppedItemInteractionComponent;
class FItemTableRowBase;
class UItem;
class UVehicleRiderComponent;
class UWeaponProcessorComponent;
class ATslWeapon;
class ATslWeapon_Gun;
class ATslWeapon_Trajectory;
class ATslWeapon_Melee;
class ATslWeapon_Throwable;
class UAnimInstance;
class UTslAnimInstance;
class UWeaponTrajectoryData;
class AItemPackage;
class ATslPlayerState;
class UCurveVector;
class FWeaponAttachmentWeaponTagData;
class FItemTableRowAttachment;

//Engine.World.CurrentLevel 
//Function TslGame.TslLivingThing.GetWorldTimeSeconds 
DefClass(UWorld, UObject,
	MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x3f8)
	MemberAtOffset(EncryptedPtr<UGameInstance>, GameInstance, 0x278)

	MemberAtOffset(float, TimeSeconds, 0x130)
	,
	constexpr static uintptr_t BASE = 0x8CF7E50;
	static bool GetUWorld(UWorld& World);
)

DefClass(UGameInstance, UObject,
	MemberAtOffset(TArray<EncryptedPtr<ULocalPlayer>>, LocalPlayers, 0x50)
)

DefClass(ULocalPlayer, UObject,
	MemberAtOffset(EncryptedPtr<APlayerController>, PlayerController, 0x40)
)

//Engine.Controller.Character 
//Engine.Controller.Pawn 
DefClass(AController, UObject,
	MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x430)
	MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x460)
)

//Function Engine.PlayerInput.SetMouseSensitivity 
DefClass(UPlayerInput, UObject,
	MemberAtOffset(UNPACK(TMap<FName, FInputAxisProperties>), AxisProperties, 0x140)
)

//Engine.PlayerController.SpectatorPawn 
//Engine.PlayerController.PlayerCameraManager 
//Engine.PlayerController.PlayerInput 
DefClass(APlayerController, AController,
	MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x738)
	MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4a8)
	MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x520)
)

//TslGame.TslPlayerController.DefaultFOV 
DefClass(ATslPlayerController, APlayerController,
	MemberAtOffset(float, DefaultFOV, 0xaec)
)

//Engine.PlayerCameraManager.CameraCache +
//Engine.CameraCacheEntry.POV +
//Engine.MinimalViewInfo.Fov 
//Engine.MinimalViewInfo.Rotation 
//Engine.MinimalViewInfo.Location 
DefClass(APlayerCameraManager, UObject,
	MemberAtOffset(float, CameraCache_POV_FOV, 0x4c0 + 0x10 + 0x24)
	MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x4c0 + 0x10 + 0x4)
	MemberAtOffset(FVector, CameraCache_POV_Location, 0x4c0 + 0x10 + 0x5ac)
)

DefClass(ULevel, UObject,
	MemberAtOffset(EncryptedPtr<TArray<NativePtr<AActor>>>, Actors, 0x98)
)

//Engine.Actor.RootComponent 
//Engine.Actor.ReplicatedMovement 
DefClass(AActor, UObject,
	MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x298)
	MemberAtOffset(FRepMovement, ReplicatedMovement, 0x78)

	MemberAtOffset(TSet<NativePtr<UActorComponent>>, OwnedComponents, 0x318)
	,
	FTransform ActorToWorld() const;
)

DefClass(APlayerState, UObject, )

//Engine.Pawn.PlayerState 
DefClass(APawn, AActor,
	MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x420)
)

//Engine.Character.Mesh 
DefClass(ACharacter, APawn,
	MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x548)
	MemberAtOffset(FVector, BaseTranslationOffset, 0x460)
)

//TslGame.TslCharacter.Health 
//TslGame.TslCharacter.HealthMax 
//TslGame.TslCharacter.GroggyHealth 
//TslGame.TslCharacter.GroggyHealthMax 
//TslGame.TslCharacter.CharacterName 
//TslGame.TslCharacter.LastTeamNum 
//TslGame.TslCharacter.VehicleRiderComponent 
//TslGame.TslCharacter.WeaponProcessor 
//TslGame.TslCharacter.SpectatedCount 
DefClass(ATslCharacter, ACharacter,
	MemberAtOffset(float, Health, 0x1bb8)
	MemberAtOffset(float, HealthMax, 0x273c)
	MemberAtOffset(float, GroggyHealth, 0x12a0)
	MemberAtOffset(float, GroggyHealthMax, 0x1ba4)
	MemberAtOffset(FString, CharacterName, 0x1be8)
	MemberAtOffset(int, LastTeamNum, 0xe40)
	MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1c70)
	MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x1bb0)
	MemberAtOffset(int, SpectatedCount, 0x1288)
	MemberAtOffset(FRotator, AimOffsets, 0x16b0)
	,
	bool GetTslWeapon_Trajectory(ATslWeapon_Trajectory& OutTslWeapon) const;
	bool GetTslWeapon(ATslWeapon& OutTslWeapon) const;
)

//TslGame.TslWheeledVehicle.VehicleCommonComponent 
DefClass(ATslWheeledVehicle, APawn,
	MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xad8)
)

//TslGame.TslFloatingVehicle.VehicleCommonComponent 
DefClass(ATslFloatingVehicle, APawn,
	MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4c0)
)

//Function Engine.ActorComponent.GetOwner 
DefClass(UActorComponent, UObject,
	MemberAtOffset(NativePtr<AActor>, Owner, 0x1B8)
)

//TslGame.TslVehicleCommonComponent.Health 
//TslGame.TslVehicleCommonComponent.HealthMax 
//TslGame.TslVehicleCommonComponent.Fuel 
//TslGame.TslVehicleCommonComponent.FuelMax 
DefClass(UTslVehicleCommonComponent, UActorComponent,
	MemberAtOffset(float, Health, 0x2d0)
	MemberAtOffset(float, HealthMax, 0x2d4)
	MemberAtOffset(float, Fuel, 0x2d8)
	MemberAtOffset(float, FuelMax, 0x2dc)
)

//Engine.SceneComponent.ComponentVelocity 
//Engine.SceneComponent.AttachParent 
//Function Engine.SceneComponent.K2_GetComponentToWorld 
DefClass(USceneComponent, UActorComponent,
	MemberAtOffset(FVector, ComponentVelocity, 0x200)
	MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x380)

	MemberAtOffset(FTransform, ComponentToWorld, 0x260)
	,
	FTransform GetSocketTransform(FName SocketName, ERelativeTransformSpace TransformSpace) const;
)

//Engine.PrimitiveComponent.LastSubmitTime 
//Engine.PrimitiveComponent.LastRenderTimeOnScreen 
DefClass(UPrimitiveComponent, USceneComponent,
	MemberAtOffset(float, LastSubmitTime, 0x778)
	MemberAtOffset(float, LastRenderTimeOnScreen, 0x780)
	,
	bool IsVisible() const { return LastRenderTimeOnScreen + 0.05f >= LastSubmitTime; }
)

DefClass(UShapeComponent, UPrimitiveComponent, )
DefClass(UCapsuleComponent, UShapeComponent, )
DefClass(UMeshComponent, UPrimitiveComponent, )

//Engine.SkinnedMeshComponent.SkeletalMesh 
DefClass(USkinnedMeshComponent, UMeshComponent,
	MemberAtOffset(NativePtr<USkeletalMesh>, SkeletalMesh, 0xad8)

	MemberAtOffset(TArray<FTransform>, BoneSpaceTransforms, 0xAE8)
	,
	FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const;
	FTransform GetBoneTransform(int32 BoneIdx, const FTransform& LocalToWorld) const;
	FTransform GetBoneTransform(int32 BoneIdx) const;
	bool GetSocketInfoByName(FName InSocketName, FTransform& OutTransform, int32& OutBoneIndex, USkeletalMeshSocket& OutSocket) const;
	int32 GetBoneIndex(FName BoneName) const;
	FName GetParentBone(FName BoneName) const;
)

//Engine.SkeletalMeshComponent.AnimScriptInstance 
DefClass(USkeletalMeshComponent, USkinnedMeshComponent,
	MemberAtOffset(NativePtr<UAnimInstance>, AnimScriptInstance, 0xc90)
)

//Engine.StaticMeshComponent.StaticMesh 
DefClass(UStaticMeshComponent, UMeshComponent,
	MemberAtOffset(NativePtr<UStaticMesh>, StaticMesh, 0xae8)
	,
	FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const;
	bool GetSocketByName(FName InSocketName, UStaticMeshSocket& OutSocket) const;
)

//Engine.SkeletalMeshSocket.SocketName 
//Engine.SkeletalMeshSocket.BoneName 
//Engine.SkeletalMeshSocket.RelativeLocation 
//Engine.SkeletalMeshSocket.RelativeRotation 
//Engine.SkeletalMeshSocket.RelativeScale 
DefClass(USkeletalMeshSocket, UObject,
	MemberAtOffsetZero(FName, SocketName, 0x38)
	MemberAtOffset(FName, BoneName, 0x40)
	MemberAtOffset(FVector, RelativeLocation, 0x48)
	MemberAtOffset(FRotator, RelativeRotation, 0x54)
	MemberAtOffset(FVector, RelativeScale, 0x60)
	,
	FTransform GetSocketLocalTransform() const;
)

//Engine.StaticMeshSocket.SocketName 
//Engine.StaticMeshSocket.RelativeLocation 
//Engine.StaticMeshSocket.RelativeRotation 
//Engine.StaticMeshSocket.RelativeScale 
DefClass(UStaticMeshSocket, UObject,
	MemberAtOffsetZero(FName, SocketName, 0x38)
	MemberAtOffset(FVector, RelativeLocation, 0x40)
	MemberAtOffset(FRotator, RelativeRotation, 0x4c)
	MemberAtOffset(FVector, RelativeScale, 0x58)
	,
	bool GetSocketTransform(FTransform& OutTransform, const UStaticMeshComponent& MeshComp) const;
)

//Engine.SkeletalMesh.Skeleton 
//Engine.SkeletalMesh.Sockets 
//Function Engine.SkinnedMeshComponent.GetNumBones -> Get FinalRefBoneInfo Offset(-8)
DefClass(USkeletalMesh, UObject,
	MemberAtOffset(NativePtr<USkeleton>, Skeleton, 0x58)
	MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x2d8)

	MemberAtOffset(TArray<FMeshBoneInfo>, FinalRefBoneInfo, 0x160 - 0x8)
	,
	bool FindSocketInfo(FName InSocketName, FTransform& OutTransform, int32& OutBoneIndex, int32& OutIndex, USkeletalMeshSocket& OutSocket) const;
	int FindBoneIndex(FName BoneName) const;
	FName GetBoneName(const int32 BoneIndex) const;
	int32 GetParentIndex(const int32 BoneIndex) const;
	int32 GetParentIndexInternal(const int32 BoneIndex, const TArray<FMeshBoneInfo>& BoneInfo) const;
)

//Engine.StaticMesh.Sockets 
DefClass(UStaticMesh, UObject,
	MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd0)
	,
	bool FindSocket(FName InSocketName, UStaticMeshSocket& OutSocket) const;
)

//TslGame.WeaponMeshComponent.AttachedStaticComponentMap 
DefClass(UWeaponMeshComponent, USkeletalMeshComponent,
	MemberAtOffset(UNPACK(TMap<TEnumAsByte<EWeaponAttachmentSlotID>, NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)
	,
	NativePtr<UStaticMeshComponent> GetStaticMeshComponentScopeType() const;
float GetScopingAttachPointRelativeZ(FName ScopingAttachPoint) const;
)

//Engine.Skeleton.Sockets 
DefClass(USkeleton, UObject,
	MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a0)
	,
	bool FindSocketAndIndex(FName InSocketName, int32& OutIndex, USkeletalMeshSocket& Socket) const;
)

//TslGame.DroppedItem.Item 
DefClass(ADroppedItem, AActor,
	MemberAtOffset(EncryptedPtr<UItem>, Item, 0x428)
)

//TslGame.DroppedItemInteractionComponent.Item 
DefClass(UDroppedItemInteractionComponent, USceneComponent,
	MemberAtOffset(NativePtr<UItem>, Item, 0x730)
)

//Function TslGame.Item.BP_GetItemID 
DefBaseClass(FItemTableRowBase,
	MemberAtOffset(FName, ItemID, 0x248)
)

//Function TslGame.Item.BP_GetItemID 
DefClass(UItem, UObject,
	MemberAtOffset(NativePtr<FItemTableRowBase>, ItemTable, 0xB8)
	,
	FName GetItemID() const;
	unsigned GetHash() const;
	ItemInfo GetInfo() const;
)

//TslGame.VehicleRiderComponent.SeatIndex 
//TslGame.VehicleRiderComponent.LastVehiclePawn 
DefClass(UVehicleRiderComponent, UActorComponent,
	MemberAtOffset(int, SeatIndex, 0x228)
	MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x260)
)

//TslGame.WeaponProcessorComponent.EquippedWeapons 
//Function TslGame.WeaponProcessorComponent.GetWeaponIndex 
DefClass(UWeaponProcessorComponent, UActorComponent,
	MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2b8)
	MemberAtOffset(uint8_t, WeaponArmInfo_RightWeaponIndex, 0x2d8 + 0x1)
)

//Function TslGame.AttachableItem.GetAttachmentData 
DefClass(UAttachableItem, UItem,
	MemberAtOffset(NativePtr<FItemTableRowAttachment>, WeaponAttachmentData, 0x130)
)

DefClass(FItemTableRowAttachment, FItemTableRowBase,)

//TslGame.TslWeapon.AttachedItems 
//TslGame.TslWeapon.Mesh3P 
//TslGame.TslWeapon.WeaponTag 
//TslGame.TslWeapon.FiringAttachPoint 
DefClass(ATslWeapon, AActor,
	MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x7d8)
	MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x790)
	MemberAtOffset(FName, WeaponTag, 0x7e8)
	MemberAtOffset(FName, FiringAttachPoint, 0x830)
	,
	tWeaponInfo GetWeaponInfo() const;
)

//TslGame.TslWeapon_Gun.CurrentZeroLevel 
//TslGame.TslWeapon_Gun.ScopingAttachPoint 
//TslGame.TslWeapon_Gun.CurrentAmmoData
//TslGame.TslWeapon_Gun.CurrentState
//Function TslGame.TslWeapon_Gun.GetCurrentScopeZeroingLevel 
//Function TslGame.TslWeapon_Gun.WeaponCycleDone 
DefClass(ATslWeapon_Gun, ATslWeapon,
	MemberAtOffset(int, CurrentZeroLevel, 0x9fc)
	MemberAtOffset(FName, ScopingAttachPoint, 0xbe8)
	MemberAtOffset(uint16_t, CurrentAmmoData, 0x9f8)
	MemberAtOffset(TEnumAsByte<EWeaponState>, CurrentState, 0x9f2)
	 
	MemberAtOffset(TEnumAsByte<bool>, bCantedSighted, 0xE85)
	MemberAtOffset(TEnumAsByte<bool>, bWeaponCycleDone, 0xA1D)
	,
	int GetCurrentAmmo() const { return HIBYTE(CurrentAmmoData) ? -1 : CurrentAmmoData; }
)

//TslGame.TslWeapon_Trajectory.WeaponTrajectoryData 
//TslGame.TslWeapon_Trajectory.TrajectoryGravityZ 
DefClass(ATslWeapon_Trajectory, ATslWeapon_Gun,
	MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1008)
	MemberAtOffset(float, TrajectoryGravityZ, 0xf4c)
	,
	float GetZeroingDistance(bool IsScoping) const;
)

DefClass(ATslWeapon_Throwable, ATslWeapon_Gun, );
DefClass(ATslWeapon_Melee, ATslWeapon, );

DefClass(UAnimInstance, UObject, )

//TslGame.TslAnimInstance.ControlRotation_CP 
//TslGame.TslAnimInstance.RecoilADSRotation_CP 
//TslGame.TslAnimInstance.LeanLeftAlpha_CP 
//TslGame.TslAnimInstance.LeanRightAlpha_CP 
//TslGame.TslAnimInstance.bIsScoping_CP 
DefClass(UTslAnimInstance, UAnimInstance,
	MemberAtOffset(FRotator, ControlRotation_CP, 0x73c)
	MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9c4)
	MemberAtOffset(float, LeanLeftAlpha_CP, 0xdd4)
	MemberAtOffset(float, LeanRightAlpha_CP, 0xdd8)
	MemberAtOffset(TEnumAsByte<bool>, bIsScoping_CP, 0xcee)
	MemberAtOffset(TEnumAsByte<bool>, bLocalFPP_CP, 0xf18)
)

//TslGame.WeaponTrajectoryData.TrajectoryConfig 
DefClass(UWeaponTrajectoryData, UObject,
	MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x110)
)

//TslGame.ItemPackage.Items 
DefClass(AItemPackage, AActor,
	MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x558)
)

//TslGame.TslPlayerState.PlayerStatistics 
//TslGame.TslPlayerState.DamageDealtOnEnemy 
DefClass(ATslPlayerState, APlayerState,
	MemberAtOffset(float, DamageDealtOnEnemy, 0x8a8)
	MemberAtOffset(FTslPlayerStatistics, PlayerStatistics, 0xa2c)
)

//Engine.CurveVector.FloatCurves 
DefClass(UCurveVector, UObject,
	MemberAtOffset(FRichCurve, FloatCurves, 0x40)
)
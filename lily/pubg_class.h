#pragma once
#include "xenuine.h"
#include "pubg_struct.h"
#include "GObjects.h"
#include "transform.h"
#include "vector.h"
#include "defclass.h"

class UWorld;
class ULevel;
class UGameInstance;
class ULocalPlayer;
class AController;
class APlayerController;
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
class UAnimInstance;
class UTslAnimInstance;
class UWeaponTrajectoryData;
class AItemPackage;
class ATslPlayerState;
class UCurveVector;
class FWeaponAttachmentWeaponTagData;
class FItemTableRowAttachment;

constexpr uintptr_t UWORLDBASE = 0x8F927B0;

//Engine.World.CurrentLevel 
DefClass(UWorld, UObject,
	MemberAtOffset(EncryptedObjectPtr<ULevel>, CurrentLevel, 0x48)
	MemberAtOffset(EncryptedObjectPtr<UGameInstance>, GameInstance, 0x150)
	,
	static bool GetUWorld(UWorld& World);
)

DefClass(UGameInstance, UObject,
	MemberAtOffset(TArray<EncryptedObjectPtr<ULocalPlayer>>, LocalPlayers, 0x58)
,)

DefClass(ULocalPlayer, UObject,
	MemberAtOffset(EncryptedObjectPtr<APlayerController>, PlayerController, 0x48)
,)

//Engine.Controller.Character 
//Engine.Controller.Pawn 
DefClass(AController, UObject,
	MemberAtOffset(EncryptedObjectPtr<ACharacter>, Character, 0x468)
	MemberAtOffset(EncryptedObjectPtr<APawn>, Pawn, 0x428)
,)

//Engine.PlayerController.SpectatorPawn 
//Engine.PlayerController.PlayerCameraManager 
//Engine.PlayerController.InputYawScale 
//Engine.PlayerController.InputPitchScale 
//Engine.PlayerController.InputRollScale 
DefClass(APlayerController, AController,
	MemberAtOffset(ObjectPtr<APawn>, SpectatorPawn, 0x748)
	MemberAtOffset(ObjectPtr<APlayerCameraManager>, PlayerCameraManager, 0x4b8)
	MemberAtOffset(float, InputYawScale, 0x634)
	MemberAtOffset(float, InputPitchScale, 0x638)
	MemberAtOffset(float, InputRollScale, 0x63c)

	MemberAtOffset(FRotator, RotationInput, 0x628)
,)

DefClass(ATslPlayerController, APlayerController,
	MemberAtOffset(float, DefaultFOV, 0xad4)
,)

//Engine.PlayerCameraManager.CameraCache +
//Engine.CameraCacheEntry.POV +
//Engine.MinimalViewInfo.Fov 
//Engine.MinimalViewInfo.Rotation 
//Engine.MinimalViewInfo.Location 
DefClass(APlayerCameraManager, UObject,
	MemberAtOffset(float, CameraCache_POV_FOV, 0x430 + 0x10 + 0x5b0)
	MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x430 + 0x10 + 0x24)
	MemberAtOffset(FVector, CameraCache_POV_Location, 0x430 + 0x10 + 0x10)
,)

DefClass(ULevel, UObject,
	MemberAtOffset(EncryptedObjectPtr<TArray<ObjectPtr<AActor>>>, Actors, 0x1F8)
,)

//Engine.Actor.RootComponent 
//Engine.Actor.ReplicatedMovement 
DefClass(AActor, UObject,
	MemberAtOffset(EncryptedObjectPtr<USceneComponent>, RootComponent, 0x60)
	MemberAtOffset(FRepMovement, ReplicatedMovement, 0x88)

	MemberAtOffset(TSet<ObjectPtr<UActorComponent>>, OwnedComponents, 0x108)
	,
	Transform ActorToWorld() const;
)

DefClass(APlayerState, UObject,,)

//Engine.Pawn.PlayerState 
DefClass(APawn, AActor,
	MemberAtOffset(EncryptedObjectPtr<APlayerState>, PlayerState, 0x438)
,)

//Engine.Character.Mesh 
DefClass(ACharacter, APawn,
	MemberAtOffset(ObjectPtr<USkeletalMeshComponent>, Mesh, 0x4c0)
,)

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
	MemberAtOffset(float, Health, 0x20e0)
	MemberAtOffset(float, HealthMax, 0x13ac)
	MemberAtOffset(float, GroggyHealth, 0x13b8)
	MemberAtOffset(float, GroggyHealthMax, 0xef4)
	MemberAtOffset(FString, CharacterName, 0xef8)
	MemberAtOffset(int, LastTeamNum, 0xe10)
	MemberAtOffset(ObjectPtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1b10)
	MemberAtOffset(ObjectPtr<UWeaponProcessorComponent>, WeaponProcessor, 0xf90)
	MemberAtOffset(int, SpectatedCount, 0x1108)
	,
	bool GetTslWeapon(ATslWeapon_Trajectory& OutTslWeapon) const;
)

//TslGame.TslWheeledVehicle.VehicleCommonComponent 
DefClass(ATslWheeledVehicle, APawn,
	MemberAtOffset(ObjectPtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xaa0)
,)

//TslGame.TslFloatingVehicle.VehicleCommonComponent 
DefClass(ATslFloatingVehicle, APawn,
	MemberAtOffset(ObjectPtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4b8)
,)

//Function Engine.ActorComponent.GetOwner 
DefClass(UActorComponent, UObject,
	MemberAtOffset(ObjectPtr<AActor>, Owner, 0x1C8)
,)

//TslGame.TslVehicleCommonComponent.Health 
//TslGame.TslVehicleCommonComponent.HealthMax 
//TslGame.TslVehicleCommonComponent.Fuel 
//TslGame.TslVehicleCommonComponent.FuelMax 
//TslGame.TslVehicleCommonComponent.ExplosionTimer 
DefClass(UTslVehicleCommonComponent, UActorComponent,
	MemberAtOffset(float, Health, 0x2dc)
	MemberAtOffset(float, HealthMax, 0x2e0)
	MemberAtOffset(float, Fuel, 0x2e4)
	MemberAtOffset(float, FuelMax, 0x2e8)
,)

//Engine.SceneComponent.ComponentVelocity 
//Engine.SceneComponent.AttachParent 
//Function Engine.SceneComponent.K2_GetComponentToWorld 
DefClass(USceneComponent, UActorComponent,
	MemberAtOffset(FVector, ComponentVelocity, 0x210)
	MemberAtOffset(ObjectPtr<USceneComponent>, AttachParent, 0x258)

	MemberAtOffset(FTransform, ComponentToWorld, 0x360)
	,
	Transform GetSocketTransform(FName SocketName, ERelativeTransformSpace TransformSpace) const;
)

//Engine.PrimitiveComponent.LastSubmitTime 
//Engine.PrimitiveComponent.LastRenderTimeOnScreen 
DefClass(UPrimitiveComponent, USceneComponent,
	MemberAtOffset(float, LastSubmitTime, 0x778)
	MemberAtOffset(float, LastRenderTimeOnScreen, 0x780)
	,
	bool IsVisible() const { return LastRenderTimeOnScreen + 0.12f >= LastSubmitTime; }
)

DefClass(UMeshComponent, UPrimitiveComponent,,)

//Engine.SkinnedMeshComponent.SkeletalMesh 
DefClass(USkinnedMeshComponent, UMeshComponent,
	MemberAtOffset(ObjectPtr<USkeletalMesh>, SkeletalMesh, 0xad8)

	MemberAtOffset(TArray<FTransform>, BoneSpaceTransforms, 0xAE8)
	,
	Transform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const;
	Transform GetBoneTransform(int32 BoneIdx, const Transform& LocalToWorld) const;
	Transform GetBoneTransform(int32 BoneIdx) const;
	bool GetSocketInfoByName(FName InSocketName, Transform& OutTransform, int32& OutBoneIndex, USkeletalMeshSocket& OutSocket) const;
	int32 GetBoneIndex(FName BoneName) const;
	FName GetParentBone(FName BoneName) const;
)

//Engine.SkeletalMeshComponent.AnimScriptInstance 
DefClass(USkeletalMeshComponent, USkinnedMeshComponent,
	MemberAtOffset(ObjectPtr<UAnimInstance>, AnimScriptInstance, 0xc90)
,)

//Engine.StaticMeshComponent.StaticMesh 
DefClass(UStaticMeshComponent, UMeshComponent,
	MemberAtOffset(ObjectPtr<UStaticMesh>, StaticMesh, 0xae8)
	,
	Transform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const;
	bool GetSocketByName(FName InSocketName, UStaticMeshSocket& OutSocket) const;
)

//Engine.SkeletalMeshSocket.SocketName 
//Engine.SkeletalMeshSocket.BoneName 
//Engine.SkeletalMeshSocket.RelativeLocation 
//Engine.SkeletalMeshSocket.RelativeRotation 
//Engine.SkeletalMeshSocket.RelativeScale 
DefClass(USkeletalMeshSocket, UObject,
	MemberAtOffsetZero(FName, SocketName, 0x40)
	MemberAtOffset(FName, BoneName, 0x48)
	MemberAtOffset(FVector, RelativeLocation, 0x50)
	MemberAtOffset(FRotator, RelativeRotation, 0x5c)
	MemberAtOffset(FVector, RelativeScale, 0x68)
	,
	Transform GetSocketLocalTransform() const;
)

//Engine.StaticMeshSocket.SocketName 
//Engine.StaticMeshSocket.RelativeLocation 
//Engine.StaticMeshSocket.RelativeRotation 
//Engine.StaticMeshSocket.RelativeScale 
DefClass(UStaticMeshSocket, UObject,
	MemberAtOffsetZero(FName, SocketName, 0x40)
	MemberAtOffset(FVector, RelativeLocation, 0x48)
	MemberAtOffset(FRotator, RelativeRotation, 0x54)
	MemberAtOffset(FVector, RelativeScale, 0x60)
	,
	bool GetSocketTransform(Transform& OutTransform, const UStaticMeshComponent& MeshComp) const;
)

//Engine.SkeletalMesh.Skeleton 
//Engine.SkeletalMesh.Sockets 
//Function Engine.SkinnedMeshComponent.GetNumBones -> Get FinalRefBoneInfo Offset(-8)
DefClass(USkeletalMesh, UObject,
	MemberAtOffset(ObjectPtr<USkeleton>, Skeleton, 0x60)
	MemberAtOffset(TArray<ObjectPtr<USkeletalMeshSocket>>, Sockets, 0x2e0)

	MemberAtOffset(TArray<FMeshBoneInfo>, FinalRefBoneInfo, 0x168 - 0x8)
	,
	bool FindSocketInfo(FName InSocketName, Transform& OutTransform, int32& OutBoneIndex, int32& OutIndex, USkeletalMeshSocket& OutSocket) const;
	int FindBoneIndex(FName BoneName) const;
	FName GetBoneName(const int32 BoneIndex) const;
	int32 GetParentIndex(const int32 BoneIndex) const;
	int32 GetParentIndexInternal(const int32 BoneIndex, const TArray<FMeshBoneInfo>& BoneInfo) const;
)

//Engine.StaticMesh.Sockets 
DefClass(UStaticMesh, UObject,
	MemberAtOffset(TArray<ObjectPtr<UStaticMeshSocket>>, Sockets, 0xd8)
	,
	bool FindSocket(FName InSocketName, UStaticMeshSocket& OutSocket) const;
)

//TslGame.WeaponMeshComponent.AttachedStaticComponentMap 
DefClass(UWeaponMeshComponent, USkeletalMeshComponent,
	MemberAtOffset(UNPACK(TMap<BYTE, ObjectPtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)
	,
	ObjectPtr<UStaticMeshComponent> GetStaticMeshComponentScopeType() const;
	float GetScopingAttachPointRelativeZ(FName ScopingAttachPoint) const;
)

//Engine.Skeleton.Sockets 
DefClass(USkeleton, UObject,
	MemberAtOffset(TArray<ObjectPtr<USkeletalMeshSocket>>, Sockets, 0x1a8)
	,
	bool FindSocketAndIndex(FName InSocketName, int32& OutIndex, USkeletalMeshSocket& Socket) const;
)

//TslGame.DroppedItem.Item 
DefClass(ADroppedItem, AActor,
	MemberAtOffset(EncryptedObjectPtr<UItem>, Item, 0x428)
,)

//TslGame.DroppedItemInteractionComponent.Item 
DefClass(UDroppedItemInteractionComponent, USceneComponent,
	MemberAtOffset(ObjectPtr<UItem>, Item, 0x660)
,)

//Function TslGame.Item.BP_GetItemID 
DefBaseClass(FItemTableRowBase,
	MemberAtOffset(FName, ItemID, 0x248)
,)

//Function TslGame.Item.BP_GetItemID 
DefClass(UItem, UObject,
	MemberAtOffset(ObjectPtr<FItemTableRowBase>, ItemTable, 0xB8)
	,
	FName GetItemID();
)

//TslGame.VehicleRiderComponent.SeatIndex 
//TslGame.VehicleRiderComponent.LastVehiclePawn 
DefClass(UVehicleRiderComponent, UActorComponent,
	MemberAtOffset(int, SeatIndex, 0x238)
	MemberAtOffset(ObjectPtr<APawn>, LastVehiclePawn, 0x270)
,)

//TslGame.WeaponProcessorComponent.EquippedWeapons 
//Function TslGame.WeaponProcessorComponent.GetWeaponIndex 
DefClass(UWeaponProcessorComponent, UActorComponent,
	MemberAtOffset(TArray<ObjectPtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
	MemberAtOffset(BYTE, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)
,)

//MemberAtOffset(UNPACK(TMap<FName, ObjectPtr<FWeaponAttachmentWeaponTagData>>), WeaponAttachmentWeaponTagDataMap, 0x150)
//Function TslGame.AttachableItem.GetAttachmentData 
DefClass(UAttachableItem, UItem,
	MemberAtOffset(ObjectPtr<FItemTableRowAttachment>, WeaponAttachmentData, 0x130)
,)

//TslGame.WeaponAttachmentData.ZeroingDistances 
//TslGame.WeaponAttachmentData.Name
//TslGame.WeaponAttachmentData.AttachmentSlotID
DefBaseClass(FWeaponAttachmentData,
	MemberAtOffsetZero(BYTE, AttachmentSlotID, 0x0)
	MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
	MemberAtOffset(FName, Name, 0x10)
,)

DefClass(FItemTableRowAttachment, FItemTableRowBase,
	MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x268)
,)

//TslGame.TslWeapon.AttachedItems 
//TslGame.TslWeapon.Mesh3P 
//TslGame.TslWeapon.WeaponTag 
//TslGame.TslWeapon.FiringAttachPoint 
//TslGame.TslWeapon.WeaponConfig + TslGame.WeaponData.IronSightZeroingDistances 
DefClass(ATslWeapon, AActor,
	MemberAtOffset(TArray<ObjectPtr<UAttachableItem>>, AttachedItems, 0x6e8)
	MemberAtOffset(EncryptedObjectPtr<UWeaponMeshComponent>, Mesh3P, 0x6a0)
	MemberAtOffset(FName, WeaponTag, 0x6f8)
	MemberAtOffset(FName, FiringAttachPoint, 0x740)
	MemberAtOffset(TArray<float>, WeaponConfig_IronSightZeroingDistances, 0x4e0 + 0x78)
,)

//TslGame.TslWeapon_Gun.CurrentZeroLevel 
//TslGame.TslWeapon_Gun.ScopingAttachPoint 
DefClass(ATslWeapon_Gun, ATslWeapon,
	MemberAtOffset(int, CurrentZeroLevel, 0x90c)
	MemberAtOffset(FName, ScopingAttachPoint, 0xad8)
,)

//TslGame.TslWeapon_Trajectory.WeaponTrajectoryData 
//TslGame.TslWeapon_Trajectory.TrajectoryGravityZ 
//Function TslGame.TslWeapon_Trajectory.GetBulletLocation 
DefClass(ATslWeapon_Trajectory, ATslWeapon_Gun,
	MemberAtOffset(ObjectPtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0xf08)
	MemberAtOffset(float, TrajectoryGravityZ, 0xe14)
	,
	float GetZeroingDistance() const;
)

DefClass(UAnimInstance, UObject,,)

//TslGame.TslAnimInstance.ControlRotation_CP 
//TslGame.TslAnimInstance.RecoilADSRotation_CP 
//TslGame.TslAnimInstance.LeanLeftAlpha_CP 
//TslGame.TslAnimInstance.LeanRightAlpha_CP 
//TslGame.TslAnimInstance.bIsScoping_CP 
DefClass(UTslAnimInstance, UAnimInstance,
	MemberAtOffset(FRotator, ControlRotation_CP, 0x75c)
	MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9c4)
	MemberAtOffset(float, LeanLeftAlpha_CP, 0xd7c)
	MemberAtOffset(float, LeanRightAlpha_CP, 0xd80)
	MemberAtOffset(BYTE, bIsScoping_CP, 0xc95)
,)

//TslGame.WeaponTrajectoryData.TrajectoryConfig 
DefClass(UWeaponTrajectoryData, UObject,
	MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)
,)

//TslGame.ItemPackage.Items 
DefClass(AItemPackage, AActor,
	MemberAtOffset(TArray<ObjectPtr<UItem>>, Items, 0x568)
,)

//TslGame.TslPlayerState.PlayerStatistics 
//TslGame.TslPlayerState.DamageDealtOnEnemy 
DefClass(ATslPlayerState, APlayerState,
	MemberAtOffset(float, DamageDealtOnEnemy, 0x78c)
	MemberAtOffset(int, PlayerStatistics_NumKills, 0x790 + 0x0)
,)

//Engine.CurveVector.FloatCurves 
DefClass(UCurveVector, UObject,
	MemberAtOffset(FRichCurve, FloatCurves, 0x48)
,)
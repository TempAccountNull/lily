class ATslLivingThing
//function ATslLivingThing.GetWorldTimeSeconds 0x4d97938

class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x230)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x438)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x458)

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x758)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4c8)
MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x540)

class UPlayerInput
//function UPlayerInput.SetMouseSensitivity 0x5b1f89c

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xb7c)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x1670 + 0x10 + 0x4)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x1670 + 0x10 + 0x14)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x1670 + 0x10 + 0x8)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x338)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x90)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x438)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x498)
MemberAtOffset(FVector, BaseTranslationOffset, 0x52c)

class ATslCharacter
MemberAtOffset(float, Health, 0xb08)
MemberAtOffset(float, HealthMax, 0x1a60)
MemberAtOffset(float, GroggyHealth, 0x1198)
MemberAtOffset(float, GroggyHealthMax, 0x1380)
MemberAtOffset(FString, CharacterName, 0x1050)
MemberAtOffset(int, LastTeamNum, 0x1258)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1b40)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x1a30)
MemberAtOffset(int, SpectatedCount, 0x22bc)
MemberAtOffset(FRotator, AimOffsets, 0x15d0)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xb08)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4f0)

class UActorComponent
//function UActorComponent.GetOwner 0xc7c094

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2e0)
MemberAtOffset(float, HealthMax, 0x2e4)
MemberAtOffset(float, Fuel, 0x2e8)
MemberAtOffset(float, FuelMax, 0x2ec)
MemberAtOffset(float, ExplosionTimer, 0x2f0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x5b134cc
MemberAtOffset(FVector, ComponentVelocity, 0x2c8)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x288)

class UPrimitiveComponent
MemberAtOffset(float, LastSubmitTime, 0x788)
MemberAtOffset(float, LastRenderTimeOnScreen, 0x790)

class USkinnedMeshComponent
MemberAtOffset(NativePtr<USkeletalMesh>, SkeletalMesh, 0xae8)

class USkeletalMeshComponent
MemberAtOffset(NativePtr<UAnimInstance>, AnimScriptInstance, 0xca0)

class UStaticMeshComponent
MemberAtOffset(NativePtr<UStaticMesh>, StaticMesh, 0xaf8)

class USkeletalMeshSocket
MemberAtOffset(FName, SocketName, 0x40)
MemberAtOffset(FName, BoneName, 0x48)
MemberAtOffset(FVector, RelativeLocation, 0x50)
MemberAtOffset(FRotator, RelativeRotation, 0x5c)
MemberAtOffset(FVector, RelativeScale, 0x68)

class UStaticMeshSocket
MemberAtOffset(FName, SocketName, 0x40)
MemberAtOffset(FVector, RelativeLocation, 0x48)
MemberAtOffset(FRotator, RelativeRotation, 0x54)
MemberAtOffset(FVector, RelativeScale, 0x60)

class USkeletalMesh
MemberAtOffset(NativePtr<USkeleton>, Skeleton, 0x60)
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x2e0)

class USkinnedMeshComponent
//function USkinnedMeshComponent.GetNumBones 0x5b45258

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<uint8_t,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11e8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x450)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x740)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x4cf99dc

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(uint8_t, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x4d51cfc

class ATslWeapon
MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x808)
MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x7c0)
MemberAtOffset(FName, WeaponTag, 0x818)
MemberAtOffset(FName, FiringAttachPoint, 0x860)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0xa2c)
MemberAtOffset(FName, ScopingAttachPoint, 0xc18)
MemberAtOffset(uint16_t, CurrentAmmoData, 0xa28)
//function ATslWeapon_Gun.GetCurrentScopeZeroingLevel 0x4d532c8

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xfa0)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x744)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9d4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xde4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xde8)
MemberAtOffset(uint8_t, bIsScoping_CP, 0xcfe)
MemberAtOffset(uint8_t, bLocalFPP_CP, 0xf28)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x580)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0xa58)
MemberAtOffset(int, PlayerStatistics_NumKills, 0x8e0 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

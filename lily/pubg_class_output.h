class ATslLivingThing
//function ATslLivingThing.GetWorldTimeSeconds 0x4d9a2ac

class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x7e8)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x448)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x420)

class UPlayerInput
//function UPlayerInput.SetMouseSensitivity 0x5b0cbbc

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x758)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4c8)
MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x540)

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xb6c)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x450 + 0x10 + 0x28)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x450 + 0x10 + 0x18)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x450 + 0x10 + 0x4)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x320)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x80)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x438)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x480)
MemberAtOffset(FVector, BaseTranslationOffset, 0x4a0)

class ATslCharacter
MemberAtOffset(float, Health, 0x2144)
MemberAtOffset(float, HealthMax, 0xb00)
MemberAtOffset(float, GroggyHealth, 0x1b98)
MemberAtOffset(float, GroggyHealthMax, 0x1344)
MemberAtOffset(FString, CharacterName, 0x1488)
MemberAtOffset(int, LastTeamNum, 0x1ae8)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1c30)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x1020)
MemberAtOffset(int, SpectatedCount, 0x146c)
MemberAtOffset(FRotator, AimOffsets, 0x1750)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xaf8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4e0)

class UActorComponent
//function UActorComponent.GetOwner 0xc55b5c

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2e0)
MemberAtOffset(float, HealthMax, 0x2e4)
MemberAtOffset(float, Fuel, 0x2e8)
MemberAtOffset(float, FuelMax, 0x2ec)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x5b007d4
MemberAtOffset(FVector, ComponentVelocity, 0x2e0)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x220)

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
//function USkinnedMeshComponent.GetNumBones 0x5b324cc

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<TEnumAsByte<EWeaponAttachmentSlotID>,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11e8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x448)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x740)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x4cfc32c

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(uint8_t, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x4d54610

class ATslWeapon
MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x7f8)
MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x7b0)
MemberAtOffset(FName, WeaponTag, 0x808)
MemberAtOffset(FName, FiringAttachPoint, 0x850)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0xa1c)
MemberAtOffset(FName, ScopingAttachPoint, 0xc08)
MemberAtOffset(uint16_t, CurrentAmmoData, 0xa18)
MemberAtOffset(TEnumAsByte<EWeaponState>, CurrentState, 0xa12)
//function ATslWeapon_Gun.GetCurrentScopeZeroingLevel 0x4d55bf4
//function ATslWeapon_Gun.WeaponCycleDone 0xea2308

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xfdc)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x744)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9d4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xde4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xde8)
MemberAtOffset(TEnumAsByte<bool>, bIsScoping_CP, 0xcfe)
MemberAtOffset(TEnumAsByte<bool>, bLocalFPP_CP, 0xf28)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x578)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x898)
MemberAtOffset(FTslPlayerStatistics, PlayerStatistics, 0x67c)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

class ATslLivingThing
//function ATslLivingThing.GetWorldTimeSeconds 0x4b3c7c0

class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x6d8)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x410)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x438)

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x740)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4b0)
MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x528)

class UPlayerInput
//function UPlayerInput.SetMouseSensitivity 0x58f23e8

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xb5c)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x1720 + 0x10 + 0x0)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x1720 + 0x10 + 0x598)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x1720 + 0x10 + 0x588)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x108)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x70)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x428)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x4d8)

class ATslCharacter
MemberAtOffset(float, Health, 0xac4)
MemberAtOffset(float, HealthMax, 0x13a0)
MemberAtOffset(float, GroggyHealth, 0x12c8)
MemberAtOffset(float, GroggyHealthMax, 0x1020)
MemberAtOffset(FString, CharacterName, 0x1040)
MemberAtOffset(int, LastTeamNum, 0x2108)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1b90)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x27f0)
MemberAtOffset(int, SpectatedCount, 0x1a10)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xae8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4d8)

class UActorComponent
//function UActorComponent.GetOwner 0xc28604

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2e0)
MemberAtOffset(float, HealthMax, 0x2e4)
MemberAtOffset(float, Fuel, 0x2e8)
MemberAtOffset(float, FuelMax, 0x2ec)
MemberAtOffset(float, ExplosionTimer, 0x2f0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x58e6458
MemberAtOffset(FVector, ComponentVelocity, 0x268)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x308)

class UPrimitiveComponent
MemberAtOffset(float, LastSubmitTime, 0x778)
MemberAtOffset(float, LastRenderTimeOnScreen, 0x780)

class USkinnedMeshComponent
MemberAtOffset(NativePtr<USkeletalMesh>, SkeletalMesh, 0xad8)

class USkeletalMeshComponent
MemberAtOffset(NativePtr<UAnimInstance>, AnimScriptInstance, 0xc90)

class UStaticMeshComponent
MemberAtOffset(NativePtr<UStaticMesh>, StaticMesh, 0xae8)

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
//function USkinnedMeshComponent.GetNumBones 0x5917df8

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<uint8_t,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x440)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x730)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x4a9f010

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(uint8_t, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x4af60d0

struct FWeaponAttachmentData
MemberAtOffsetZero(uint8_t, AttachmentSlotID, 0x0)
MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
MemberAtOffset(FName, Name, 0x10)

class UWeaponAttachmentDataAsset
MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x48)

struct FItemTableRowAttachment
MemberAtOffset(NativePtr<UWeaponAttachmentDataAsset>, WeaponAttachmentData, 0x268)

class ATslWeapon
MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x7f8)
MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x7b0)
MemberAtOffset(FName, WeaponTag, 0x808)
MemberAtOffset(FName, FiringAttachPoint, 0x850)
MemberAtOffset(TArray<float>, WeaponConfig_IronSightZeroingDistances, 0x4f0 + 0xc0)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0xa1c)
MemberAtOffset(FName, ScopingAttachPoint, 0xc08)
MemberAtOffset(uint16_t, CurrentAmmoData, 0xa18)

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xfc4)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x744)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9d4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xde4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xde8)
MemberAtOffset(uint8_t, bIsScoping_CP, 0xcfe)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x570)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x4a0)
MemberAtOffset(int, PlayerStatistics_NumKills, 0xa68 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

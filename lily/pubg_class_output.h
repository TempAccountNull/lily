class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x1a0)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x450)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x460)

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x768)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4d8)
MemberAtOffset(float, InputYawScale, 0x654)
MemberAtOffset(float, InputPitchScale, 0x658)
MemberAtOffset(float, InputRollScale, 0x65c)

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xafc)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0xa20 + 0x10 + 0x590)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0xa20 + 0x10 + 0x594)
MemberAtOffset(FVector, CameraCache_POV_Location, 0xa20 + 0x10 + 0x5a4)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x328)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x80)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x450)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x558)

class ATslCharacter
MemberAtOffset(float, Health, 0x13a4)
MemberAtOffset(float, HealthMax, 0x1a8c)
MemberAtOffset(float, GroggyHealth, 0x1c10)
MemberAtOffset(float, GroggyHealthMax, 0xe44)
MemberAtOffset(FString, CharacterName, 0x12e0)
MemberAtOffset(int, LastTeamNum, 0x1bf0)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1cc0)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x1478)
MemberAtOffset(int, SpectatedCount, 0x1c34)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xaf8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4e8)

class UActorComponent
//function UActorComponent.GetOwner 0x1d13df0

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2e0)
MemberAtOffset(float, HealthMax, 0x2e4)
MemberAtOffset(float, Fuel, 0x2e8)
MemberAtOffset(float, FuelMax, 0x2ec)
MemberAtOffset(float, ExplosionTimer, 0x2f0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x416a6d0
MemberAtOffset(FVector, ComponentVelocity, 0x37c)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x350)

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
//function USkinnedMeshComponent.GetNumBones 0x440b660

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<BYTE,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x458)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x730)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x1551d50

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(BYTE, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x1a93810

struct FWeaponAttachmentData
MemberAtOffsetZero(BYTE, AttachmentSlotID, 0x0)
MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
MemberAtOffset(FName, Name, 0x10)

struct FItemTableRowAttachment
MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x270)

class ATslWeapon
MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x808)
MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x7c0)
MemberAtOffset(FName, WeaponTag, 0x818)
MemberAtOffset(FName, FiringAttachPoint, 0x860)
MemberAtOffset(TArray<float>, WeaponConfig_IronSightZeroingDistances, 0x500 + 0xc0)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0xa2c)
MemberAtOffset(FName, ScopingAttachPoint, 0xc18)

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xf38)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x744)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9d4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xde4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xde8)
MemberAtOffset(BYTE, bIsScoping_CP, 0xcfe)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x588)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x980)
MemberAtOffset(int, PlayerStatistics_NumKills, 0x634 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

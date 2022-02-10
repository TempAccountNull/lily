class ATslLivingThing
//function ATslLivingThing.GetWorldTimeSeconds 0x21575f0

class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x850)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x460)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x450)

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x750)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4c0)
MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x538)

class UPlayerInput
//function UPlayerInput.SetMouseSensitivity 0x4398480

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xaec)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x1060 + 0x10 + 0x5ac)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x1060 + 0x10 + 0x0)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x1060 + 0x10 + 0x59c)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x3f8)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x80)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x448)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x530)

class ATslCharacter
MemberAtOffset(float, Health, 0xfc4)
MemberAtOffset(float, HealthMax, 0xf4c)
MemberAtOffset(float, GroggyHealth, 0xf6c)
MemberAtOffset(float, GroggyHealthMax, 0x101c)
MemberAtOffset(FString, CharacterName, 0x1940)
MemberAtOffset(int, LastTeamNum, 0x27d8)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1c40)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0xfb8)
MemberAtOffset(int, SpectatedCount, 0xf48)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xae8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4d8)

class UActorComponent
//function UActorComponent.GetOwner 0x43876c0

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2d0)
MemberAtOffset(float, HealthMax, 0x2d4)
MemberAtOffset(float, Fuel, 0x2d8)
MemberAtOffset(float, FuelMax, 0x2dc)
MemberAtOffset(float, ExplosionTimer, 0x2e0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x438cb50
MemberAtOffset(FVector, ComponentVelocity, 0x200)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x300)

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
MemberAtOffset(FName, SocketName, 0x38)
MemberAtOffset(FName, BoneName, 0x40)
MemberAtOffset(FVector, RelativeLocation, 0x48)
MemberAtOffset(FRotator, RelativeRotation, 0x54)
MemberAtOffset(FVector, RelativeScale, 0x60)

class UStaticMeshSocket
MemberAtOffset(FName, SocketName, 0x38)
MemberAtOffset(FVector, RelativeLocation, 0x40)
MemberAtOffset(FRotator, RelativeRotation, 0x4c)
MemberAtOffset(FVector, RelativeScale, 0x58)

class USkeletalMesh
MemberAtOffset(NativePtr<USkeleton>, Skeleton, 0x58)
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x2d8)

class USkinnedMeshComponent
//function USkinnedMeshComponent.GetNumBones 0x462d9c0

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd0)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<BYTE,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a0)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x440)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x730)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x174f7d0

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x228)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x260)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2b8)
MemberAtOffset(BYTE, WeaponArmInfo_RightWeaponIndex, 0x2d8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x1c91220

struct FWeaponAttachmentData
MemberAtOffsetZero(BYTE, AttachmentSlotID, 0x0)
MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
MemberAtOffset(FName, Name, 0x10)

struct FItemTableRowAttachment
MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x270)

class ATslWeapon
MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x7e8)
MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x7a0)
MemberAtOffset(FName, WeaponTag, 0x7f8)
MemberAtOffset(FName, FiringAttachPoint, 0x840)
MemberAtOffset(TArray<float>, WeaponConfig_IronSightZeroingDistances, 0x4e0 + 0xc0)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0xa0c)
MemberAtOffset(FName, ScopingAttachPoint, 0xbf8)
MemberAtOffset(uint16_t, CurrentAmmoData, 0xa08)

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0xff8)
MemberAtOffset(float, TrajectoryGravityZ, 0xfe4)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x73c)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9c4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xdd4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xdd8)
MemberAtOffset(BYTE, bIsScoping_CP, 0xcee)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x110)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x570)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x4c0)
MemberAtOffset(int, PlayerStatistics_NumKills, 0x770 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x40)

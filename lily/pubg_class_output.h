class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x870)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x418)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x438)

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x748)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4b8)

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xb5c)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x4a0 + 0x10 + 0x0)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x4a0 + 0x10 + 0x5a8)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x4a0 + 0x10 + 0x1c)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x1d8)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x68)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x428)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x538)

class ATslCharacter
MemberAtOffset(float, Health, 0x11d8)
MemberAtOffset(float, HealthMax, 0xe88)
MemberAtOffset(float, GroggyHealth, 0x100c)
MemberAtOffset(float, GroggyHealthMax, 0x1a68)
MemberAtOffset(FString, CharacterName, 0x2788)
MemberAtOffset(int, LastTeamNum, 0x1250)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1b10)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x2028)
MemberAtOffset(int, SpectatedCount, 0x1180)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xae8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4d0)

class UActorComponent
//function UActorComponent.GetOwner 0x1cd7180

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2e0)
MemberAtOffset(float, HealthMax, 0x2e4)
MemberAtOffset(float, Fuel, 0x2e8)
MemberAtOffset(float, FuelMax, 0x2ec)
MemberAtOffset(float, ExplosionTimer, 0x2f0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x413d200
MemberAtOffset(FVector, ComponentVelocity, 0x368)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x320)

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
//function USkinnedMeshComponent.GetNumBones 0x43de1a0

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<BYTE,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x440)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x730)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x15150e0

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(BYTE, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x1a56b40

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

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xf7c)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x744)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9d4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xde4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xde8)
MemberAtOffset(BYTE, bIsScoping_CP, 0xcfe)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x570)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x630)
MemberAtOffset(int, PlayerStatistics_NumKills, 0xa30 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

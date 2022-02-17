class ATslLivingThing
//function ATslLivingThing.GetWorldTimeSeconds 0x5567540

class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x200)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x450)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x408)

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x740)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4b0)
MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x528)

class UPlayerInput
//function UPlayerInput.SetMouseSensitivity 0x633a268

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xacc)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x450 + 0x10 + 0x5ac)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x450 + 0x10 + 0x10)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x450 + 0x10 + 0x4)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x2e8)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x88)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x430)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x468)

class ATslCharacter
MemberAtOffset(float, Health, 0x1000)
MemberAtOffset(float, HealthMax, 0x1bc8)
MemberAtOffset(float, GroggyHealth, 0xe14)
MemberAtOffset(float, GroggyHealthMax, 0x12dc)
MemberAtOffset(FString, CharacterName, 0x1328)
MemberAtOffset(int, LastTeamNum, 0x2170)
MemberAtOffset(NativePtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1c48)
MemberAtOffset(NativePtr<UWeaponProcessorComponent>, WeaponProcessor, 0x21d0)
MemberAtOffset(int, SpectatedCount, 0x27a4)

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xad8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4c8)

class UActorComponent
//function UActorComponent.GetOwner 0xcfc3e8

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2d0)
MemberAtOffset(float, HealthMax, 0x2d4)
MemberAtOffset(float, Fuel, 0x2d8)
MemberAtOffset(float, FuelMax, 0x2dc)
MemberAtOffset(float, ExplosionTimer, 0x2e0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x632e260
MemberAtOffset(FVector, ComponentVelocity, 0x370)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x218)

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
//function USkinnedMeshComponent.GetNumBones 0x635fc1c

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd0)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<uint8_t,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a0)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x430)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x730)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x54c99c8

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x228)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x260)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2b8)
MemberAtOffset(uint8_t, WeaponArmInfo_RightWeaponIndex, 0x2d8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x5520be0

struct FWeaponAttachmentData
MemberAtOffsetZero(EWeaponAttachmentSlotID, AttachmentSlotID, 0x0)
MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
MemberAtOffset(FName, Name, 0x10)

class UWeaponAttachmentDataAsset
MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x40)

struct FItemTableRowAttachment
MemberAtOffset(NativePtr<UWeaponAttachmentDataAsset>, WeaponAttachmentData, 0x268)

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
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xf34)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x73c)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9c4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xdd4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xdd8)
MemberAtOffset(uint8_t, bIsScoping_CP, 0xcee)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x110)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x560)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x4a8)
MemberAtOffset(int, PlayerStatistics_NumKills, 0xa48 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x40)

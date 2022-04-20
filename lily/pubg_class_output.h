class ATslLivingThing
//function ATslLivingThing.GetWorldTimeSeconds 0x4d8e2f0

class UWorld
MemberAtOffset(EncryptedPtr<ULevel>, CurrentLevel, 0x3f8)

class AController
MemberAtOffset(EncryptedPtr<ACharacter>, Character, 0x430)
MemberAtOffset(EncryptedPtr<APawn>, Pawn, 0x460)

class UPlayerInput
//function UPlayerInput.SetMouseSensitivity 0x5b01d60

class APlayerController
MemberAtOffset(NativePtr<APawn>, SpectatorPawn, 0x738)
MemberAtOffset(NativePtr<APlayerCameraManager>, PlayerCameraManager, 0x4a8)
MemberAtOffset(NativePtr<UPlayerInput>, PlayerInput, 0x520)

class ATslPlayerController
MemberAtOffset(float, DefaultFOV, 0xaec)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x4c0 + 0x10 + 0x24)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x4c0 + 0x10 + 0x4)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x4c0 + 0x10 + 0x5ac)

class AActor
MemberAtOffset(EncryptedPtr<USceneComponent>, RootComponent, 0x298)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x78)

class APawn
MemberAtOffset(EncryptedPtr<APlayerState>, PlayerState, 0x420)

class ACharacter
MemberAtOffset(NativePtr<USkeletalMeshComponent>, Mesh, 0x548)
MemberAtOffset(FVector, BaseTranslationOffset, 0x460)

class ATslCharacter
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

class ATslWheeledVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xad8)

class ATslFloatingVehicle
MemberAtOffset(NativePtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4c0)

class UActorComponent
//function UActorComponent.GetOwner 0xc045e0

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2d0)
MemberAtOffset(float, HealthMax, 0x2d4)
MemberAtOffset(float, Fuel, 0x2d8)
MemberAtOffset(float, FuelMax, 0x2dc)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x5af5b34
MemberAtOffset(FVector, ComponentVelocity, 0x200)
MemberAtOffset(NativePtr<USceneComponent>, AttachParent, 0x380)

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
//function USkinnedMeshComponent.GetNumBones 0x5b275f8

class UStaticMesh
MemberAtOffset(TArray<NativePtr<UStaticMeshSocket>>, Sockets, 0xd0)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<TEnumAsByte<EWeaponAttachmentSlotID>,NativePtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<NativePtr<USkeletalMeshSocket>>, Sockets, 0x1a0)

class ADroppedItem
MemberAtOffset(EncryptedPtr<UItem>, Item, 0x428)

class UDroppedItemInteractionComponent
MemberAtOffset(NativePtr<UItem>, Item, 0x730)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x4cf02c8

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x228)
MemberAtOffset(NativePtr<APawn>, LastVehiclePawn, 0x260)

class UWeaponProcessorComponent
MemberAtOffset(TArray<NativePtr<ATslWeapon>>, EquippedWeapons, 0x2b8)
MemberAtOffset(uint8_t, WeaponArmInfo_RightWeaponIndex, 0x2d8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x4d4861c

class ATslWeapon
MemberAtOffset(TArray<NativePtr<UAttachableItem>>, AttachedItems, 0x7d8)
MemberAtOffset(EncryptedPtr<UWeaponMeshComponent>, Mesh3P, 0x790)
MemberAtOffset(FName, WeaponTag, 0x7e8)
MemberAtOffset(FName, FiringAttachPoint, 0x830)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0x9fc)
MemberAtOffset(FName, ScopingAttachPoint, 0xbe8)
MemberAtOffset(uint16_t, CurrentAmmoData, 0x9f8)
MemberAtOffset(TEnumAsByte<EWeaponState>, CurrentState, 0x9f2)
//function ATslWeapon_Gun.GetCurrentScopeZeroingLevel 0x4d49c00
//function ATslWeapon_Gun.WeaponCycleDone 0xe3a6c4

class ATslWeapon_Trajectory
MemberAtOffset(NativePtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1008)
MemberAtOffset(float, TrajectoryGravityZ, 0xf4c)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x73c)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9c4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xdd4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xdd8)
MemberAtOffset(TEnumAsByte<bool>, bIsScoping_CP, 0xcee)
MemberAtOffset(TEnumAsByte<bool>, bLocalFPP_CP, 0xf18)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x110)

class AItemPackage
MemberAtOffset(TArray<NativePtr<UItem>>, Items, 0x558)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x8a8)
MemberAtOffset(FTslPlayerStatistics, PlayerStatistics, 0xa2c)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x40)

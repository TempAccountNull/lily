class UWorld
MemberAtOffset(EncryptedObjectPtr<ULevel>, CurrentLevel, 0x860)

class AController
MemberAtOffset(EncryptedObjectPtr<ACharacter>, Character, 0x438)
MemberAtOffset(EncryptedObjectPtr<APawn>, Pawn, 0x448)

class APlayerController
MemberAtOffset(ObjectPtr<APawn>, SpectatorPawn, 0x738)
MemberAtOffset(ObjectPtr<APlayerCameraManager>, PlayerCameraManager, 0x4a8)
MemberAtOffset(float, InputYawScale, 0x624)
MemberAtOffset(float, InputPitchScale, 0x628)
MemberAtOffset(float, InputRollScale, 0x62c)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0xa20 + 0x10 + 0x59c)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0xa20 + 0x10 + 0x590)
MemberAtOffset(FVector, CameraCache_POV_Location, 0xa20 + 0x10 + 0x5a8)

class AActor
MemberAtOffset(EncryptedObjectPtr<USceneComponent>, RootComponent, 0x350)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0xc0)

class APawn
MemberAtOffset(EncryptedObjectPtr<APlayerState>, PlayerState, 0x420)

class ACharacter
MemberAtOffset(ObjectPtr<USkeletalMeshComponent>, Mesh, 0x4f8)

class ATslCharacter
MemberAtOffset(float, Health, 0x1b84)
MemberAtOffset(float, HealthMax, 0x1078)
MemberAtOffset(float, GroggyHealth, 0x13f0)
MemberAtOffset(float, GroggyHealthMax, 0x1c10)
MemberAtOffset(FString, CharacterName, 0x10c0)
MemberAtOffset(int, LastTeamNum, 0xe38)
MemberAtOffset(ObjectPtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1ca0)
MemberAtOffset(ObjectPtr<UWeaponProcessorComponent>, WeaponProcessor, 0x1af0)
MemberAtOffset(int, SpectatedCount, 0x143c)

class ATslWheeledVehicle
MemberAtOffset(ObjectPtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xad8)

class ATslFloatingVehicle
MemberAtOffset(ObjectPtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4c8)

class UActorComponent
//function UActorComponent.GetOwner 0x1d2aae0

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2e0)
MemberAtOffset(float, HealthMax, 0x2e4)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x418c4e0
MemberAtOffset(FVector, ComponentVelocity, 0x370)
MemberAtOffset(ObjectPtr<USceneComponent>, AttachParent, 0x368)

class UPrimitiveComponent
MemberAtOffset(float, LastSubmitTime, 0x788)
MemberAtOffset(float, LastRenderTimeOnScreen, 0x790)

class USkinnedMeshComponent
MemberAtOffset(ObjectPtr<USkeletalMesh>, SkeletalMesh, 0xae8)

class USkeletalMeshComponent
MemberAtOffset(ObjectPtr<UAnimInstance>, AnimScriptInstance, 0xca0)

class UStaticMeshComponent
MemberAtOffset(ObjectPtr<UStaticMesh>, StaticMesh, 0xaf8)

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
MemberAtOffset(ObjectPtr<USkeleton>, Skeleton, 0x60)
MemberAtOffset(TArray<ObjectPtr<USkeletalMeshSocket>>, Sockets, 0x2e0)

class USkinnedMeshComponent
//function USkinnedMeshComponent.GetNumBones 0x442d640

class UStaticMesh
MemberAtOffset(TArray<ObjectPtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<BYTE,ObjectPtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11e8)

class USkeleton
MemberAtOffset(TArray<ObjectPtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedObjectPtr<UItem>, Item, 0x430)

class UDroppedItemInteractionComponent
MemberAtOffset(ObjectPtr<UItem>, Item, 0x740)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x15680d0

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(ObjectPtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<ObjectPtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(BYTE, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x1aaa740

struct FWeaponAttachmentData
MemberAtOffsetZero(BYTE, AttachmentSlotID, 0x0)
MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
MemberAtOffset(FName, Name, 0x10)

struct FItemTableRowAttachment
MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x270)

class ATslWeapon
MemberAtOffset(TArray<ObjectPtr<UAttachableItem>>, AttachedItems, 0x7d8)
MemberAtOffset(EncryptedObjectPtr<UWeaponMeshComponent>, Mesh3P, 0x790)
MemberAtOffset(FName, WeaponTag, 0x7e8)
MemberAtOffset(FName, FiringAttachPoint, 0x830)
MemberAtOffset(TArray<float>, WeaponConfig_IronSightZeroingDistances, 0x4d0 + 0xc0)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0x9fc)
MemberAtOffset(FName, ScopingAttachPoint, 0xbe8)

class ATslWeapon_Trajectory
MemberAtOffset(ObjectPtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0x1000)
MemberAtOffset(float, TrajectoryGravityZ, 0xfbc)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x744)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9d4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xde4)
MemberAtOffset(float, LeanRightAlpha_CP, 0xde8)
MemberAtOffset(BYTE, bIsScoping_CP, 0xcfe)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<ObjectPtr<UItem>>, Items, 0x560)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x698)
MemberAtOffset(int, PlayerStatistics_NumKills, 0x4b0 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

class UWorld
MemberAtOffset(EncryptedObjectPtr<ULevel>, CurrentLevel, 0x48)

class AController
MemberAtOffset(EncryptedObjectPtr<ACharacter>, Character, 0x468)
MemberAtOffset((EncryptedObjectPtr<APawn>, Pawn, 0x428)

class APlayerController
MemberAtOffset(ObjectPtr<APawn>, SpectatorPawn, 0x748)
MemberAtOffset(ObjectPtr<APlayerCameraManager>, PlayerCameraManager, 0x4b8)
MemberAtOffset(float, InputYawScale, 0x634)
MemberAtOffset(float, InputPitchScale, 0x638)
MemberAtOffset(float, InputRollScale, 0x63c)

class APlayerCameraManager
MemberAtOffset(float, CameraCache_POV_FOV, 0x430 + 0x10 + 0x5b0)
MemberAtOffset(FRotator, CameraCache_POV_Rotation, 0x430 + 0x10 + 0x24)
MemberAtOffset(FVector, CameraCache_POV_Location, 0x430 + 0x10 + 0x10)

class AActor
MemberAtOffset(EncryptedObjectPtr<USceneComponent>, RootComponent, 0x60)
MemberAtOffset(FRepMovement, ReplicatedMovement, 0x88)

class APawn
MemberAtOffset(EncryptedObjectPtr<APlayerState>, PlayerState, 0x438)

class ACharacter
MemberAtOffset(ObjectPtr<USkeletalMeshComponent>, Mesh, 0x4c0)

class UTslSettings
MemberAtOffset(float, BallisticDragScale, 0x3cc)
MemberAtOffset(float, BallisticDropScale, 0x3d0)
//function UTslSettings.GetTslSettings 0x1b4e080

class ATslCharacter
MemberAtOffset(float, Health, 0x20e0)
MemberAtOffset(float, HealthMax, 0x13ac)
MemberAtOffset(float, GroggyHealth, 0x13b8)
MemberAtOffset(float, GroggyHealthMax, 0xef4)
MemberAtOffset(FString, CharacterName, 0xef8)
MemberAtOffset(int, LastTeamNum, 0xe10)
MemberAtOffset(ObjectPtr<UVehicleRiderComponent>, VehicleRiderComponent, 0x1b10)
MemberAtOffset(ObjectPtr<UWeaponProcessorComponent>, WeaponProcessor, 0xf90)
MemberAtOffset(int, SpectatedCount, 0x1108)

class ATslWheeledVehicle
MemberAtOffset(ObjectPtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0xaa0)

class ATslFloatingVehicle
MemberAtOffset(ObjectPtr<UTslVehicleCommonComponent>, VehicleCommonComponent, 0x4b8)

class UActorComponent
//function UActorComponent.GetOwner 0x1dc77e0

class UTslVehicleCommonComponent
MemberAtOffset(float, Health, 0x2dc)
MemberAtOffset(float, HealthMax, 0x2e0)

class USceneComponent
//function USceneComponent.K2_GetComponentToWorld 0x41e0e90
MemberAtOffset(FVector, ComponentVelocity, 0x210)
MemberAtOffset(ObjectPtr<USceneComponent>, AttachParent, 0x258)

class UPrimitiveComponent
MemberAtOffset(float, LastSubmitTime, 0x778)
MemberAtOffset(float, LastRenderTimeOnScreen, 0x780)

class USkinnedMeshComponent
MemberAtOffset(ObjectPtr<USkeletalMesh>, SkeletalMesh, 0xad8)

class USkeletalMeshComponent
MemberAtOffset(ObjectPtr<UAnimInstance>, AnimScriptInstance, 0xc90)

class UStaticMeshComponent
MemberAtOffset(ObjectPtr<UStaticMesh>, StaticMesh, 0xae8)

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
//function USkinnedMeshComponent.GetNumBones 0x44819a0

class UStaticMesh
MemberAtOffset(TArray<ObjectPtr<UStaticMeshSocket>>, Sockets, 0xd8)

class UWeaponMeshComponent
MemberAtOffset(UNPACK(TMap<BYTE,ObjectPtr<UStaticMeshComponent>>), AttachedStaticComponentMap, 0x11d8)

class USkeleton
MemberAtOffset(TArray<ObjectPtr<USkeletalMeshSocket>>, Sockets, 0x1a8)

class ADroppedItem
MemberAtOffset(EncryptedObjectPtr<UItem>, Item, 0x428)

class UDroppedItemInteractionComponent
MemberAtOffset(ObjectPtr<UItem>, Item, 0x660)

struct FItemTableRowBase
MemberAtOffset(FName, ItemID, 0x248)

class UItem
//function UItem.BP_GetItemID 0x16153d0

class UVehicleRiderComponent
MemberAtOffset(int, SeatIndex, 0x238)
MemberAtOffset(ObjectPtr<APawn>, LastVehiclePawn, 0x270)

class UWeaponProcessorComponent
MemberAtOffset(TArray<ObjectPtr<ATslWeapon>>, EquippedWeapons, 0x2c8)
MemberAtOffset(BYTE, WeaponArmInfo_RightWeaponIndex, 0x2e8 + 0x1)

class UAttachableItem
//function UAttachableItem.GetAttachmentData 0x1b47fa0

struct FWeaponAttachmentData
MemberAtOffsetZero(BYTE, AttachmentSlotID, 0x0)
MemberAtOffset(TArray<float>, ZeroingDistances, 0x50)
MemberAtOffset(FName, Name, 0x10)

struct FItemTableRowAttachment
MemberAtOffset(FWeaponAttachmentData, AttachmentData, 0x268)

class ATslWeapon
MemberAtOffset(TArray<ObjectPtr<UAttachableItem>>, AttachedItems, 0x6e8)
MemberAtOffset(EncryptedObjectPtr<UWeaponMeshComponent>, Mesh3P, 0x6a0)
MemberAtOffset(FName, WeaponTag, 0x6f8)
MemberAtOffset(FName, FiringAttachPoint, 0x740)
MemberAtOffset(TArray<float>, WeaponConfig_IronSightZeroingDistances, 0x4e0 + 0x78)

class ATslWeapon_Gun
MemberAtOffset(int, CurrentZeroLevel, 0x90c)
MemberAtOffset(FName, ScopingAttachPoint, 0xad8)

class ATslWeapon_Trajectory
MemberAtOffset(ObjectPtr<UWeaponTrajectoryData>, WeaponTrajectoryData, 0xf08)
MemberAtOffset(float, TrajectoryGravityZ, 0xe14)

class UTslAnimInstance
MemberAtOffset(FRotator, ControlRotation_CP, 0x75c)
MemberAtOffset(FRotator, RecoilADSRotation_CP, 0x9c4)
MemberAtOffset(float, LeanLeftAlpha_CP, 0xd7c)
MemberAtOffset(float, LeanRightAlpha_CP, 0xd80)
MemberAtOffset(BYTE, bIsScoping_CP, 0xc95)

class UWeaponTrajectoryData
MemberAtOffset(FWeaponTrajectoryConfig, TrajectoryConfig, 0x118)

class AItemPackage
MemberAtOffset(TArray<ObjectPtr<UItem>>, Items, 0x568)

class ATslPlayerState
MemberAtOffset(float, DamageDealtOnEnemy, 0x78c)
MemberAtOffset(int, PlayerStatistics_NumKills, 0x790 + 0x0)

class UCurveVector
MemberAtOffset(FRichCurve, FloatCurves, 0x48)

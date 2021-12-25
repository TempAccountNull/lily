class UWorld
EncryptedObjectPtr<ULevel> CurrentLevel

class AController
EncryptedObjectPtr<ACharacter> Character
EncryptedObjectPtr<APawn> Pawn

class APlayerController
ObjectPtr<APawn> SpectatorPawn
ObjectPtr<APlayerCameraManager> PlayerCameraManager
float InputYawScale
float InputPitchScale
float InputRollScale

class APlayerCameraManager
float CameraCache + CameraCacheEntry.POV + MinimalViewInfo.FOV
FRotator CameraCache + CameraCacheEntry.POV + MinimalViewInfo.Rotation
FVector CameraCache + CameraCacheEntry.POV + MinimalViewInfo.Location

class AActor
EncryptedObjectPtr<USceneComponent> RootComponent
FRepMovement ReplicatedMovement

class APawn
EncryptedObjectPtr<APlayerState> PlayerState

class ACharacter
ObjectPtr<USkeletalMeshComponent> Mesh

class ATslCharacter
float Health
float HealthMax
float GroggyHealth
float GroggyHealthMax
FString CharacterName
int LastTeamNum
ObjectPtr<UVehicleRiderComponent> VehicleRiderComponent
ObjectPtr<UWeaponProcessorComponent> WeaponProcessor
int SpectatedCount

class ATslWheeledVehicle
ObjectPtr<UTslVehicleCommonComponent> VehicleCommonComponent

class ATslFloatingVehicle
ObjectPtr<UTslVehicleCommonComponent> VehicleCommonComponent

class UActorComponent
function GetOwner

class UTslVehicleCommonComponent
float Health
float HealthMax

class USceneComponent
function K2_GetComponentToWorld
FVector ComponentVelocity
ObjectPtr<USceneComponent> AttachParent

class UPrimitiveComponent
float LastSubmitTime
float LastRenderTimeOnScreen

class USkinnedMeshComponent
ObjectPtr<USkeletalMesh> SkeletalMesh

class USkeletalMeshComponent
ObjectPtr<UAnimInstance> AnimScriptInstance

class UStaticMeshComponent
ObjectPtr<UStaticMesh> StaticMesh

class USkeletalMeshSocket
FName SocketName
FName BoneName
FVector RelativeLocation
FRotator RelativeRotation
FVector RelativeScale

class UStaticMeshSocket
FName SocketName
FVector RelativeLocation
FRotator RelativeRotation
FVector RelativeScale

class USkeletalMesh
ObjectPtr<USkeleton> Skeleton
TArray<ObjectPtr<USkeletalMeshSocket>> Sockets

class USkinnedMeshComponent
function GetNumBones

class UStaticMesh
TArray<ObjectPtr<UStaticMeshSocket>> Sockets

class UWeaponMeshComponent
UNPACK(TMap<BYTE,ObjectPtr<UStaticMeshComponent>>) AttachedStaticComponentMap

class USkeleton
TArray<ObjectPtr<USkeletalMeshSocket>> Sockets

class ADroppedItem
EncryptedObjectPtr<UItem> Item

class UDroppedItemInteractionComponent
ObjectPtr<UItem> Item

struct ItemTableRowBase
FName ItemID

class UItem
function BP_GetItemID

class UVehicleRiderComponent
int SeatIndex
ObjectPtr<APawn> LastVehiclePawn

class UWeaponProcessorComponent
TArray<ObjectPtr<ATslWeapon>> EquippedWeapons
BYTE WeaponArmInfo + WeaponArmInfo.RightWeaponIndex

class UAttachableItem
function GetAttachmentData

struct WeaponAttachmentData
BYTE AttachmentSlotID
TArray<float> ZeroingDistances
FName Name

struct ItemTableRowAttachment
FWeaponAttachmentData AttachmentData

class ATslWeapon
TArray<ObjectPtr<UAttachableItem>> AttachedItems
EncryptedObjectPtr<UWeaponMeshComponent> Mesh3P
FName WeaponTag
FName FiringAttachPoint
TArray<float> WeaponConfig + WeaponData.IronSightZeroingDistances

class ATslWeapon_Gun
int CurrentZeroLevel
FName ScopingAttachPoint

class ATslWeapon_Trajectory
ObjectPtr<UWeaponTrajectoryData> WeaponTrajectoryData
float TrajectoryGravityZ

class UTslAnimInstance
FRotator ControlRotation_CP
FRotator RecoilADSRotation_CP
float LeanLeftAlpha_CP
float LeanRightAlpha_CP
BYTE bIsScoping_CP

class UWeaponTrajectoryData
FWeaponTrajectoryConfig TrajectoryConfig

class AItemPackage
TArray<ObjectPtr<UItem>> Items

class ATslPlayerState
float DamageDealtOnEnemy
int PlayerStatistics + TslPlayerStatistics.NumKills

class UCurveVector
FRichCurve FloatCurves
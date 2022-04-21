#pragma once
#include <windows.h>
#include <vector>

#include "common/ida_defs.h"
#include "common/defclass.h"
#include "common/process.h"

#include "ue4math/vector.h"
#include "ue4math/rotator.h"
#include "ue4math/quat.h"
#include "ue4math/transform.h"
#include "ue4math/matrix.h"

#include "pubg_process.h"

template<class T>
class TArray {
protected:
	constexpr static size_t ARRAY_MAX = 0x2000;
	uintptr_t Data = 0;
	uint32_t Count = 0;
	uint32_t Max = 0;
public:
	operator uintptr_t() { return Data; }

	size_t GetCount(size_t MaxSize = ARRAY_MAX) const {
		return Count < MaxSize ? Count : MaxSize;
	}

	bool GetValue(int i, T& value) const {
		if (i < 0 || i >= GetCount(ARRAY_MAX)) return false;
		return g_Pubg->Read(Data + sizeof(T) * i, &value);
	}

	bool SetValue(int i, const T& value) {
		if (i < 0 || i >= GetCount(ARRAY_MAX)) return false;
		return g_Pubg->Write(Data + sizeof(T) * i, &value);
	}

	bool GetValues(T& value, size_t MaxSize = ARRAY_MAX) const {
		return g_Pubg->ReadProcessMemory(Data, &value, sizeof(T) * GetCount(MaxSize));
	}

	bool SetValues(const T& value, size_t MaxSize = ARRAY_MAX) const {
		return g_Pubg->WriteProcessMemory(Data, &value, sizeof(T) * GetCount(MaxSize));
	}

	std::vector<T> GetVector(size_t MaxSize = ARRAY_MAX) const {
		std::vector<T> v(GetCount(MaxSize));
		if (!GetValues(v[0], MaxSize))
			v.clear();

		return v;
	}

	T operator [](int i) const {
		T Value;
		if (!GetValue(i, Value))
			ZeroMemory(&Value, sizeof(Value));
		return Value;
	}
};

struct FString : public TArray<wchar_t> {};

template<class T>
struct TSetElement {
	T Value;
	int HashNextId;
	int HashIndex;
};

template<class T>
class TSet : public TArray<TSetElement<T>> {};

template<class KeyType, class ValueType>
struct TPair {
	KeyType Key;
	ValueType Value;
};

template<class KeyType, class ValueType>
class TMap : public TSet<TPair<KeyType, ValueType>> {
public:
	bool GetValue(const KeyType& Key, ValueType& Value) const {
		for (const auto& Elem : this->GetVector()) {
			if (Elem.Value.Key == Key) {
				Value = Elem.Value.Value;
				return true;
			}
		}
		return false;
	}
};

template <class ObjectType, class PtrType>
class ObjectPtr {
private:
	PtrType P;
public:
	ObjectPtr(uintptr_t P = 0) : P(P) {}

	operator uintptr_t() const { return P; }

	template <class Type, class = std::enable_if_t<std::is_same_v<ObjectType, Type>, Type>>
	bool Read(Type& Obj) const { return g_Pubg->Read(P, &Obj); }

	template <class Type, class = std::enable_if_t<std::is_same_v<ObjectType, Type>, Type>>
	bool Write(Type& Obj) const { return g_Pubg->Write(P, &Obj); }

	template<class Type, class = std::enable_if_t<std::is_base_of_v<ObjectType, Type>, Type>>
	bool ReadOtherType(Type& Obj) const { return g_Pubg->Read(P, &Obj); }

	template<class DataType>
	bool WriteAtOffset(const DataType& Data, size_t Offset) const { return g_Pubg->Write(P + Offset, &Data); }

	unsigned GetHash() const {
		using UObject = typename ObjectType::UObject;
		UObject Obj;
		return NativePtr<UObject>(P).Read(Obj) ? Obj.GetFName().GetHash() : 0;
	}
};

template <class ObjectType>
using NativePtr = ObjectPtr<ObjectType, uintptr_t>;

template <class ObjectType>
using EncryptedPtr = ObjectPtr<ObjectType, XenuinePtr>;

template<class TEnum>
class TEnumAsByte
{
public:
	typedef TEnum EnumType;
	TEnumAsByte() = default;
	TEnumAsByte(const TEnumAsByte&) = default;
	TEnumAsByte& operator=(const TEnumAsByte&) = default;
	FORCEINLINE TEnumAsByte(TEnum InValue) : Value(static_cast<uint8>(InValue)) {}
	explicit FORCEINLINE TEnumAsByte(int32 InValue) : Value(static_cast<uint8>(InValue)) {}
	explicit FORCEINLINE TEnumAsByte(uint8 InValue) : Value(InValue) {}
	bool operator==(TEnum InValue) const { return static_cast<TEnum>(Value) == InValue; }
	bool operator==(TEnumAsByte InValue) const { return Value == InValue.Value; }
	operator TEnum() const { return (TEnum)Value; }
	TEnum GetValue() const { return (TEnum)Value; }
private:
	uint8 Value;
};

// ScriptStruct CoreUObject.Vector2D
// 0x0008
struct FVector2D
{
	float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

struct FName
{
	int ComparisonIndex = 0;
	int Number = 0;
	bool operator == (const FName& rhs) const { return ComparisonIndex == rhs.ComparisonIndex && Number == rhs.Number; }
	bool operator != (const FName& rhs) const { return !(*this == rhs); }
	bool GetName(char* szBuf, size_t SizeMax) const { return g_Pubg->NameArr.GetName(*this, szBuf, SizeMax); }
	unsigned GetHash() const {
		char szBuf[TNameEntryArray::NAME_SIZE];
		return GetName(szBuf, sizeof(szBuf)) ? CompileTime::StrHash(szBuf) : 0;
	}
	std::string GetName() const {
		char szBuf[TNameEntryArray::NAME_SIZE];
		if (!GetName(szBuf, sizeof(szBuf)))
			return {};
		return szBuf;
	}

	constexpr FName(int ComparisonIndex = 0, int Number = 0) : ComparisonIndex(ComparisonIndex), Number(Number) {}
	FName(const char* szName) { *this = g_Pubg->NameArr.FindName(szName); }
};

struct FMeshBoneInfo
{
	FName Name;
	int32 ParentIndex;
	int32 pad;
};

// ScriptStruct CoreUObject.BoxSphereBounds
// 0x001C
struct FBoxSphereBounds
{
	struct FVector                                     Origin;                                                   // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FVector                                     BoxExtent;                                                // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              SphereRadius;                                             // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

// ScriptStruct Engine.RepMovement
// 0x0034
struct FRepMovement
{
	struct FVector                                     LinearVelocity;                                           // 0x0000(0x000C) (CPF_Transient, CPF_IsPlainOldData)
	struct FVector                                     AngularVelocity;                                          // 0x000C(0x000C) (CPF_Transient, CPF_IsPlainOldData)
	struct FVector                                     Location;                                                 // 0x0018(0x000C) (CPF_Transient, CPF_IsPlainOldData)
	struct FRotator                                    Rotation;                                                 // 0x0024(0x000C) (CPF_Transient, CPF_IsPlainOldData)
	unsigned char                                      bSimulatedPhysicSleep : 1;                                // 0x0030(0x0001) (CPF_Transient)
	unsigned char                                      bRepPhysics : 1;                                          // 0x0030(0x0001) (CPF_Transient)
	unsigned char                                      UnknownData00[0x1];                                       // 0x0031(0x0001) UNKNOWN PROPERTY: EnumProperty Engine.RepMovement.LocationQuantizationLevel
	unsigned char                                      UnknownData01[0x1];                                       // 0x0032(0x0001) UNKNOWN PROPERTY: EnumProperty Engine.RepMovement.VelocityQuantizationLevel
	unsigned char                                      UnknownData02[0x1];                                       // 0x0033(0x0001) UNKNOWN PROPERTY: EnumProperty Engine.RepMovement.RotationQuantizationLevel
};

class UCurveVector;

// ScriptStruct TslGame.WeaponTrajectoryConfig
// 0x0048
struct FWeaponTrajectoryConfig
{
	float                                              InitialSpeed;                                             // 0x0000(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	int                                                HitDamage;                                                // 0x0004(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VehicleDamageScalar;                                      // 0x0008(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              LowerClampDamage;                                         // 0x000C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	NativePtr<UCurveVector>                            BallisticCurve;                                           // 0x0010(0x0008) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              RangeModifier;                                            // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              ReferenceDistance;                                        // 0x001C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              TravelDistanceMax;                                        // 0x0020(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      bUseMaxDamageDistance : 1;                                // 0x0024(0x0001) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0025(0x0003) MISSED OFFSET
	float                                              SimulationSubstepTime;                                    // 0x0028(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              VDragCoefficient;                                         // 0x002C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	float                                              BDS;                                                      // 0x0030(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      bUseAdvancedBallistics : 1;                               // 0x0034(0x0001) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      bCanProduceCrackSound : 1;                                // 0x0035(0x0001) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      IsPenetrable : 1;                                         // 0x0036(0x0001) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      UnknownData01[0x1];                                       // 0x0037(0x0001) MISSED OFFSET
	uintptr_t                                          DamageType;                                               // 0x0038(0x0008) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
	unsigned char                                      bIsTrajectoryReplicationEnabled : 1;                      // 0x0040(0x0001) (CPF_Edit, CPF_DisableEditOnInstance)
	unsigned char                                      UnknownData02[0x3];                                       // 0x0041(0x0003) MISSED OFFSET
	float                                              WeaponSpread;                                             // 0x0044(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_BlueprintReadOnly, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
};

enum ERichCurveInterpMode
{
	RCIM_Linear,
	RCIM_Constant,
	RCIM_Cubic,
	RCIM_None,
};

enum ERichCurveTangentMode
{
	RCTM_Auto,
	RCTM_User,
	RCTM_Break,
	RCTM_None,
};

enum ERichCurveTangentWeightMode
{
	RCTWM_WeightedNone,
	RCTWM_WeightedArrive,
	RCTWM_WeightedLeave,
	RCTWM_WeightedBoth,
};

enum ERichCurveExtrapolation
{
	RCCE_Cycle,
	RCCE_CycleWithOffset,
	RCCE_Oscillate,
	RCCE_Linear,
	RCCE_Constant,
	RCCE_None,
};

enum ERelativeTransformSpace
{
	RTS_World,
	RTS_Actor,
	RTS_Component,
	RTS_ParentBoneSpace,
};

enum EWeaponAttachmentSlotID {
	None,
	Muzzle,
	LowerRail,
	UpperRail,
	Magazine,
	Stock,
	Angled
};

// Enum TslGame.EWeaponState
enum class EWeaponState
{
	EWeaponState__Idle = 0,
	EWeaponState__Firing = 1,
	EWeaponState__Reloading = 2,
	EWeaponState__EWeaponState_MAX = 3
};

enum class ESight
{
	Iron,
	RedDot,
	Holo,
	X2,
	X3,
	X4,
	X6,
	X8,
	X15
};

// ScriptStruct Engine.KeyHandleMap
// 0x0050
struct FKeyHandleMap
{
	unsigned char                                      UnknownData00[0x50];                                      // 0x0000(0x0050) MISSED OFFSET
};

// ScriptStruct Engine.IndexedCurve
// 0x0058
struct FIndexedCurve
{
	unsigned char                                      UnknownData00[0x8];                                       // 0x0000(0x0008) MISSED OFFSET
	struct FKeyHandleMap                               KeyHandlesToIndices;                                      // 0x0008(0x0050) (CPF_Transient)
};

// ScriptStruct Engine.RichCurveKey
// 0x001C
struct FRichCurveKey
{
	uint8_t                                            InterpMode;                                               // 0x0000(0x0001) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	uint8_t                                            TangentMode;                                              // 0x0001(0x0001) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	uint8_t                                            TangentWeightMode;                                        // 0x0002(0x0001) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0x1];                                       // 0x0003(0x0001) MISSED OFFSET
	float                                              Time;                                                     // 0x0004(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              Value;                                                    // 0x0008(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              ArriveTangent;                                            // 0x000C(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              ArriveTangentWeight;                                      // 0x0010(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              LeaveTangent;                                             // 0x0014(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              LeaveTangentWeight;                                       // 0x0018(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
};

class UCurveFloat;

// ScriptStruct Engine.InputAxisProperties
// 0x0020
struct FInputAxisProperties
{
	float                                              DEADZONE;                                                 // 0x0000(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OuterDeadZone;                                            // 0x0004(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              Sensitivity;                                              // 0x0008(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              Exponent;                                                 // 0x000C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      bInvert : 1;                                              // 0x0010(0x0001) (CPF_Edit)
	unsigned char                                      UnknownData00[0x7];                                       // 0x0011(0x0007) MISSED OFFSET
	//NativePtr<UCurveFloat>                           MultiplierAxisValueCurve;	                             // 0x0018(0x0008) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
};

static_assert(sizeof(FInputAxisProperties) == 0x18);

static bool IsItNotWeighted(const FRichCurveKey& Key1, const FRichCurveKey& Key2);
static float WeightedEvalForTwoKeys(
	float Key1Value, float Key1Time, float Key1LeaveTangent, float Key1LeaveTangentWeight, ERichCurveTangentWeightMode Key1TangentWeightMode,
	float Key2Value, float Key2Time, float Key2ArriveTangent, float Key2ArriveTangentWeight, ERichCurveTangentWeightMode Key2TangentWeightMode,
	float InTime);

static void CycleTime(float MinTime, float MaxTime, float& InTime, int& CycleCount);
static float EvalForTwoKeys(const FRichCurveKey& Key1, const FRichCurveKey& Key2, const float InTime);

// ScriptStruct Engine.RichCurve
// 0x0018 (0x0070 - 0x0058)
struct FRichCurve : public FIndexedCurve
{
	uint8_t                                            PreInfinityExtrap;                                        // 0x0058(0x0001) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	uint8_t                                            PostInfinityExtrap;                                       // 0x0059(0x0001) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0x2];                                       // 0x005A(0x0002) MISSED OFFSET
	float                                              DefaultValue;                                             // 0x005C(0x0004) (CPF_Edit, CPF_ZeroConstructor, CPF_IsPlainOldData)
	TArray<struct FRichCurveKey>                       Keys;                                                     // 0x0060(0x0010) (CPF_Edit, CPF_EditFixedSize, CPF_ZeroConstructor)

	void RemapTimeValue(float& InTime, float& CycleValueOffset) const;
	float Eval(float InTime, float InDefaultValue) const;
};

// ScriptStruct TslGame.TslPlayerStatistics
// 0x0008
struct FTslPlayerStatistics
{
	int                                                NumKills;                                                 // 0x0000(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	int                                                NumAssists;                                               // 0x0004(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
};
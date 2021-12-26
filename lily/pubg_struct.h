#pragma once

#include <windows.h>
#include <vector>

#include "ida_defs.h"
#include "process.h"
#include "mymath.h"
#include "xenuine.h"

#define ARRAY_MAX 0x2000
#define INDEX_NONE -1
#define NAME_None FName(0, 0)

#define UNPACK(...) __VA_ARGS__

#define MemberAtOffset(type, name, offset) \
static_assert(offset > BaseOffset, "Offset must be greater than sizeof(Derived Class)");\
struct{class _zpad##name{uint8_t pad[offset-BaseOffset];}_zpad##name; type name;};

#define MemberAtOffsetZero(type, name, offset) \
static_assert(offset == BaseOffset, "Offset must be greater than sizeof(Derived Class)");\
struct{type name;};

#define DefClass(structname, derivedclass, members, ...) \
class structname : public derivedclass { \
	private:\
	static constexpr unsigned AlignNeeded = 4; \
	static constexpr unsigned BaseOffset = sizeof(derivedclass); \
	static_assert(sizeof(derivedclass) % AlignNeeded == 0, "DerivedClass is not aligned");\
	public: \
	structname(){} \
	union {members; uint8_t _zpad[AlignNeeded];}; \
	__VA_ARGS__ \
};

#define DefBaseClass(structname, members, ...) \
class structname { \
	private:\
	static constexpr unsigned AlignNeeded = 4; \
	static constexpr unsigned BaseOffset = 0; \
	public: \
	structname(){} \
	union {members; uint8_t _zpad[AlignNeeded];}; \
	__VA_ARGS__ \
};

template<class T>
class TArray {
protected:
	uintptr_t Data = 0;
	uint32_t Count = 0;
	uint32_t Max = 0;
public:
	operator uintptr_t() { return Data; }

	size_t GetCount(size_t MaxSize = ARRAY_MAX) const {
		return Count < MaxSize ? Count : MaxSize;
	}

	bool SetValue(int i, const T& value) {
		if (i < 0 || i >= GetCount(ARRAY_MAX)) return false;
		return gXenuine->process.SetValue(Data + sizeof(T) * i, &value);
	}

	bool GetValue(int i, T& value) const {
		if (i < 0 || i >= GetCount(ARRAY_MAX)) return false;
		return gXenuine->process.GetValue(Data + sizeof(T) * i, &value);
	}

	bool GetValues(T& value, size_t MaxSize = ARRAY_MAX) const {
		return gXenuine->process.GetValueWithSize(Data, &value, sizeof(T) * GetCount(MaxSize));
	}

	bool SetValues(const T& value, size_t MaxSize = ARRAY_MAX) const {
		return gXenuine->process.SetValueWithSize(Data, &value, sizeof(T) * GetCount(MaxSize));
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
class TMap : public TSet<TPair<KeyType, ValueType>> {};


template<class T>
class ObjectPtr {
private:
	uintptr_t P;
public:
	ObjectPtr(uintptr_t P = 0) : P(P) {}

	operator uintptr_t() const { return P; }
	bool Read(T& Obj) const { return gXenuine->process.GetValue(P, &Obj); }

	//Do not call Read() with other types (including inherited class)
	template<class T2>
	bool Read(T2& Obj) const { return 1 / 0; }

	template<class T2>
	bool ReadOtherType(T2& Obj) const { return gXenuine->process.GetValue(P, &Obj); }

	bool Write(T& Obj) const { return gXenuine->process.SetValue(P, &Obj); }

	template<class T2>
	bool WriteAtOffset(T2& Obj, size_t Offset) const { return gXenuine->process.SetValue(P + Offset, &Obj); }
};

template<class T>
class EncryptedObjectPtr {
private:
	XenuinePtr P;
public:
	EncryptedObjectPtr(uintptr_t P = 0) : P(P) {}

	operator uintptr_t() const { return P; }
	bool Read(T& Obj) const { return gXenuine->process.GetValue(P, &Obj); }

	//Do not call Read() with other types (including inherited class)
	template<class T2>
	bool Read(T2& Obj) const { return 1 / 0; }

	template<class T2>
	bool ReadOtherType(T2& Obj) const { return gXenuine->process.GetValue(P, &Obj); }

	bool Write(T& Obj) const { return gXenuine->process.SetValue(P, &Obj); }

	template<class T2>
	bool WriteAtOffset(T2& Obj, size_t Offset) const { return gXenuine->process.SetValue(P + Offset, &Obj); }
};

// ScriptStruct CoreUObject.Vector
// 0x000C
struct FVector
{
	float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

// ScriptStruct CoreUObject.Rotator
// 0x000C
struct FRotator
{
	float                                              Pitch;                                                    // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Yaw;                                                      // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Roll;                                                     // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

// ScriptStruct CoreUObject.Quat
// 0x0010
struct alignas(16) FQuat
{
	float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              W;                                                        // 0x000C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

// ScriptStruct CoreUObject.Vector2D
// 0x0008
struct FVector2D
{
	float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

// ScriptStruct CoreUObject.Transform
// 0x0030
struct alignas(16) FTransform
{
	struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FVector                                     Translation;                                              // 0x0010(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
	struct FVector                                     Scale3D;                                                  // 0x0020(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
};

struct FName
{
	int ComparisonIndex;
	int Number;
	bool operator == (const FName& Name2) {
		return ComparisonIndex == Name2.ComparisonIndex && Number == Name2.Number;
	}
	bool operator != (const FName& Name2) {
		return !(*this == Name2);
	}
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
	ObjectPtr<UCurveVector>                            BallisticCurve;                                           // 0x0010(0x0008) (CPF_Edit, CPF_ZeroConstructor, CPF_DisableEditOnInstance, CPF_IsPlainOldData)
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
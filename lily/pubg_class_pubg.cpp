#include "pubg_class.h"

void SimulateWeaponTrajectory(Vector Direction, float Distance, float TrajectoryGravityZ, float BallisticDragScale, float BallisticDropScale, 
	float BDS, float SimulationSubstepTime, float VDragCoefficient, const FRichCurve& FloatCurve, float& TravelTime, float& BulletDrop) 
{
	TravelTime = 0.0;
	BulletDrop = 0.0;

	float TravelDistance = 0.0;
	float CurrentDrop = 0.0;

	Direction.Normalize();
	Direction = Direction * 100.0;

	while (1) {
		float BulletSpeed = FloatCurve.Eval(TravelDistance * BDS * BallisticDragScale, 0.0f);
		Vector Velocity = Direction * BulletSpeed;
		Velocity.Z += CurrentDrop;

		Vector Acceleration = Velocity * SimulationSubstepTime;
		float AccelerationLen = Acceleration.Length() / 100.0f;

		//BulletDrop += SimulationSubstepTime * CurrentDrop;

		if (TravelDistance + AccelerationLen > Distance)
			break;

		TravelDistance += AccelerationLen;
		TravelTime += SimulationSubstepTime;
		BulletDrop += SimulationSubstepTime * CurrentDrop;
		CurrentDrop += SimulationSubstepTime * TrajectoryGravityZ * 100.0f * VDragCoefficient * BallisticDropScale;
	}
}

std::pair<float, float> GetBulletDropAndTravelTime(const Vector& GunLocation, const Rotator& GunRotation, const Vector& TargetPos,
	float ZeroingDistance, float BulletDropAdd, float InitialSpeed, float TrajectoryGravityZ, float BallisticDragScale,
	float BallisticDropScale, float BDS, float SimulationSubstepTime, float VDragCoefficient, NativePtr<UCurveVector> BallisticCurve)
{
	const float ZDistanceToTarget = TargetPos.Z - GunLocation.Z;				//= 0.0f;
	const float DistanceToTarget = GunLocation.Distance(TargetPos) / 100.0f;
	
	float TravelTime = DistanceToTarget / InitialSpeed;
	float BulletDrop = 0.5f * TrajectoryGravityZ * TravelTime * TravelTime * 100.0f;

	float TravelTimeZero = ZeroingDistance / InitialSpeed;
	float BulletDropZero = 0.5f * TrajectoryGravityZ * TravelTimeZero * TravelTimeZero * 100.0f;

	UCurveVector CurveVector;
	if (BallisticCurve.Read(CurveVector)) {
		SimulateWeaponTrajectory(GunRotation.GetUnitVector(), DistanceToTarget, TrajectoryGravityZ, BallisticDragScale, BallisticDropScale, BDS, SimulationSubstepTime, VDragCoefficient, CurveVector.FloatCurves, TravelTime, BulletDrop);
		SimulateWeaponTrajectory(Vector(1.0f, 0.0f, 0.0f), ZeroingDistance, TrajectoryGravityZ, BallisticDragScale, BallisticDropScale, BDS, SimulationSubstepTime, VDragCoefficient, CurveVector.FloatCurves, TravelTimeZero, BulletDropZero);
	}

	BulletDrop = fabsf(BulletDrop) - fabsf(BulletDropAdd);
	if (BulletDrop < 0.0f)
		BulletDrop = 0.0f;
	BulletDropZero = fabsf(BulletDropZero) + fabsf(BulletDropAdd);

	const float TargetPitch = asinf((ZDistanceToTarget + BulletDrop) / 100.0f / DistanceToTarget);
	const float ZeroPitch = IsNearlyZero(ZeroingDistance) ? 0.0f : atan2f(BulletDropZero / 100.0f, ZeroingDistance);
	const float FinalPitch = TargetPitch - ZeroPitch;
	const float AdditiveZ = DistanceToTarget * sinf(FinalPitch) * 100.0f - ZDistanceToTarget;

	return std::pair(AdditiveZ , TravelTime);
}

FName UItem::GetItemID() {
	FItemTableRowBase ItemTableRowBase;
	if (!ItemTable.Read(ItemTableRowBase))
		return { 0, 0 };
	return ItemTableRowBase.ItemID;
}

float ATslWeapon_Trajectory::GetZeroingDistance() const {
	float ZeroingDistance = 0.0f;

	bool bUseIronSightZeroingDistance = true;
	for (const auto& AttachableItemPtr : AttachedItems.GetVector()) {
		UAttachableItem AttachableItem;
		if (!AttachableItemPtr.Read(AttachableItem))
			continue;

		FItemTableRowAttachment ItemTableRowAttachment;
		if (!AttachableItem.WeaponAttachmentData.Read(ItemTableRowAttachment))
			continue;

		if (ItemTableRowAttachment.AttachmentData.AttachmentSlotID != EWeaponAttachmentSlotID::UpperRail)
			continue;

		if (!ItemTableRowAttachment.AttachmentData.ZeroingDistances.GetValue(CurrentZeroLevel, ZeroingDistance))
			continue;

		bUseIronSightZeroingDistance = false;
		break;
	}

	if (bUseIronSightZeroingDistance)
		WeaponConfig_IronSightZeroingDistances.GetValue(CurrentZeroLevel, ZeroingDistance);

	return ZeroingDistance;
}

NativePtr<UStaticMeshComponent> UWeaponMeshComponent::GetStaticMeshComponentScopeType() const {
	for (const auto& Element : AttachedStaticComponentMap.GetVector()) {
		if (Element.Value.Key == EWeaponAttachmentSlotID::UpperRail) {
			return Element.Value.Value;
			break;
		}
	}
	return 0;
}

float UWeaponMeshComponent::GetScopingAttachPointRelativeZ(FName ScopingAttachPoint) const {
	float Default = 15.0f;

	UStaticMeshComponent StaticMeshComponent;
	if (GetStaticMeshComponentScopeType().Read(StaticMeshComponent)) {
		//Scope Attached
		const float RelativeZ_1 = StaticMeshComponent.GetSocketTransform(ScopingAttachPoint, RTS_Component).Translation.Z;
		const float RelativeZ_2 = Transform(StaticMeshComponent.ComponentToWorld).GetRelativeTransform(ComponentToWorld).Translation.Z;
		return RelativeZ_1 + RelativeZ_2;
	}
	else
		return GetSocketTransform(ScopingAttachPoint, RTS_Component).Translation.Z;

	return Default;
}

bool ATslCharacter::GetTslWeapon(ATslWeapon_Trajectory& OutTslWeapon) const {
	UWeaponProcessorComponent WeaponProcessorComponent;
	if (!WeaponProcessor.Read(WeaponProcessorComponent))
		return false;

	BYTE WeaponIndex = WeaponProcessorComponent.WeaponArmInfo_RightWeaponIndex;
	if (WeaponIndex < 0 || WeaponIndex >= 3)
		return false;

	NativePtr<ATslWeapon> TslWeaponPtr;
	if (!WeaponProcessorComponent.EquippedWeapons.GetValue(WeaponIndex, TslWeaponPtr))
		return false;

	return TslWeaponPtr.ReadOtherType(OutTslWeapon);
}

//DefBaseClass(ABulletInfo,
//	MemberAtOffset(float, TravelTime, 0x4)
//	MemberAtOffset(uint8_t, PadSize, 0x108 - 1)
//,)
//
//DefBaseClass(ABulletTable,
//	MemberAtOffsetZero(TArray<ABulletInfo>, BulletInfo, 0x0)
//,)
//
//MemberAtOffset(EncryptedPtr<ABulletTable>, BulletTable, 0xEC0)
////BulletTable
//[&] {
//	if (!IsWeaponed || !bHitScan)
//		return;
//
//	ABulletTable BulletTable;
//	if (!BulletTablePtr.Read(BulletTable))
//		return;
//
//	for (unsigned i = 0; i < BulletTable.BulletInfo.GetCount(); i++) {
//		uintptr_t ptr = BulletTable.BulletInfo + i * sizeof(ABulletInfo);
//		float TravelTime;
//		if (process.GetValue(ptr + offsetof(ABulletInfo, TravelTime), &TravelTime)) {
//
//			TravelTime += std::min((CustomTimeDilation - 1.0f) * TimeDelta, 0.02f);
//
//			printlog("%f %f\n", (CustomTimeDilation - 1.0f) * TimeDelta, TravelTime);
//
//			process.SetValue(ptr + offsetof(ABulletInfo, TravelTime), &TravelTime);
//		}
//	}
//}();
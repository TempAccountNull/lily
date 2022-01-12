#pragma once
#include "pubg_struct.h"
#include "vector.h"

Vector WorldToScreen(const Vector& WorldLocation, const Matrix& RotationMatrix, const Vector& CameraLocation, float CameraFOV, float Width, float Height);

std::pair<float, float> GetBulletDropAndTravelTime(const Vector& GunLocation, const Rotator& GunRotation, const Vector& TargetPos,
	float ZeroingDistance, float BulletDropAdd, float InitialSpeed, float TrajectoryGravityZ, float BallisticDragScale,
	float BallisticDropScale, float BDS, float SimulationSubstepTime, float VDragCoefficient, NativePtr<UCurveVector> BallisticCurve);
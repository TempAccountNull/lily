#pragma once

#include "pubg_struct.h"

#include "vector.h"
#include "quat.h"
#include "matrix.h"

class Vector;
class Quat;
class Matrix;

class Rotator {
public:
	float Pitch, Yaw, Roll;

	Rotator() : Pitch(0.0), Yaw(0.0), Roll(0.0) {}
	Rotator(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

	static float NormalizeAxis(float Angle) {
		//-180 ~ 180
		if (Angle > 180.0)
			Angle -= 360.0;
		if (Angle < -180.0)
			Angle += 360.0;
		return Angle;
	}

	void Clamp() {
		Pitch = NormalizeAxis(Pitch);
		Yaw = NormalizeAxis(Yaw);
		Roll = NormalizeAxis(Roll);

		if (Pitch > 75.0)
			Pitch = 75.0;

		if (Pitch < -75.0)
			Pitch = -75.0;
	}

	float InnerProduct(const Rotator& v) const {
		return (Pitch * v.Pitch) + (Yaw * v.Yaw) + (Roll * v.Roll);
	}

	Rotator OuterProduct(const Rotator& v) const {
		Rotator output;
		output.Pitch = (Yaw * v.Roll) - (Roll * v.Yaw);
		output.Yaw = (Roll * v.Pitch) - (Pitch * v.Roll);
		output.Roll = (Pitch * v.Yaw) - (Yaw * v.Pitch);
		return output;
	}

	bool operator == (const Rotator& v) const {
		return Pitch == v.Pitch && Yaw == v.Yaw && Roll == v.Roll;
	}

	bool operator != (const Rotator& v) const {
		return !(*this == v);
	}

	Rotator operator - () const {
		return Rotator(-Pitch, -Yaw, -Roll);
	}

	Rotator operator + (const Rotator& v) const {
		return Rotator(Pitch + v.Pitch, Yaw + v.Yaw, Roll + v.Roll);
	}

	Rotator operator - (const Rotator& v) const {
		return Rotator(Pitch - v.Pitch, Yaw - v.Yaw, Roll - v.Roll);
	}

	Rotator operator * (float Value) const {
		return Rotator(Pitch * Value, Yaw * Value, Roll * Value);
	}

	float Length() const {
		return sqrtf(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	float Distance(const Rotator& v) const {
		return (v - *this).Length();
	}

	Rotator operator ^ (const Rotator& v) const {
		return OuterProduct(v);
	}

	float operator * (const Rotator& v) const {
		return InnerProduct(v);
	}

	Quat GetQuaternion() const;
	Rotator(const Quat& q);
	operator Quat() const;

	Vector GetUnitVector() const;
	Matrix GetMatrix(Vector origin) const;

	//Convert to FRotator
	operator FRotator() const { return FRotator((float)Pitch, (float)Yaw, (float)Roll); }
	Rotator& operator=(const FRotator& v) { return *this = { v.Pitch, v.Yaw, v.Roll }; }
	Rotator(const FRotator& v) { operator=(v); }
};
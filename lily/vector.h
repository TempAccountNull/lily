#pragma once

#include "mYmath.h"
#include "pubg_struct.h"

#include "matriX.h"
#include "rotator.h"

class Vector
{
public:
	float X, Y, Z;
	Vector() : X(0.0), Y(0.0), Z(0.0) {}
	Vector(float X, float Y, float Z) :X(X), Y(Y), Z(Z) {}

	float DotProduct(const Vector& v) const {
		return (X * v.X) + (Y * v.Y) + (Z * v.Z);
	}

	Vector CrossProduct(const Vector& v) const {
		Vector output;
		output.X = (Y * v.Z) - (Z * v.Y);
		output.Y = (Z * v.X) - (X * v.Z);
		output.Z = (X * v.Y) - (Y * v.X);
		return output;
	}

	Vector Min(const Vector& v) const {
		Vector output;
		output.X = X < v.X ? X : v.X;
		output.Y = Y < v.Y ? Y : v.Y;
		output.Z = Z < v.Z ? Z : v.Z;
		return output;
	}

	Vector Max(const Vector& v) const {
		Vector output;
		output.X = X > v.X ? X : v.X;
		output.Y = Y > v.Y ? Y : v.Y;
		output.Z = Z > v.Z ? Z : v.Z;
		return output;
	}

	bool operator == (const Vector& v) const {
		return X == v.X && Y == v.Y && Z == v.Z;
	}

	bool operator != (const Vector& v) const {
		return !(*this == v);
	}

	Vector operator - () const {
		return Vector(-X, -Y, -Z);
	}

	Vector operator + (const Vector& v) const {
		return Vector(X + v.X, Y + v.Y, Z + v.Z);
	}

	Vector operator - (const Vector& v) const {
		return Vector(X - v.X, Y - v.Y, Z - v.Z);
	}

	Vector operator * (const Vector& v) const {
		return Vector(X * v.X, Y * v.Y, Z * v.Z);
	}

	Vector operator * (float Value) const {
		return Vector(X * Value, Y * Value, Z * Value);
	}

	void Normalize() {
		float factor = 1.0f / sqrtf(X * X + Y * Y + Z * Z);
		(*this) = (*this) * factor;
	}

	float Length() const {
		return sqrtf(X * X + Y * Y + Z * Z);
	}

	float Distance(const Vector& v) const {
		return (v - *this).Length();
	}

	Vector operator ^ (const Vector& v) const {
		return CrossProduct(v);
	}

	float operator | (const Vector& v) const {
		return DotProduct(v);
	}

	Vector GetSignVector() const
	{
		return Vector
		(
			Select(X, 1.0, -1.0),
			Select(Y, 1.0, -1.0),
			Select(Z, 1.0, -1.0)
		);
	}

	bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const {
		return fabs(X) <= Tolerance && fabs(Y) <= Tolerance && fabs(Z) <= Tolerance;
	}

	Rotator GetDirectionRotator() const;

	//Convert to FVector
	operator FVector() const { return FVector((float)X, (float)Y, (float)Z); }
	Vector& operator=(const FVector& v) { return *this = { v.X, v.Y, v.Z }; }
	Vector(const FVector& v) { operator=(v); }
};

static Vector operator * (float Value, const Vector& v) {
	return v.operator*(Value);
}
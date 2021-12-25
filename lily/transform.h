#pragma once

#include "pubg_struct.h"

#include "quat.h"
#include "vector.h"

class Transform {
public:
	Quat Rotation;
	Vector Translation;
	Vector Scale3D;

	Transform();
	Transform(const Quat& Rotation, const Vector& Translation, const Vector& Scale3D);
	static bool AnyHasNegativeScale(const Vector& InScale3D, const Vector& InOtherScale3D);
	static void Multiply(Transform* OutTransform, const Transform* A, const Transform* B);

	static void MultiplyUsingMatrixWithScale(Transform* OutTransform, const Transform* A, const Transform* B);
	static void ConstructTransformFromMatrixWithDesiredScale(const Matrix& AMatrix, const Matrix& BMatrix, const Vector& DesiredScale, Transform& OutTransform);

	Matrix ToMatrixWithScale() const;

	Transform operator*(const Transform& A);

	static Vector GetSafeScaleReciprocal(const Vector& InScale, float Tolerance = SMALL_NUMBER);
	Transform GetRelativeTransform(const Transform& Other) const;

	static void GetRelativeTransformUsingMatrixWithScale(Transform* OutTransform, const Transform* Base, const Transform* Relative);


	//Convert to FTransform
	operator FTransform() const;
	Transform& operator=(const FTransform& v);
	Transform(const FTransform& v);
};
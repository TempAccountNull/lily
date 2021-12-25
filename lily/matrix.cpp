#include "matrix.h"
#include "vector.h"
#include "rotator.h"
#include "transform.h"

Vector Matrix::GetScaledAxisX() const { return Vector(M[0][0], M[0][1], M[0][2]); }
Vector Matrix::GetScaledAxisY() const { return Vector(M[1][0], M[1][1], M[1][2]); }
Vector Matrix::GetScaledAxisZ() const { return Vector(M[2][0], M[2][1], M[2][2]); }

Vector Matrix::GetOrigin() const { return Vector(_41, _42, _43); }

Rotator Matrix::GetRotator() const {
    const Vector XAxis = GetScaledAxisX();
    const Vector YAxis = GetScaledAxisY();
    const Vector ZAxis = GetScaledAxisZ();

    Rotator r = Rotator(
        atan2f(XAxis.Z, sqrtf(XAxis.X * XAxis.X + XAxis.Y * XAxis.Y)) * 180.0f / PI,
        atan2f(XAxis.Y, XAxis.X) * 180.0f / PI,
        0
    );

    const Vector SYAxis = GetScaledAxisY();

    r.Roll = atan2f(ZAxis | SYAxis, YAxis | SYAxis) * 180.0f / PI;

    return r;
}

Matrix& Matrix::operator=(const FTransform& t) { return *this = Transform(t).ToMatrixWithScale(); }
Matrix::Matrix(const FTransform& t) { operator=(t); }

void Matrix::SetAxis0(const Vector& Axis)
{
    M[0][0] = Axis.X;
    M[0][1] = Axis.Y;
    M[0][2] = Axis.Z;
}

void Matrix::SetAxis1(const Vector& Axis)
{
    M[1][0] = Axis.X;
    M[1][1] = Axis.Y;
    M[1][2] = Axis.Z;
}

void Matrix::SetAxis2(const Vector& Axis)
{
    M[2][0] = Axis.X;
    M[2][1] = Axis.Y;
    M[2][2] = Axis.Z;
}

Matrix Matrix::Inverse() const
{
    Matrix Result;

    // Check for zero scale matrix to invert
    if (GetScaledAxisX().IsNearlyZero(SMALL_NUMBER) &&
        GetScaledAxisY().IsNearlyZero(SMALL_NUMBER) &&
        GetScaledAxisZ().IsNearlyZero(SMALL_NUMBER))
    {
        // just set to zero - avoids unsafe inverse of zero and duplicates what QNANs were resulting in before (scaling away all children)
        Result = Matrix();
    }
    else
    {
        const float	Det = Determinant();

        if (Det == 0.0f)
        {
            Result = Matrix();
        }
        else
        {
            VectorMatrixInverse(&Result, this);
        }
    }

    return Result;
}
#include "vector.h"
#include "rotator.h"

Rotator Vector::GetDirectionRotator() const {
	Rotator r;
	r.Pitch = ConvertToDegrees(atan2f(Z, sqrtf(X * X + Y * Y)));
	r.Yaw = ConvertToDegrees(atan2f(Y, X));
	r.Roll = 0.0;
	return r;
}
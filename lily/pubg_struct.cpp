#include "pubg_struct.h"

static bool IsItNotWeighted(const FRichCurveKey& Key1, const FRichCurveKey& Key2)
{
	return ((Key1.TangentWeightMode == RCTWM_WeightedNone || Key1.TangentWeightMode == RCTWM_WeightedArrive)
		&& (Key2.TangentWeightMode == RCTWM_WeightedNone || Key2.TangentWeightMode == RCTWM_WeightedLeave));
}

static float WeightedEvalForTwoKeys(
	float Key1Value, float Key1Time, float Key1LeaveTangent, float Key1LeaveTangentWeight, ERichCurveTangentWeightMode Key1TangentWeightMode,
	float Key2Value, float Key2Time, float Key2ArriveTangent, float Key2ArriveTangentWeight, ERichCurveTangentWeightMode Key2TangentWeightMode,
	float InTime)
{
	const float Diff = Key2Time - Key1Time;
	const float Alpha = (InTime - Key1Time) / Diff;
	const float P0 = Key1Value;
	const float P3 = Key2Value;
	const float OneThird = 1.0f / 3.0f;
	const float Time1 = Key1Time;
	const float Time2 = Key2Time;
	const float X = Time2 - Time1;
	float CosAngle, SinAngle;
	float Angle = atanf(Key1LeaveTangent);
	SinAngle = sinf(Angle);
	CosAngle = cosf(Angle);
	float LeaveWeight;
	if (Key1TangentWeightMode == RCTWM_WeightedNone || Key1TangentWeightMode == RCTWM_WeightedArrive)
	{
		const float LeaveTangentNormalized = Key1LeaveTangent;
		const float Y = LeaveTangentNormalized * X;
		LeaveWeight = sqrtf(X * X + Y * Y) * OneThird;
	}
	else
	{
		LeaveWeight = Key1LeaveTangentWeight;
	}
	const float Key1TanX = CosAngle * LeaveWeight + Time1;
	const float Key1TanY = SinAngle * LeaveWeight + Key1Value;

	Angle = atanf(Key2ArriveTangent);
	SinAngle = sinf(Angle);
	CosAngle = cosf(Angle);
	float ArriveWeight;
	if (Key2TangentWeightMode == RCTWM_WeightedNone || Key2TangentWeightMode == RCTWM_WeightedLeave)
	{
		const float ArriveTangentNormalized = Key2ArriveTangent;
		const float Y = ArriveTangentNormalized * X;
		ArriveWeight = sqrtf(X * X + Y * Y) * OneThird;
	}
	else
	{
		ArriveWeight = Key2ArriveTangentWeight;
	}
	const float Key2TanX = -CosAngle * ArriveWeight + Time2;
	const float Key2TanY = -SinAngle * ArriveWeight + Key2Value;

	//Normalize the Time Range
	const float RangeX = Time2 - Time1;

	const float Dx1 = Key1TanX - Time1;
	const float Dx2 = Key2TanX - Time1;

	// Normalize values
	const float NormalizedX1 = Dx1 / RangeX;
	const float NormalizedX2 = Dx2 / RangeX;

	float Coeff[4];
	float Results[3];

	//Convert Bezier to Power basis, also float to float for precision for root finding.
	BezierToPower(
		0.0, NormalizedX1, NormalizedX2, 1.0,
		&(Coeff[3]), &(Coeff[2]), &(Coeff[1]), &(Coeff[0])
	);

	Coeff[0] = Coeff[0] - Alpha;

	int NumResults = SolveCubic(Coeff, Results);
	float NewInterp = Alpha;
	if (NumResults == 1)
	{
		NewInterp = Results[0];
	}
	else
	{
		NewInterp = std::numeric_limits<float>::lowest(); //just need to be out of range
		for (float Result : Results)
		{
			if ((Result >= 0.0f) && (Result <= 1.0f))
			{
				if (NewInterp < 0.0f || Result > NewInterp)
				{
					NewInterp = Result;
				}
			}
		}

		if (NewInterp == std::numeric_limits<double>::lowest())
		{
			NewInterp = 0.f;
		}

	}
	//now use NewInterp and adjusted tangents plugged into the Y (Value) part of the graph.
	//const float P0 = Key1.Value;
	const float P1 = Key1TanY;
	//const float P3 = Key2.Value;
	const float P2 = Key2TanY;

	float OutValue = BezierInterp(P0, P1, P2, P3, NewInterp);
	return OutValue;
}

static void CycleTime(float MinTime, float MaxTime, float& InTime, int& CycleCount)
{
	float InitTime = InTime;
	float Duration = MaxTime - MinTime;

	if (InTime > MaxTime)
	{
		CycleCount = (int)floor((MaxTime - InTime) / Duration);
		InTime = InTime + Duration * CycleCount;
	}
	else if (InTime < MinTime)
	{
		CycleCount = (int)floor((InTime - MinTime) / Duration);
		InTime = InTime - Duration * CycleCount;
	}

	if (InTime == MaxTime && InitTime < MinTime)
	{
		InTime = MinTime;
	}

	if (InTime == MinTime && InitTime > MaxTime)
	{
		InTime = MaxTime;
	}

	CycleCount = abs(CycleCount);
}

static float EvalForTwoKeys(const FRichCurveKey& Key1, const FRichCurveKey& Key2, const float InTime)
{
	const float Diff = Key2.Time - Key1.Time;

	if (Diff > 0.f && Key1.InterpMode != RCIM_Constant)
	{
		const float Alpha = (InTime - Key1.Time) / Diff;
		const float P0 = Key1.Value;
		const float P3 = Key2.Value;

		if (Key1.InterpMode == RCIM_Linear)
		{
			return Lerp(P0, P3, Alpha);
		}
		else
		{
			if (IsItNotWeighted(Key1, Key2))
			{
				const float OneThird = 1.0f / 3.0f;
				const float P1 = P0 + (Key1.LeaveTangent * Diff * OneThird);
				const float P2 = P3 - (Key2.ArriveTangent * Diff * OneThird);

				return BezierInterp(P0, P1, P2, P3, Alpha);
			}
			else //it's weighted
			{
				return  WeightedEvalForTwoKeys(
					Key1.Value, Key1.Time, Key1.LeaveTangent, Key1.LeaveTangentWeight, (ERichCurveTangentWeightMode)Key1.TangentWeightMode,
					Key2.Value, Key2.Time, Key2.ArriveTangent, Key2.ArriveTangentWeight, (ERichCurveTangentWeightMode)Key2.TangentWeightMode,
					InTime);
			}
		}
	}
	else
	{
		return Key1.Value;
	}
}


void FRichCurve::RemapTimeValue(float& InTime, float& CycleValueOffset) const
{
	const int32 NumKeys = (int32)this->Keys.GetCount();

	if (NumKeys < 2)
	{
		return;
	}

	if (InTime <= Keys[0].Time)
	{
		if (PreInfinityExtrap != RCCE_Linear && PreInfinityExtrap != RCCE_Constant)
		{
			float MinTime = Keys[0].Time;
			float MaxTime = Keys[NumKeys - 1].Time;

			int CycleCount = 0;
			CycleTime(MinTime, MaxTime, InTime, CycleCount);

			if (PreInfinityExtrap == RCCE_CycleWithOffset)
			{
				float DV = Keys[0].Value - Keys[NumKeys - 1].Value;
				CycleValueOffset = DV * CycleCount;
			}
			else if (PreInfinityExtrap == RCCE_Oscillate)
			{
				if (CycleCount % 2 == 1)
				{
					InTime = MinTime + (MaxTime - InTime);
				}
			}
		}
	}
	else if (InTime >= Keys[NumKeys - 1].Time)
	{
		if (PostInfinityExtrap != RCCE_Linear && PostInfinityExtrap != RCCE_Constant)
		{
			float MinTime = Keys[0].Time;
			float MaxTime = Keys[NumKeys - 1].Time;

			int CycleCount = 0;
			CycleTime(MinTime, MaxTime, InTime, CycleCount);

			if (PostInfinityExtrap == RCCE_CycleWithOffset)
			{
				float DV = Keys[NumKeys - 1].Value - Keys[0].Value;
				CycleValueOffset = DV * CycleCount;
			}
			else if (PostInfinityExtrap == RCCE_Oscillate)
			{
				if (CycleCount % 2 == 1)
				{
					InTime = MinTime + (MaxTime - InTime);
				}
			}
		}
	}
}

float FRichCurve::Eval(float InTime, float InDefaultValue) const
{
	// Remap time if extrapolation is present and compute offset value to use if cycling 
	float CycleValueOffset = 0;
	RemapTimeValue(InTime, CycleValueOffset);

	const int32 NumKeys = (int32)this->Keys.GetCount();

	// If the default value hasn't been initialized, use the incoming default value
	float InterpVal = DefaultValue == DBL_MAX ? InDefaultValue : DefaultValue;

	if (NumKeys == 0)
	{
		// If no keys in curve, return the Default value.
	}
	else if (NumKeys < 2 || (InTime <= Keys[0].Time))
	{
		if (PreInfinityExtrap == RCCE_Linear && NumKeys > 1)
		{
			float DT = Keys[1].Time - Keys[0].Time;

			if (IsNearlyZero(DT))
			{
				InterpVal = Keys[0].Value;
			}
			else
			{
				float DV = Keys[1].Value - Keys[0].Value;
				float Slope = DV / DT;

				InterpVal = Slope * (InTime - Keys[0].Time) + Keys[0].Value;
			}
		}
		else
		{
			// Otherwise if constant or in a cycle or oscillate, always use the first key value
			InterpVal = Keys[0].Value;
		}
	}
	else if (InTime < Keys[NumKeys - 1].Time)
	{
		// perform a lower bound to get the second of the interpolation nodes
		int32 first = 1;
		int32 last = NumKeys - 1;
		int32 count = last - first;

		while (count > 0)
		{
			int32 step = count / 2;
			int32 middle = first + step;

			if (InTime >= Keys[middle].Time)
			{
				first = middle + 1;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}

		InterpVal = EvalForTwoKeys(Keys[first - 1], Keys[first], InTime);
	}
	else
	{
		if (PostInfinityExtrap == RCCE_Linear)
		{
			float DT = Keys[NumKeys - 2].Time - Keys[NumKeys - 1].Time;

			if (IsNearlyZero(DT))
			{
				InterpVal = Keys[NumKeys - 1].Value;
			}
			else
			{
				float DV = Keys[NumKeys - 2].Value - Keys[NumKeys - 1].Value;
				float Slope = DV / DT;

				InterpVal = Slope * (InTime - Keys[NumKeys - 1].Time) + Keys[NumKeys - 1].Value;
			}
		}
		else
		{
			// Otherwise if constant or in a cycle or oscillate, always use the last key value
			InterpVal = Keys[NumKeys - 1].Value;
		}
	}

	return InterpVal + CycleValueOffset;
}


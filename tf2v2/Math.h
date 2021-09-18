#pragma once
#include "SDK.h"

#define TICK_INTERVAL			( gInts.Globals->interval_per_tick)
#define TIME_TO_TICKS( t )		( (int)( 0.5f + (float)( t ) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL * (float)( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		( -1 )

class CMath
{
public:
	void AngleVectors(const Vector &angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	void TransformVector(const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut)
	{
		for (auto i = 0; i < 3; i++)
			vOut[i] = vSome.Dot((Vector&)vMatrix[i]) + vMatrix[i][3];
	}

	float DistancePointToLine(Vector Point, Vector LineOrigin, Vector Dir)
	{
		auto PointDir = Point - LineOrigin;

		auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
		if (TempOffset < 0.000001f)
			return FLT_MAX;

		auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

		return (Point - PerpendicularPoint).Length();
	}

	Vector CalcAngle(Vector src, Vector dst)
	{
		Vector AimAngles, delta;
		float hyp;
		delta = src - dst;
		hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y));
		AimAngles.x = atanf(delta.z / hyp) * RADPI;
		AimAngles.y = atanf(delta.y / delta.x) * RADPI;
		AimAngles.z = 0.0f;
		if (delta.x >= 0.0)
			AimAngles.y += 180.0f;
		return AimAngles;
	}


	void MakeVector(Vector angle, Vector& vector)
	{
		float pitch, yaw, tmp;
		pitch = float(angle[0] * PI / 180);
		yaw = float(angle[1] * PI / 180);
		tmp = float(cos(pitch));
		vector[0] = float(-tmp * -cos(yaw));
		vector[1] = float(sin(yaw)*tmp);
		vector[2] = float(-sin(pitch));
	}

	float GetFOV(Vector angle, Vector src, Vector dst)
	{

		Vector ang, aim;
		float mag, u_dot_v;
		ang = CalcAngle(src, dst);


		MakeVector(angle, aim);
		MakeVector(ang, ang);

		mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
		u_dot_v = aim.Dot(ang);

		float result = RAD2DEG(acos(u_dot_v / (pow(mag, 2))));

		if (!isfinite(result) || isinf(result) || isnan(result))
			result = 0.0f;

		return result;
	}

	void CorrectMovement(Vector old_angles, CUserCmd* cmd, float old_forwardmove, float old_sidemove)
	{
		float delta_view;
		float f1;
		float f2;

		if (old_angles.y < 0.f)
			f1 = 360.0f + old_angles.y;
		else
			f1 = old_angles.y;

		if (cmd->viewangles.y < 0.0f)
			f2 = 360.0f + cmd->viewangles.y;
		else
			f2 = cmd->viewangles.y;

		if (f2 < f1)
			delta_view = abs(f2 - f1);
		else
			delta_view = 360.0f - abs(f1 - f2);

		delta_view = 360.0f - delta_view;

		cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
		cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	}

	FORCEINLINE void VectorMultiply_PS3(const Vector& a, float b, Vector& c)
	{
		c.x = a.x * b;
		c.y = a.y * b;
		c.z = a.z * b;
	}

	FORCEINLINE void VectorMultiply_PS3(const Vector& a, const Vector& b, Vector& c)
	{
		c.x = a.x * b.x;
		c.y = a.y * b.y;
		c.z = a.z * b.z;
	}

	inline void VectorScale(const Vector& in, float scale, Vector& result)
	{
		VectorMultiply_PS3(in, scale, result);
	}


};
extern CMath gMath;
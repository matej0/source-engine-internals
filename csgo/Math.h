#pragma once
#include "SDK.h"
#define TICK_INTERVAL			( gInts.Globals->interval_per_tick)
#define TIME_TO_TICKS( t )		( (int)( 0.5f + (float)( t ) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL * (float)( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		( -1 )
class RequiredMath
{
public:

	/*static Vector GetIntersectionPoint(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, float radius)
	{
		auto sphereRayIntersection = [start, end, radius](auto&& center) -> Vector
		{
			auto direction = (end - start);
			direction.Normalize();

			Vector q = center - start;
			auto v = q.Dot(direction);
			auto d = radius * radius - (q.LengthSqr() - v * v);

			if (d < FLT_EPSILON)
				return {};

			return start + direction * (v - std::sqrt(d));
		};

		auto delta = (maxs - mins);
		delta.Normalize();

		for (size_t i{}; i < std::floor(mins.DistTo(maxs)); ++i)
		{
			auto intersection = sphereRayIntersection(mins + delta * float(i));

			if (!intersection.IsZero())
				return intersection;
		}

		auto intersection = sphereRayIntersection(maxs);
		if (!intersection.IsZero())
			return intersection;

		return {};
	}*/

	Vector AngleVector(Vector meme)
	{
		auto sy = sin(meme.y / 180.f * static_cast<float>(3.141592654f));
		auto cy = cos(meme.y / 180.f * static_cast<float>(3.141592654f));

		auto sp = sin(meme.x / 180.f * static_cast<float>(3.141592654f));
		auto cp = cos(meme.x / 180.f* static_cast<float>(3.141592654f));

		return Vector(cp*cy, cp*sy, -sp);
	}

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
		double hyp;
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


	inline float DotProduct(const float* v1, const float* v2)
	{
		return(v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
	}


	void VectorRotate(const float* in1, const matrix3x4& in2, float* out)
	{
		float x = DotProduct(in1, in2[0]);
		float y = DotProduct(in1, in2[1]);
		float z = DotProduct(in1, in2[2]);

		out[0] = x;
		out[1] = y;
		out[2] = z;
	}

	void _VectorTransform(const float * in1, const matrix3x4& in2, float * out)
	{
		float x = DotProduct(in1, in2[0]) + in2[0][3];
		float y = DotProduct(in1, in2[1]) + in2[1][3];
		float z = DotProduct(in1, in2[2]) + in2[2][3];

		out[0] = x;
		out[1] = y;
		out[2] = z;
	}


	void VectorTransform(const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut)
	{
		vOut = {
			  vSome.Dot(Vector(vMatrix[0][0], vMatrix[0][1], vMatrix[0][2])) + vMatrix[0][3],
	          vSome.Dot(Vector(vMatrix[1][0], vMatrix[1][1], vMatrix[1][2])) + vMatrix[1][3],
	          vSome.Dot(Vector(vMatrix[2][0], vMatrix[2][1], vMatrix[2][2])) + vMatrix[2][3]
		};
	}

	void MatrixSetColumn(Vector &in, int column, matrix3x4& out)
	{
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}

	void _CrossProduct(Vector* v1, Vector* v2, Vector* cross)
	{
		cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
		cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
		cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
	}


	void VectorVectors(Vector &forward, Vector &right, Vector &up)
	{
		Vector tmp;

		if (fabs(forward[0]) < 1e-6 && fabs(forward[1]) < 1e-6)
		{
			// pitch 90 degrees up/down from identity
			right[0] = 0;
			right[1] = -1;
			right[2] = 0;
			up[0] = -forward[2];
			up[1] = 0;
			up[2] = 0;
		}
		else
		{
			tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
			_CrossProduct(&forward, &tmp, &right);
			right.NormalizeInPlace();
			_CrossProduct(&right, &forward, &up);
			up.Normalize();
		}
	}



	void VectorMatrix(Vector &forward, matrix3x4& matrix)
	{
		Vector right, up;
		VectorVectors(forward, right, up);

		MatrixSetColumn(forward, 0, matrix);
		MatrixSetColumn(-right, 1, matrix);
		MatrixSetColumn(up, 2, matrix);
	}

	float RandomFloat(float min, float max)
	{
		static auto oRandomFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
		return oRandomFloat(min, max);
	}

};
extern RequiredMath gMath;
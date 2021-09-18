#include "SDK.h"
#include <DirectXMath.h>

#pragma once
#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define square( x ) ( x * x )
#define RADPI 57.295779513082f
#define SQUARE( a ) a*a
#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

class Quaternion
{
public:
	float x, y, z, w;
};

class Vector					
{
public:
	float x, y, z;
	Vector(void); 
	Vector(float X, float Y, float Z);
	void Init(float ix=0.0f, float iy=0.0f, float iz=0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;	
	__forceinline Vector&	operator+=(const Vector &v);			
	__forceinline Vector&	operator-=(const Vector &v);		
	__forceinline Vector&	operator*=(const Vector &v);			
	__forceinline Vector&	operator*=(float s);
	__forceinline Vector&	operator/=(const Vector &v);		
	__forceinline Vector&	operator/=(float s);	
	__forceinline Vector&	operator+=(float fl);
	__forceinline Vector&	operator-=(float fl);
	inline float	Length() const;

	Vector ToEulerAngles();



	__forceinline float LengthSqr(void) const
	{ 
		CHECK_VALID(*this);
		return (x*x + y*y + z*z);		
	}
	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance &&
				y > -tolerance && y < tolerance &&
				z > -tolerance && z < tolerance);
	}
	float Normalize()
	{
		float flLength = Length();
		float flLengthNormal = 1.f / (FLT_EPSILON + flLength);

		x *= flLengthNormal;
		y *= flLengthNormal;
		z *= flLengthNormal;

		return flLength;
	}
	float	NormalizeInPlace();
	__forceinline float	DistToSqr(const Vector &vOther) const;
	__forceinline float   DistTo(const Vector &vOther) const;
	float	Dot(const Vector& vOther) const;			
	float	Length2D(void) const;		
	float	Length2DSqr(void) const;
	Vector& operator=(const Vector &vOther);
	Vector	operator-(void) const;
	Vector	operator+(const Vector& v) const;	
	Vector	operator-(const Vector& v) const;	
	Vector	operator*(const Vector& v) const;	
	Vector	operator/(const Vector& v) const;	
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;
};
//===============================================
inline void Vector::Init( float ix, float iy, float iz )    
{ 
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}
//===============================================
inline Vector::Vector(float X, float Y, float Z)
{ 
	x = X; y = Y; z = Z;
	CHECK_VALID(*this);
}
//===============================================
inline Vector::Vector(void){ }
//===============================================
inline void Vector::Zero()
{
	x = y = z = 0.0f;
}
//===============================================
inline void VectorClear( Vector& a )
{
	a.x = a.y = a.z = 0.0f;
}
//===============================================
inline Vector& Vector::operator=(const Vector &vOther)	
{
	CHECK_VALID(vOther);
	x=vOther.x; y=vOther.y; z=vOther.z; 
	return *this; 
}
//===============================================
inline float& Vector::operator[](int i)
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}
//===============================================
inline float Vector::operator[](int i) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}
//===============================================
inline bool Vector::operator==( const Vector& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool Vector::operator!=( const Vector& src ) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
__forceinline void VectorCopy( const Vector& src, Vector& dst )
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
//===============================================
__forceinline  Vector& Vector::operator+=(const Vector& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x+=v.x; y+=v.y; z += v.z;	
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator-=(const Vector& v)	
{ 
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x-=v.x; y-=v.y; z -= v.z;	
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator*=(float fl)	
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator*=(const Vector& v)	
{ 
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline Vector&	Vector::operator+=(float fl) 
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline Vector&	Vector::operator-=(float fl) 
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator/=(float fl)	
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
__forceinline  Vector& Vector::operator/=(const Vector& v)	
{ 
	CHECK_VALID(v);
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}
//===============================================
inline float Vector::Length(void) const	
{
	CHECK_VALID(*this);
	
	float root = 0.0f;

	float sqsr = x*x + y*y + z*z;

	__asm
	{
		sqrtss xmm0, sqsr
		movss root, xmm0
	}

	return root;
}

inline Vector Vector::ToEulerAngles()
{
	float Pitch, Yaw, Length;

	Length = this->Length2D();

	if (Length > 0)
	{
		Pitch = (atan2(-this->z, Length) * 180.f / PI);

		if (Pitch < 0)
		{
			Pitch += 360;
		}

		Yaw = (atan2(this->y, this->x) * 180.f / PI);

		if (Yaw < 0)
		{
			Yaw += 360;
		}
	}
	else
	{
		Pitch = (this->z > 0.0f) ? 270.f : 90.f;
		Yaw = 0;
	}

	return Vector(Pitch, Yaw, 0.0f);
}

//===============================================
inline float Vector::Length2D(void) const
{
	CHECK_VALID(*this);
	
	float root = 0.0f;

	float sqst = x*x + y*y;

	__asm
	{
		sqrtss xmm0, sqst
		movss root, xmm0
	}

	return root;
}
//===============================================
inline float Vector::Length2DSqr(void) const
{ 
	return (x*x + y*y); 
}
//===============================================
inline Vector CrossProduct(const Vector& a, const Vector& b) 
{ 
	return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); 
}
//===============================================
float Vector::DistToSqr(const Vector &vOther) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}
inline float Vector::DistTo(const Vector &vOther) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.Length();
}
//===============================================
inline float Vector::NormalizeInPlace()
{
	Vector& v = *this;

	float iradius = 1.f / ( this->Length() + FLT_EPSILON);
	
	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;

	return this->Length();
}
//===============================================
inline Vector Vector::operator+(const Vector& v) const	
{ 
	Vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;	
}
//===============================================
inline Vector Vector::operator-(const Vector& v) const	
{ 
	Vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;	
}
//===============================================
inline Vector Vector::operator*(float fl) const	
{ 
	Vector res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;	
}
//===============================================
inline Vector Vector::operator*(const Vector& v) const	
{ 
	Vector res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;	
}
//===============================================
inline Vector Vector::operator/(float fl) const	
{ 
	Vector res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;	
}
//===============================================
inline Vector Vector::operator/(const Vector& v) const	
{ 
	Vector res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}
inline float Vector::Dot( const Vector& vOther ) const
{
	const Vector& a = *this;
	
	return( a.x*vOther.x + a.y*vOther.y + a.z*vOther.z ); 
}

inline void ClampAngle(Vector& qaAng)
{
	while (qaAng.x > 89)
		qaAng.x -= 180;

	while (qaAng.x < -89)
		qaAng.x += 180;

	while (qaAng.y > 180)
		qaAng.y -= 360;

	while (qaAng.y < -180)
		qaAng.y += 360;

	while (qaAng.z != 0)
		qaAng.z = 0;
}

inline void NormalizeAngle(float &flAngle)
{
	while (flAngle > 180.f)
		flAngle -= 360.f;

	while (flAngle < -180.f)
		flAngle += 360.f;
}

inline void NormalizeAngles(Vector &vecAngles)
{
	for (int iAxis = 0; iAxis < 3; iAxis++)
		NormalizeAngle(vecAngles[iAxis]);
}

inline void SinCos(float flRadians, float *flSine, float *flCosine)
{
	__asm
	{
		fld dword ptr[flRadians]
		fsincos

		mov edx, dword ptr[flCosine]
		mov eax, dword ptr[flSine]

		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}


inline void AngleVectors(const Vector &vecAngles, Vector *vecForward, Vector *vecRight, Vector *vecUp)
{
	float flSinPitch, flSinYaw, flSinRoll, flCosPitch, flCosYaw, flCosRoll;

	SinCos(DEG2RAD(vecAngles.x), &flSinPitch, &flCosPitch);
	SinCos(DEG2RAD(vecAngles.y), &flSinYaw, &flCosYaw);
	SinCos(DEG2RAD(vecAngles.z), &flSinRoll, &flCosRoll);

	if (vecForward)
	{
		vecForward->x = flCosPitch * flCosYaw;
		vecForward->y = flCosPitch * flSinYaw;
		vecForward->z = -flSinPitch;
		vecForward->Normalize();
	}

	if (vecRight)
	{
		vecRight->x = -1.f * flSinRoll * flSinPitch * flCosYaw + -1.f * flCosRoll * -flSinYaw;
		vecRight->y = -1.f * flSinRoll * flSinPitch * flSinYaw + -1.f * flCosRoll * flCosYaw;
		vecRight->z = -1.f * flSinRoll * flCosPitch;
		vecRight->Normalize();
	}

	if (vecUp)
	{
		vecUp->x = flCosRoll * flSinPitch * flCosYaw + -flSinRoll * -flSinYaw;
		vecUp->y = flCosRoll * flSinPitch * flSinYaw + -flSinRoll * flCosYaw;
		vecUp->z = flCosRoll * flCosPitch;
		vecUp->Normalize();
	}
}


inline float DotProduct(const Vector &v1, const Vector &v2)
{
	return(v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}


inline void AngleVectors(const Vector &angles, Vector *forward)
{
	float sp, sy, cp, cy;

	sy = sinf(DEG2RAD(angles.y));
	cy = cosf(DEG2RAD(angles.y));

	sp = sinf(DEG2RAD(angles.x));
	cp = cosf(DEG2RAD(angles.x));

	forward->x = (cp * cy);
	forward->y = (cp * sy);
	forward->z = -sp;
}

inline void VectorAngles(Vector &forward, Vector &angles)
{
	float tmp, yaw, pitch;

	if (forward.y == 0 && forward.x == 0)
	{
		yaw = 0;

		if (forward.z > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = RAD2DEG(atan2f(forward.y, forward.x));

		if (yaw < 0)
			yaw += 360;

		tmp = forward.Length2D();
		pitch = RAD2DEG(atan2f(-forward.z, tmp));

		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}





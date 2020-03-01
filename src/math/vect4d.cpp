#include <memory.h>
#include <math.h>
#include "vect3d.h"
#include "vect4d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//constructors
Vect4d::Vect4d()
{
	memcpy(v,0,4*sizeof(float));
}

Vect4d::Vect4d(float x,float y,float z,float w)
{
	v[0]=x;v[1]=y;v[2]=z;v[3]=w;
}

Vect4d::Vect4d(const float *newv)
{
	memcpy(v,newv,4*sizeof(float));
}

Vect4d::Vect4d(const Vect4d &newv)
{
	memcpy(v,newv.v,4*sizeof(float));
}


//unary operators
void Vect4d::Zero(void)
{
	memset(v,0,4*sizeof(float));
}

void Vect4d::One(void)
{
	v[0]=v[1]=v[2]=1.f;
}

void Vect4d::RotateX(double angle)
{
	(*this)=GetRotatedX(angle);
}

Vect4d Vect4d::GetRotatedX(double angle) const
{
	Vect3d v3d(v[0],v[1],v[2]);

	v3d.RotateX(angle);

	return Vect4d(v3d);
}

void Vect4d::RotateY(double angle)
{
	(*this)=GetRotatedY(angle);
}

Vect4d Vect4d::GetRotatedY(double angle) const
{
	Vect3d v3d(v[0],v[1],v[2]);

	v3d.RotateY(angle);
	return Vect4d(v3d);
}

void Vect4d::RotateZ(double angle)
{
	(*this)=GetRotatedZ(angle);
}

Vect4d Vect4d::GetRotatedZ(double angle) const
{
	Vect3d v3d(v[0],v[1],v[2]);

	v3d.RotateZ(angle);

	return Vect4d(v3d);
}

void Vect4d::RotateAxis(double angle, const Vect3d & axis)
{
	(*this)=GetRotatedAxis(angle, axis);
}

Vect4d Vect4d::GetRotatedAxis(double angle, const Vect3d & axis) const
{
	Vect3d v3d(v[0],v[1],v[2]);

	v3d.RotateAxis(angle, axis);

	return Vect4d(v3d);
}


Vect4d operator*(float scaleFactor, const Vect4d & rhs)
{
	return rhs*scaleFactor;
}

bool Vect4d::operator==(const Vect4d & rhs) const
{
	if(v[0]==rhs.v[0] && v[1]==rhs.v[1] && v[2]==rhs.v[2] && v[3]==rhs.v[3]) return true;
																	    else return false;
}

Vect4d::operator Vect3d()
{
	if(v[3]==0.0f || v[3]==1.0f) return Vect3d(v[0],v[1],v[2]);
		                    else return Vect3d(v[0]/v[3],v[1]/v[3],v[2]/v[3]);
}

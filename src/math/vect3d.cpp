#include <memory.h>
#include <math.h>
#include "math/vect3d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//constructors
Vect3d::Vect3d()
{
	memset(v,0,sizeof(float)*3);
}

Vect3d::Vect3d(float x,float y,float z)
{
	v[0]=x;v[1]=y;v[2]=z;
}

Vect3d::Vect3d(const float *newv)
{
	memcpy(v,newv,sizeof(float)*3);
}

Vect3d::Vect3d(const Vect3d &newv)
{
	memcpy(v,newv.v,sizeof(float)*3);
}


//unary operators
void Vect3d::Zero(void)
{
	memset(v,0,3*sizeof(float));
}

void Vect3d::One(void)
{
	v[0]=v[1]=v[2]=1.f;
}


inline void Vect3d::Normalize()
{
	float length=Length();

	if(length==1||length==0)return;
	float scalefactor = 1.0f/length;
	v[0]*=scalefactor;
	v[1]*=scalefactor;
	v[2]*=scalefactor;
}

inline float Vect3d::Length() const
{
	return (float)sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
}


Vect3d Vect3d::GetNormalized() const
{
	Vect3d result(*this);

	result.Normalize();

	return result;
}

Vect3d Vect3d::GetRotatedX(double angle) const
{
	if(angle==0.0)	return (*this);

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);

	return Vect3d(v[0],v[1]*cosAngle-v[2]*sinAngle,v[1]*sinAngle+v[2]*cosAngle);
}

void Vect3d::RotateX(double angle)
{
	(*this)=GetRotatedX(angle);
}

Vect3d Vect3d::GetRotatedY(double angle) const
{
	if(angle==0.0)	return (*this);

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);

	return Vect3d(v[0]*cosAngle+v[2]*sinAngle,v[1],-v[0]*sinAngle+v[2]*cosAngle);
}

void Vect3d::RotateY(double angle)
{
	(*this)=GetRotatedY(angle);
}

Vect3d Vect3d::GetRotatedZ(double angle) const
{
	if(angle==0.0)	return (*this);

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);
	
	return Vect3d(v[0]*cosAngle-v[1]*sinAngle,v[0]*sinAngle+v[1]*cosAngle,v[2]);
}

void Vect3d::RotateZ(double angle)
{
	(*this)=GetRotatedZ(angle);
}

Vect3d Vect3d::GetRotatedAxis(double angle, const Vect3d & axis) const
{
	if(angle==0.0)
		return (*this);

	Vect3d u=axis.GetNormalized();

	Vect3d rotMatrixRow0, rotMatrixRow1, rotMatrixRow2;

	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);
	float oneMinusCosAngle=1.0f-cosAngle;

	rotMatrixRow0.v[0]=(u.v[0])*(u.v[0])+cosAngle*(1-(u.v[0])*(u.v[0]));
	rotMatrixRow0.v[1]=(u.v[0])*(u.v[1])*(oneMinusCosAngle)-sinAngle*u.v[2];
	rotMatrixRow0.v[2]=(u.v[0])*(u.v[2])*(oneMinusCosAngle)+sinAngle*u.v[1];

	rotMatrixRow1.v[0]=(u.v[0])*(u.v[1])*(oneMinusCosAngle)+sinAngle*u.v[2];
	rotMatrixRow1.v[1]=(u.v[1])*(u.v[1])+cosAngle*(1-(u.v[1])*(u.v[1]));
	rotMatrixRow1.v[2]=(u.v[1])*(u.v[2])*(oneMinusCosAngle)-sinAngle*u.v[0];
	
	rotMatrixRow2.v[0]=(u.v[0])*(u.v[2])*(oneMinusCosAngle)-sinAngle*u.v[1];
	rotMatrixRow2.v[1]=(u.v[1])*(u.v[2])*(oneMinusCosAngle)+sinAngle*u.v[0];
	rotMatrixRow2.v[2]=(u.v[2])*(u.v[2])+cosAngle*(1-(u.v[2])*(u.v[2]));

	return Vect3d(this->Dot(rotMatrixRow0),
				  this->Dot(rotMatrixRow1),
				  this->Dot(rotMatrixRow2));
}

void Vect3d::RotateAxis(double angle, const Vect3d & axis)
{
	(*this)=GetRotatedAxis(angle, axis);
}


void Vect3d::Saturate()
{
	(*this)=GetSaturated();	
}

Vect3d Vect3d::GetSaturated() const
{
	Vect3d temp(*this);
	temp.Normalize();
	temp=temp*0.5f+Vect3d(0.5f, 0.5f, 0.5f);
	return temp;
}

Vect3d operator*(float scaleFactor, const Vect3d & rhs)
{
	return rhs*scaleFactor;
}

bool Vect3d::operator==(const Vect3d & rhs) const
{
	if(v[0]==rhs.v[0] && v[1]==rhs.v[1] && v[2]==rhs.v[2])	return true;
													   else return false;
}


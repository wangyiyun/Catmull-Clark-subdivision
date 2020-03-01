#include <memory.h>
#include <math.h>
#include "vect3d.h"
#include "vect4d.h"
#include "matrix4d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//constructor
Matrix4d::Matrix4d(float  m0,float  m1,float  m2,float  m3,
				   float  m4,float  m5,float  m6,float  m7,
				   float  m8,float  m9,float m10,float m11,
				   float m12,float m13,float m14,float m15)
{
	m[0] =m0; m[1] =m1; m[2] =m2; m[3] =m3;
	m[4] =m4; m[5] =m5; m[6] =m6; m[7] =m7;
	m[8] =m8; m[9] =m9; m[10]=m10;m[11]=m11;
	m[12]=m12;m[13]=m13;m[14]=m14;m[15]=m15;
}

//constructor
Matrix4d::Matrix4d(const Matrix4d &rhs)
{
	memcpy(m,rhs.m,16*sizeof(float));
}

Matrix4d::Matrix4d(const float *rhs)
{
	memcpy(m, rhs, 16*sizeof(float));
}

void Matrix4d::SetEntry(int pos,float val)
{
	if(pos>=0 && pos<=15) m[pos]=val;
}
	
float Matrix4d::GetEntry(int pos) const
{
	if(pos>=0 && pos<=15) return m[pos];
				     else return 0.0f;
}

Vect4d Matrix4d::GetRow(int pos) const
{
	switch (pos)
	{
		case 0:	return Vect4d(m[0],m[4],m[8], m[12]);break;
		case 1:	return Vect4d(m[1],m[5],m[9], m[13]);break;
		case 2:	return Vect4d(m[2],m[6],m[10],m[14]);break;
		case 3:	return Vect4d(m[3],m[7],m[11],m[15]);break;
		default: return Vect4d(0.f,0.f,0.f,0.f);
	}
}

Vect4d Matrix4d::GetColumn(int pos) const
{
	switch (pos)
	{
		case 0:	return Vect4d(m[0], m[1], m[2], m[3]);break;
		case 1:	return Vect4d(m[4], m[5], m[6], m[7]);break;
		case 2:	return Vect4d(m[8], m[9], m[10],m[11]);break;
		case 3:	return Vect4d(m[12],m[13],m[14],m[15]);break;
		default:return Vect4d(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

void Matrix4d::Identity(void)
{
	memset(m,0,16*sizeof(float));
	m[0]=m[5]=m[10]=m[15]=1.0f;
}

void Matrix4d::Zero(void)
{
	memset(m,0,16*sizeof(float));
}

Matrix4d Matrix4d::operator+(const Matrix4d & rhs) const		//overloaded operators
{
	return Matrix4d(m[0]+rhs.m[0],
					m[1]+rhs.m[1],
					m[2]+rhs.m[2],
					m[3]+rhs.m[3],
					m[4]+rhs.m[4],
					m[5]+rhs.m[5],
					m[6]+rhs.m[6],
					m[7]+rhs.m[7],
					m[8]+rhs.m[8],
					m[9]+rhs.m[9],
					m[10]+rhs.m[10],
					m[11]+rhs.m[11],
					m[12]+rhs.m[12],
					m[13]+rhs.m[13],
					m[14]+rhs.m[14],
					m[15]+rhs.m[15]);
}

Matrix4d Matrix4d::operator-(const Matrix4d & rhs) const		//overloaded operators
{
	return Matrix4d(m[0]-rhs.m[0],
					m[1]-rhs.m[1],
					m[2]-rhs.m[2],
					m[3]-rhs.m[3],
					m[4]-rhs.m[4],
					m[5]-rhs.m[5],
					m[6]-rhs.m[6],
					m[7]-rhs.m[7],
					m[8]-rhs.m[8],
					m[9]-rhs.m[9],
					m[10]-rhs.m[10],
					m[11]-rhs.m[11],
					m[12]-rhs.m[12],
					m[13]-rhs.m[13],
					m[14]-rhs.m[14],
					m[15]-rhs.m[15]);
}

Matrix4d Matrix4d::operator*(const Matrix4d & rhs) const
{
	//Optimize for matrices in which bottom row is (0, 0, 0, 1) in both matrices
	if(	m[3]==0.0f && m[7]==0.0f && m[11]==0.0f && m[15]==1.0f	&&
		rhs.m[3]==0.0f && rhs.m[7]==0.0f &&
		rhs.m[11]==0.0f && rhs.m[15]==1.0f)
	{
		return Matrix4d(	m[0]*rhs.m[0]+m[4]*rhs.m[1]+m[8]*rhs.m[2],
							m[1]*rhs.m[0]+m[5]*rhs.m[1]+m[9]*rhs.m[2],
							m[2]*rhs.m[0]+m[6]*rhs.m[1]+m[10]*rhs.m[2],
							0.0f,
							m[0]*rhs.m[4]+m[4]*rhs.m[5]+m[8]*rhs.m[6],
							m[1]*rhs.m[4]+m[5]*rhs.m[5]+m[9]*rhs.m[6],
							m[2]*rhs.m[4]+m[6]*rhs.m[5]+m[10]*rhs.m[6],
							0.0f,
							m[0]*rhs.m[8]+m[4]*rhs.m[9]+m[8]*rhs.m[10],
							m[1]*rhs.m[8]+m[5]*rhs.m[9]+m[9]*rhs.m[10],
							m[2]*rhs.m[8]+m[6]*rhs.m[9]+m[10]*rhs.m[10],
							0.0f,
							m[0]*rhs.m[12]+m[4]*rhs.m[13]+m[8]*rhs.m[14]+m[12],
							m[1]*rhs.m[12]+m[5]*rhs.m[13]+m[9]*rhs.m[14]+m[13],
							m[2]*rhs.m[12]+m[6]*rhs.m[13]+m[10]*rhs.m[14]+m[14],
							1.0f);
	}

	//Optimise for when bottom row of 1st matrix is (0, 0, 0, 1)
	if(	m[3]==0.0f && m[7]==0.0f && m[11]==0.0f && m[15]==1.0f)
	{
		return Matrix4d(	m[0]*rhs.m[0]+m[4]*rhs.m[1]+m[8]*rhs.m[2]+m[12]*rhs.m[3],
							m[1]*rhs.m[0]+m[5]*rhs.m[1]+m[9]*rhs.m[2]+m[13]*rhs.m[3],
							m[2]*rhs.m[0]+m[6]*rhs.m[1]+m[10]*rhs.m[2]+m[14]*rhs.m[3],
							rhs.m[3],
							m[0]*rhs.m[4]+m[4]*rhs.m[5]+m[8]*rhs.m[6]+m[12]*rhs.m[7],
							m[1]*rhs.m[4]+m[5]*rhs.m[5]+m[9]*rhs.m[6]+m[13]*rhs.m[7],
							m[2]*rhs.m[4]+m[6]*rhs.m[5]+m[10]*rhs.m[6]+m[14]*rhs.m[7],
							rhs.m[7],
							m[0]*rhs.m[8]+m[4]*rhs.m[9]+m[8]*rhs.m[10]+m[12]*rhs.m[11],
							m[1]*rhs.m[8]+m[5]*rhs.m[9]+m[9]*rhs.m[10]+m[13]*rhs.m[11],
							m[2]*rhs.m[8]+m[6]*rhs.m[9]+m[10]*rhs.m[10]+m[14]*rhs.m[11],
							rhs.m[11],
							m[0]*rhs.m[12]+m[4]*rhs.m[13]+m[8]*rhs.m[14]+m[12]*rhs.m[15],
							m[1]*rhs.m[12]+m[5]*rhs.m[13]+m[9]*rhs.m[14]+m[13]*rhs.m[15],
							m[2]*rhs.m[12]+m[6]*rhs.m[13]+m[10]*rhs.m[14]+m[14]*rhs.m[15],
							rhs.m[15]);
	}

	//Optimise for when bottom row of 2nd matrix is (0, 0, 0, 1)
	if(	rhs.m[3]==0.0f && rhs.m[7]==0.0f &&
		rhs.m[11]==0.0f && rhs.m[15]==1.0f)
	{
		return Matrix4d(	m[0]*rhs.m[0]+m[4]*rhs.m[1]+m[8]*rhs.m[2],
							m[1]*rhs.m[0]+m[5]*rhs.m[1]+m[9]*rhs.m[2],
							m[2]*rhs.m[0]+m[6]*rhs.m[1]+m[10]*rhs.m[2],
							m[3]*rhs.m[0]+m[7]*rhs.m[1]+m[11]*rhs.m[2],
							m[0]*rhs.m[4]+m[4]*rhs.m[5]+m[8]*rhs.m[6],
							m[1]*rhs.m[4]+m[5]*rhs.m[5]+m[9]*rhs.m[6],
							m[2]*rhs.m[4]+m[6]*rhs.m[5]+m[10]*rhs.m[6],
							m[3]*rhs.m[4]+m[7]*rhs.m[5]+m[11]*rhs.m[6],
							m[0]*rhs.m[8]+m[4]*rhs.m[9]+m[8]*rhs.m[10],
							m[1]*rhs.m[8]+m[5]*rhs.m[9]+m[9]*rhs.m[10],
							m[2]*rhs.m[8]+m[6]*rhs.m[9]+m[10]*rhs.m[10],
							m[3]*rhs.m[8]+m[7]*rhs.m[9]+m[11]*rhs.m[10],
							m[0]*rhs.m[12]+m[4]*rhs.m[13]+m[8]*rhs.m[14]+m[12],
							m[1]*rhs.m[12]+m[5]*rhs.m[13]+m[9]*rhs.m[14]+m[13],
							m[2]*rhs.m[12]+m[6]*rhs.m[13]+m[10]*rhs.m[14]+m[14],
							m[3]*rhs.m[12]+m[7]*rhs.m[13]+m[11]*rhs.m[14]+m[15]);
	}	
	
	return Matrix4d(	m[0]*rhs.m[0]+m[4]*rhs.m[1]+m[8]*rhs.m[2]+m[12]*rhs.m[3],
						m[1]*rhs.m[0]+m[5]*rhs.m[1]+m[9]*rhs.m[2]+m[13]*rhs.m[3],
						m[2]*rhs.m[0]+m[6]*rhs.m[1]+m[10]*rhs.m[2]+m[14]*rhs.m[3],
						m[3]*rhs.m[0]+m[7]*rhs.m[1]+m[11]*rhs.m[2]+m[15]*rhs.m[3],
						m[0]*rhs.m[4]+m[4]*rhs.m[5]+m[8]*rhs.m[6]+m[12]*rhs.m[7],
						m[1]*rhs.m[4]+m[5]*rhs.m[5]+m[9]*rhs.m[6]+m[13]*rhs.m[7],
						m[2]*rhs.m[4]+m[6]*rhs.m[5]+m[10]*rhs.m[6]+m[14]*rhs.m[7],
						m[3]*rhs.m[4]+m[7]*rhs.m[5]+m[11]*rhs.m[6]+m[15]*rhs.m[7],
						m[0]*rhs.m[8]+m[4]*rhs.m[9]+m[8]*rhs.m[10]+m[12]*rhs.m[11],
						m[1]*rhs.m[8]+m[5]*rhs.m[9]+m[9]*rhs.m[10]+m[13]*rhs.m[11],
						m[2]*rhs.m[8]+m[6]*rhs.m[9]+m[10]*rhs.m[10]+m[14]*rhs.m[11],
						m[3]*rhs.m[8]+m[7]*rhs.m[9]+m[11]*rhs.m[10]+m[15]*rhs.m[11],
						m[0]*rhs.m[12]+m[4]*rhs.m[13]+m[8]*rhs.m[14]+m[12]*rhs.m[15],
						m[1]*rhs.m[12]+m[5]*rhs.m[13]+m[9]*rhs.m[14]+m[13]*rhs.m[15],
						m[2]*rhs.m[12]+m[6]*rhs.m[13]+m[10]*rhs.m[14]+m[14]*rhs.m[15],
						m[3]*rhs.m[12]+m[7]*rhs.m[13]+m[11]*rhs.m[14]+m[15]*rhs.m[15]);
}

Matrix4d Matrix4d::operator*(const float rhs) const
{
	return Matrix4d(m[0]*rhs, m[1]*rhs, m[2]*rhs, m[3]*rhs,
					m[4]*rhs, m[5]*rhs, m[6]*rhs, m[7]*rhs,
					m[8]*rhs, m[9]*rhs, m[10]*rhs,m[11]*rhs,
					m[12]*rhs,m[13]*rhs,m[14]*rhs,m[15]*rhs);
}

Matrix4d Matrix4d::operator/(const float rhs) const
{
	if (rhs==0.0f||rhs==1.0f)	return (*this);
	return (*this)*1.f/rhs;
}

Matrix4d operator*(float scaleFactor, const Matrix4d & rhs)
{
	return rhs*scaleFactor;
}

bool Matrix4d::operator==(const Matrix4d & rhs) const
{
	for(int i=0;i<16;i++)
	{
		if(m[i]!=rhs.m[i])return false;
	}
	return true;
}

bool Matrix4d::operator!=(const Matrix4d & rhs) const
{
	return !((*this)==rhs);
}

void Matrix4d::operator+=(const Matrix4d & rhs)
{
	(*this)=(*this)+rhs;
}

void Matrix4d::operator-=(const Matrix4d & rhs)
{
	(*this)=(*this)-rhs;
}

void Matrix4d::operator*=(const Matrix4d & rhs)
{
	(*this)=(*this)*rhs;
}

void Matrix4d::operator*=(const float rhs)
{
	(*this)=(*this)*rhs;
}

void Matrix4d::operator/=(const float rhs)
{
	(*this)=(*this)/rhs;
}

Matrix4d Matrix4d::operator-(void) const
{
	Matrix4d result(*this);

	for(int i=0; i<16; i++)	result.m[i]=-result.m[i];
	return result;
}

Vect4d Matrix4d::operator*(const Vect4d rhs) const
{
	//Optimize for matrices in which bottom row is (0, 0, 0, 1)
	if(m[3]==0.0f && m[7]==0.0f && m[11]==0.0f && m[15]==1.0f)
	{
		return Vect4d(m[0]*rhs.v[0]+m[4]*rhs.v[1]+m[8]*rhs.v[2]+m[12]*rhs.v[3],
					  m[1]*rhs.v[0]+m[5]*rhs.v[1]+m[9]*rhs.v[2]+m[13]*rhs.v[3],
					  m[2]*rhs.v[0]+m[6]*rhs.v[1]+m[10]*rhs.v[2]+m[14]*rhs.v[3],
					  rhs.v[3]);
	}
	
	return Vect4d(m[0]*rhs.v[0]+m[4]*rhs.v[1]+m[8]*rhs.v[2]+m[12]*rhs.v[3],
				  m[1]*rhs.v[0]+m[5]*rhs.v[1]+m[9]*rhs.v[2]+m[13]*rhs.v[3],
				  m[2]*rhs.v[0]+m[6]*rhs.v[1]+m[10]*rhs.v[2]+m[14]*rhs.v[3],
				  m[3]*rhs.v[0]+m[7]*rhs.v[1]+m[11]*rhs.v[2]+m[15]*rhs.v[3]);
}

Vect3d Matrix4d::GetRotatedVector3D(const Vect3d & rhs) const
{
	return Vect3d(m[0]*rhs.v[0]+m[4]*rhs.v[1]+ m[8]*rhs.v[2],
				  m[1]*rhs.v[0]+m[5]*rhs.v[1]+ m[9]*rhs.v[2],
				  m[2]*rhs.v[0]+m[6]*rhs.v[1]+m[10]*rhs.v[2]);
}

Vect3d Matrix4d::GetInverseRotatedVector3D(const Vect3d & rhs) const
{
	//rotate by transpose:
	return Vect3d(m[0]*rhs.v[0]+m[1]*rhs.v[1]+ m[2]*rhs.v[2],
				  m[4]*rhs.v[0]+m[5]*rhs.v[1]+ m[6]*rhs.v[2],
				  m[8]*rhs.v[0]+m[9]*rhs.v[1]+m[10]*rhs.v[2]);
}

Vect3d Matrix4d::GetTranslatedVector3D(const Vect3d & rhs) const
{
	return Vect3d(rhs.v[0]+m[12],rhs.v[1]+m[13],rhs.v[2]+m[14]);
}

Vect3d Matrix4d::GetInverseTranslatedVector3D(const Vect3d & rhs) const
{
	return Vect3d(rhs.v[0]-m[12],rhs.v[1]-m[13],rhs.v[2]-m[14]);
}

void Matrix4d::Invert(void)
{
	*this=GetInverse();
}

Matrix4d Matrix4d::GetInverse(void) const
{
	Matrix4d result=GetInverseTranspose();
	result.Transpose();
	return result;
}


void Matrix4d::Transpose(void)
{
	*this=GetTranspose();
}

Matrix4d Matrix4d::GetTranspose(void) const
{
	return Matrix4d(m[0],m[4], m[8],m[12],
					m[1],m[5], m[9],m[13],
					m[2],m[6],m[10],m[14],
					m[3],m[7],m[11],m[15]);
}

void Matrix4d::InvertTranspose(void)
{
	*this=GetInverseTranspose();
}

Matrix4d Matrix4d::GetInverseTranspose(void) const
{
	Matrix4d result;

	float tmp[12];												//temporary pair storage
	float det;													//determinant

	//calculate pairs for first 8 elements (cofactors)
	tmp[0]=m[10]*m[15];
	tmp[1]=m[11]*m[14];
	tmp[2]=m[9] *m[15];
	tmp[3]=m[11]*m[13];
	tmp[4]=m[9] *m[14];
	tmp[5]=m[10]*m[13];
	tmp[6]=m[8] *m[15];
	tmp[7]=m[11]*m[12];
	tmp[8]=m[8] *m[14];
	tmp[9]=m[10]*m[12];
	tmp[10]=m[8]*m[13];
	tmp[11]=m[9]*m[12];

	//calculate first 8 elements (cofactors)
	result.SetEntry(0,tmp[0]*m[5]+tmp[3]*m[6]+tmp[4] *m[7]-tmp[1]*m[5]-tmp[2]*m[6]-tmp[5] *m[7]);
	result.SetEntry(1,tmp[1]*m[4]+tmp[6]*m[6]+tmp[9] *m[7]-tmp[0]*m[4]-tmp[7]*m[6]-tmp[8] *m[7]);
	result.SetEntry(2,tmp[2]*m[4]+tmp[7]*m[5]+tmp[10]*m[7]-tmp[3]*m[4]-tmp[6]*m[5]-tmp[11]*m[7]);
	result.SetEntry(3,tmp[5]*m[4]+tmp[8]*m[5]+tmp[11]*m[6]-tmp[4]*m[4]-tmp[9]*m[5]-tmp[10]*m[6]);
	result.SetEntry(4,tmp[1]*m[1]+tmp[2]*m[2]+tmp[5]* m[3]-tmp[0]*m[1]-tmp[3]*m[2]-tmp[4]* m[3]);
	result.SetEntry(5,tmp[0]*m[0]+tmp[7]*m[2]+tmp[8]* m[3]-tmp[1]*m[0]-tmp[6]*m[2]-tmp[9]* m[3]);
	result.SetEntry(6,tmp[3]*m[0]+tmp[6]*m[1]+tmp[11]*m[3]-tmp[2]*m[0]-tmp[7]*m[1]-tmp[10]*m[3]);
	result.SetEntry(7,tmp[4]*m[0]+tmp[9]*m[1]+tmp[10]*m[2]-tmp[5]*m[0]-tmp[8]*m[1]-tmp[11]*m[2]);

	//calculate pairs for second 8 elements (cofactors)
	tmp[0] =m[2]*m[7];
	tmp[1] =m[3]*m[6];
	tmp[2] =m[1]*m[7];
	tmp[3] =m[3]*m[5];
	tmp[4] =m[1]*m[6];
	tmp[5] =m[2]*m[5];
	tmp[6] =m[0]*m[7];
	tmp[7] =m[3]*m[4];
	tmp[8] =m[0]*m[6];
	tmp[9] =m[2]*m[4];
	tmp[10]=m[0]*m[5];
	tmp[11]=m[1]*m[4];

	//calculate second 8 elements (cofactors)
	result.SetEntry(8, tmp[0]*m[13]+tmp[3] *m[14]+tmp[4] *m[15]-tmp[1] *m[13]-tmp[2] *m[14]-tmp[5] *m[15]);
	result.SetEntry(9, tmp[1]*m[12]+tmp[6] *m[14]+tmp[9] *m[15]-tmp[0] *m[12]-tmp[7] *m[14]-tmp[8] *m[15]);
	result.SetEntry(10,tmp[2]*m[12]+tmp[7] *m[13]+tmp[10]*m[15]-tmp[3] *m[12]-tmp[6] *m[13]-tmp[11]*m[15]);
	result.SetEntry(11,tmp[5]*m[12]+tmp[8] *m[13]+tmp[11]*m[14]-tmp[4] *m[12]-tmp[9] *m[13]-tmp[10]*m[14]);
	result.SetEntry(12,tmp[2]*m[10]+tmp[5] *m[11]+tmp[1] *m[9]- tmp[4] *m[11]-tmp[0] *m[9]- tmp[3] *m[10]);
	result.SetEntry(13,tmp[8]*m[11]+tmp[0] *m[8]+ tmp[7] *m[10]-tmp[6] *m[10]-tmp[9] *m[11]-tmp[1] *m[8]);
	result.SetEntry(14,tmp[6]* m[9]+tmp[11]*m[11]+tmp[3] *m[8]- tmp[10]*m[11]-tmp[2] *m[8]- tmp[7] *m[9]);
	result.SetEntry(15,tmp[10]*m[10]+tmp[4]*m[8]+ tmp[9] *m[9]- tmp[8] *m[9]- tmp[11]*m[10]-tmp[5] *m[8]);

	// calculate determinant
	det=m[0]*result.GetEntry(0)+m[1]*result.GetEntry(1)+m[2]*result.GetEntry(2)+m[3]*result.GetEntry(3);

	if(det==0.0f)
	{
		Matrix4d id;
		return id;
	}
	result=result/det;
	return result;
}

//Invert if only composed of rotations & translations
void Matrix4d::AffineInvert(void)
{
	(*this)=GetAffineInverse();
}

Matrix4d Matrix4d::GetAffineInverse(void) const
{
	//return the transpose of the rotation part
	//and the negative of the inverse rotated translation part
	return Matrix4d(m[0],m[4],m[8], 0.0f,
					m[1],m[5],m[9], 0.0f,
					m[2],m[6],m[10],0.0f,
					-(m[0]*m[12]+m[1]*m[13]+m[2]*m[14]),-(m[4]*m[12]+m[5]*m[13]+m[6]*m[14]),-(m[8]*m[12]+m[9]*m[13]+m[10]*m[14]),1.0f);
}

void Matrix4d::AffineInvertTranspose(void)
{
	(*this)=GetAffineInverseTranspose();
}

Matrix4d Matrix4d::GetAffineInverseTranspose(void) const
{
	//return the transpose of the rotation part
	//and the negative of the inverse rotated translation part
	//transposed
	return Matrix4d(m[0],m[1],m[2],-(m[0]*m[12]+m[1]*m[13]+m[2]*m[14]),
					m[4],m[5],m[6],-(m[4]*m[12]+m[5]*m[13]+m[6]*m[14]),
					m[8],m[9],m[10],-(m[8]*m[12]+m[9]*m[13]+m[10]*m[14]),
					0.0f, 0.0f, 0.0f, 1.0f);
}

void Matrix4d::SetTranslation(const Vect3d & translation)
{
	Identity();
	SetTranslationPart(translation);
}

void Matrix4d::SetScale(const Vect3d & scaleFactor)
{
	Identity();
	m[0] =scaleFactor.v[0];
	m[5] =scaleFactor.v[1];
	m[10]=scaleFactor.v[2];
}

void Matrix4d::SetUniformScale(const float scaleFactor)
{
	Identity();
	m[0]=m[5]=m[10]=scaleFactor;
}

void Matrix4d::SetRotationAxis(const double angle, const Vect3d & axis)
{
	Vect3d u=axis.GetNormalized();
	float sinAngle=(float)sin(M_PI*angle/180);
	float cosAngle=(float)cos(M_PI*angle/180);
	float oneMinusCosAngle=1.0f-cosAngle;

	Identity();
	m[0]=(u.v[0])*(u.v[0]) + cosAngle*(1-(u.v[0])*(u.v[0]));
	m[4]=(u.v[0])*(u.v[1])*(oneMinusCosAngle) - sinAngle*u.v[2];
	m[8]=(u.v[0])*(u.v[2])*(oneMinusCosAngle) + sinAngle*u.v[1];

	m[1]=(u.v[0])*(u.v[1])*(oneMinusCosAngle) + sinAngle*u.v[2];
	m[5]=(u.v[1])*(u.v[1]) + cosAngle*(1-(u.v[1])*(u.v[1]));
	m[9]=(u.v[1])*(u.v[2])*(oneMinusCosAngle) - sinAngle*u.v[0];
	
	m[2]=(u.v[0])*(u.v[2])*(oneMinusCosAngle) - sinAngle*u.v[1];
	m[6]=(u.v[1])*(u.v[2])*(oneMinusCosAngle) + sinAngle*u.v[0];
	m[10]=(u.v[2])*(u.v[2]) + cosAngle*(1-(u.v[2])*(u.v[2]));
}

void Matrix4d::SetRotationX(const double angle)
{
	Identity();
	m[5]=(float)cos(M_PI*angle/180);
	m[6]=(float)sin(M_PI*angle/180);
	m[9]=-m[6];
	m[10]=m[5];
}

void Matrix4d::SetRotationY(const double angle)
{
	Identity();
	m[0]=(float)cos(M_PI*angle/180);
	m[2]=-(float)sin(M_PI*angle/180);
	m[8]=-m[2];
	m[10]=m[0];
}

void Matrix4d::SetRotationZ(const double angle)
{
	Identity();
	m[0]=(float)cos(M_PI*angle/180);
	m[1]=(float)sin(M_PI*angle/180);

	m[4]=-m[1];
	m[5]=m[0];
}

void Matrix4d::SetRotationEuler(const double angleX, const double angleY, const double angleZ)
{
	Identity();
	SetRotationPartEuler(angleX, angleY, angleZ);
}

void Matrix4d::SetPerspective(float left,float right,float bottom,
							  float top,float n,float f)
{
	float nudge=0.999f;		//prevent artifacts with infinite far plane

	Zero();

	//check for division by 0
	if(left==right || top==bottom || n==f)	return;

	m[0]=(2*n)/(right-left);
	m[5]=(2*n)/(top-bottom);
	m[8]=(right+left)/(right-left);
	m[9]=(top+bottom)/(top-bottom);

	if(f!=-1)m[10]=-(f+n)/(f-n);
//if f==-1, use an infinite far plane	
	else m[10]=-nudge;
	

	m[11]=-1;
	if(f!=-1) m[14]=-(2*f*n)/(f-n);
	//if f==-1, use an infinite far plane
	else m[14]=-2*n*nudge;
}

void Matrix4d::SetPerspective(float fovy, float aspect, float n, float f)
{
	float left, right, top, bottom;

	//convert fov from degrees to radians
	fovy*=(float)M_PI/180;

	top=n*tanf(fovy/2.0f);
	bottom=-top;

	left=aspect*bottom;
	right=aspect*top;

	SetPerspective(left, right, bottom, top, n, f);
}

void Matrix4d::SetOrtho(float left,float right,
						float bottom,float top, 
						float n, float f)
{
	Identity();
	m[0]=2.0f/(right-left);
	m[5]=2.0f/(top-bottom);
	m[10]=-2.0f/(f-n);
	m[12]=-(right+left)/(right-left);
	m[13]=-(top+bottom)/(top-bottom);
	m[14]=-(f+n)/(f-n);
}

void Matrix4d::SetTranslationPart(const Vect3d & translation)
{
	m[12]=translation.v[0];
	m[13]=translation.v[1];
	m[14]=translation.v[2];
}

void Matrix4d::SetRotationPartEuler(const double angleX, const double angleY, const double angleZ)
{
	double cr = cos( M_PI*angleX/180 );
	double sr = sin( M_PI*angleX/180 );
	double cp = cos( M_PI*angleY/180 );
	double sp = sin( M_PI*angleY/180 );
	double cy = cos( M_PI*angleZ/180 );
	double sy = sin( M_PI*angleZ/180 );

	m[0] = ( float )( cp*cy );
	m[1] = ( float )( cp*sy );
	m[2] = ( float )( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	m[4] = ( float )( srsp*cy-cr*sy );
	m[5] = ( float )( srsp*sy+cr*cy );
	m[6] = ( float )( sr*cp );

	m[8] = ( float )( crsp*cy+sr*sy );
	m[9] = ( float )( crsp*sy-sr*cy );
	m[10] = ( float )( cr*cp );
}

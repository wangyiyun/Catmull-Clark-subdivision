#ifndef __matrix4d_h__
#define __matrix4d_h__

#include "vect3d.h"
#include "vect4d.h"


class Matrix4d
{
public:
	//constructors
	Matrix4d(){Identity();}
	Matrix4d(float  m0,float  m1,float  m2,float  m3,
             float  m4,float  m5,float  m6,float  m7,
			 float  m8,float  m9,float m10,float m11,
			 float m12,float m13,float m14,float m15);
	Matrix4d(const float *rhs);
	Matrix4d(const Matrix4d &rhs);
	//destructor
	~Matrix4d() {}

	void SetEntry(int pos,float value);
	float GetEntry(int pos) const;
	Vect4d GetRow(int pos) const;
	Vect4d GetColumn(int pos) const;
	
	void Identity(void);
	void Zero(void);
	
	//binary operators
	Matrix4d operator+(const Matrix4d & rhs) const;
	Matrix4d operator-(const Matrix4d & rhs) const;
	Matrix4d operator*(const Matrix4d & rhs) const;
	Matrix4d operator*(const float rhs) const;
	Matrix4d operator/(const float rhs) const;
	friend Matrix4d operator*(float scaleFactor, const Matrix4d & rhs);

	bool operator==(const Matrix4d & rhs) const;
	bool operator!=(const Matrix4d & rhs) const;

	//self-add etc
	void operator+=(const Matrix4d & rhs);
	void operator-=(const Matrix4d & rhs);
	void operator*=(const Matrix4d & rhs);
	void operator*=(const float rhs);
	void operator/=(const float rhs);

	//unary operators
	Matrix4d operator-(void) const;
	Matrix4d operator+(void) const {return (*this);}
	
	//multiply a vector by this matrix
	Vect4d operator*(const Vect4d rhs) const;

	//rotate a 3d vector by rotation part
	void RotateVector3D(Vect3d & rhs) const
	{rhs=GetRotatedVector3D(rhs);}

	void InverseRotateVector3D(Vect3d & rhs) const
	{rhs=GetInverseRotatedVector3D(rhs);}

	Vect3d GetRotatedVector3D(const Vect3d & rhs) const;
	Vect3d GetInverseRotatedVector3D(const Vect3d & rhs) const;

	//translate a 3d vector by translation part
	void TranslateVector3D(Vect3d & rhs) const
	{rhs=GetTranslatedVector3D(rhs);}

	void InverseTranslateVector3D(Vect3d & rhs) const
	{rhs=GetInverseTranslatedVector3D(rhs);}
	
	Vect3d GetTranslatedVector3D(const Vect3d & rhs) const;
	Vect3d GetInverseTranslatedVector3D(const Vect3d & rhs) const;

	//Other methods
	void Invert(void);
	Matrix4d GetInverse(void) const;
	void Transpose(void);
	Matrix4d GetTranspose(void) const;
	void InvertTranspose(void);
	Matrix4d GetInverseTranspose(void) const;

	//Inverse of a rotation/translation only matrix
	void AffineInvert(void);
	Matrix4d GetAffineInverse(void) const;
	void AffineInvertTranspose(void);
	Matrix4d GetAffineInverseTranspose(void) const;

	//set to perform an operation on space - removes other entries
	void SetTranslation(const Vect3d & translation);
	void SetScale(const Vect3d & scaleFactor);
	void SetUniformScale(const float scaleFactor);
	void SetRotationAxis(const double angle, const Vect3d & axis);
	void SetRotationX(const double angle);
	void SetRotationY(const double angle);
	void SetRotationZ(const double angle);
	void SetRotationEuler(const double angleX, const double angleY, const double angleZ);
	void SetPerspective(float left, float right, float bottom, float top, float n, float f);
	void SetPerspective(float fovy, float aspect, float n, float f);
	void SetOrtho(float left, float right, float bottom, float top, float n, float f);

	//set parts of the matrix
	void SetTranslationPart(const Vect3d & translation);
	void SetRotationPartEuler(const double angleX, const double angleY, const double angleZ);
	void SetRotationPartEuler(const Vect3d & rotations)
	{
		SetRotationPartEuler((double)rotations.v[0], (double)rotations.v[1], (double)rotations.v[2]);
	}

	//cast to pointer to a (float *) for glGetFloatv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}
	
	//member variables
	float m[16];
};

#endif	//Matrix4d_H
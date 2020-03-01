#ifndef __VECT3D_H__
#define __VECT3D_H__

#include <memory.h>

class Vect3d
{
public:
	//constructors
	Vect3d(void);

	Vect3d(float x,float y,float z);

	Vect3d(const float *newv);

	Vect3d(const Vect3d &newv);

	//destructor
	~Vect3d() {}	//empty

	inline void Set(float x,float y,float z)
	{v[0]=x;v[1]=y;v[2]=z;}

	inline void Set(const Vect3d &p)
	{memcpy(v,p,3*sizeof(float));}
	
	//Accessors kept for compatibility
	inline void SetX(float x) {v[0]=x;}
	inline void SetY(float y) {v[1]=y;}
	inline void SetZ(float z) {v[2]=z;}

	float GetX() const {return v[0];}	//public accessor functions
	float GetY() const {return v[1];}	//inline, const
	float GetZ() const {return v[2];}

	float x() const {return v[0];}	//public accessor functions
	float y() const {return v[1];}	//inline, const
	float z() const {return v[2];}


	void Zero(void);

	void One(void);
	
	//vector algebra
	inline Vect3d Cross(const Vect3d &rhs) const
	{return Vect3d(v[1]*rhs.v[2]-v[2]*rhs.v[1],v[2]*rhs.v[0]-v[0]*rhs.v[2],v[0]*rhs.v[1]-v[1]*rhs.v[0]);}

	inline static Vect3d Cross(const Vect3d &a, const Vect3d &b)
	{return Vect3d(a.v[1]*b.v[2]-a.v[2]*b.v[1],a.v[2]*b.v[0]-a.v[0]*b.v[2],a.v[0]*b.v[1]-a.v[1]*b.v[0]);}

	float Dot(const Vect3d &rhs) const
	{return v[0]*rhs.v[0]+v[1]*rhs.v[1]+v[2]*rhs.v[2];}
	
	void Normalize();
	Vect3d GetNormalized() const;
	
	float Length() const;

	
	float SquaredLength() const
	{return (v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]);}

	//rotations
	void RotateX(double angle);
	Vect3d GetRotatedX(double angle) const;
	void RotateY(double angle);
	Vect3d GetRotatedY(double angle) const;
	void RotateZ(double angle);
	Vect3d GetRotatedZ(double angle) const;
	void RotateAxis(double angle, const Vect3d & axis);
	Vect3d GetRotatedAxis(double angle, const Vect3d & axis) const;

	//pack to [0,1] for color
	void Saturate();
	Vect3d GetSaturated() const;

	//linear interpolate
	Vect3d lerp(const Vect3d & v2, float factor) const
	{return (*this)*(1.0f-factor)+v2*factor;}

	Vect3d QuadraticInterpolate(const Vect3d & v2, const Vect3d & v3, float factor) const
	{return (*this)*(1.0f-factor)*(1.0f-factor)+2*v2*factor*(1.0f-factor)+v3*factor*factor;}

	//overloaded operators
	//binary operators
	Vect3d operator+(const Vect3d & rhs) const
	{return Vect3d(v[0]+rhs.v[0],v[1]+rhs.v[1],v[2]+rhs.v[2]);}
	
	Vect3d operator-(const Vect3d & rhs) const
	{return Vect3d(v[0]-rhs.v[0],v[1]-rhs.v[1],v[2]-rhs.v[2]); }

	Vect3d operator*(const float rhs) const
	{return Vect3d(v[0]*rhs,v[1]*rhs,v[2]*rhs);}
	

	Vect3d operator/(const float rhs) const
	{	return (rhs==0.0f) ? Vect3d(0.0f, 0.0f, 0.0f) : Vect3d(v[0]/rhs,v[1]/rhs,v[2]/rhs);	}

	//multiply by a float, eg 3*v
	friend Vect3d operator*(float scaleFactor, const Vect3d & rhs);

	//Add, subtract etc, saving the construction of a temporary
	void Add(const Vect3d & v2, Vect3d & result)
	{
		result.v[0]=v[0]+v2.v[0];
		result.v[1]=v[1]+v2.v[1];
		result.v[2]=v[2]+v2.v[2];
	}

	void Subtract(const Vect3d & v2, Vect3d & result)
	{
		result.v[0]=v[0]-v2.v[0];
		result.v[1]=v[1]-v2.v[1];
		result.v[2]=v[2]-v2.v[2];
	}

	bool operator==(const Vect3d & rhs) const;
	bool operator!=(const Vect3d & rhs) const
	{	return !((*this)==rhs);	}

	//self-add etc
	void operator+=(const Vect3d & rhs)
	{	
		v[0]+=rhs.v[0];	
		v[1]+=rhs.v[1];
		v[2]+=rhs.v[2];
	}

	void operator-=(const Vect3d & rhs)
	{			
		v[0]-=rhs.v[0];	
		v[1]-=rhs.v[1];
		v[2]-=rhs.v[2];
}

	void operator*=(const float rhs)
	{	
		v[0]*=rhs;
		v[1]*=rhs;
		v[2]*=rhs;	
	}
	
	void operator/=(const float rhs)
	{	
		if(rhs==0.0f) return;
		else
		{
			v[0]/=rhs;
			v[1]/=rhs;
			v[2]/=rhs;	
		}
	}


	//unary operators
	Vect3d operator-(void) const {return Vect3d(-v[0], -v[1], -v[2]);}
	Vect3d operator+(void) const {return *this;}

	//cast to pointer to a (float *) for glVertex3fv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}

//protected:
	//member variables
	float v[3];
};

#endif	//Vect3d_H
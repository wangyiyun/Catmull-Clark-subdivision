#ifndef __VECT4D_H__
#define __VECT4D_H__



class Vect4d
{
public:
	//constructors
	Vect4d(void);

	Vect4d(float x,float y,float z,float w);	

	Vect4d(const float *newv);

	Vect4d(const Vect4d &newv);

	//convert v3d to v4d
	Vect4d(const Vect3d & rhs)
	{
		v[0]=rhs.v[0];v[1]=rhs.v[1];v[2]=rhs.v[2];v[3]=1.f;
	}

	//destructor
	~Vect4d() {}	//empty

	void Set(float newX, float newY, float newZ, float newW)
	{v[0]=newX;v[1]=newY;v[2]=newZ;v[3]=newW;}
	
	//accessors kept for compatability
	void SetX(float newX) {v[0]=newX;}
	void SetY(float newY) {v[1]=newY;}
	void SetZ(float newZ) {v[2]=newZ;}
	void SetW(float newW) {v[3]=newW;}
	
	float GetX() const {return v[0];}	//public accessor functions
	float GetY() const {return v[1];}	//inline, const
	float GetZ() const {return v[2];}
	float GetW() const {return v[3];}

	void Zero(void);

	void One(void);

	//vector algebra
	float Dot(const Vect4d & rhs)
	{
		return v[0]*rhs.v[0]+v[1]*rhs.v[1]+v[2]*rhs.v[2]+v[3]*rhs.v[3];
	}

	//rotations
	void RotateX(double angle);
	Vect4d GetRotatedX(double angle) const;
	void RotateY(double angle);
	Vect4d GetRotatedY(double angle) const;
	void RotateZ(double angle);
	Vect4d GetRotatedZ(double angle) const;
	void RotateAxis(double angle, const Vect3d & axis);
	Vect4d GetRotatedAxis(double angle, const Vect3d & axis) const;
	
	Vect4d lerp(const Vect4d & v2, float factor) const
	{	
		return (*this)*(1.0f-factor)+v2*factor;
	}

	Vect4d QuadraticInterpolate(const Vect4d & v2, const Vect4d & v3, float factor) const
	{	
		return (*this)*(1.0f-factor)*(1.0f-factor) + 2*v2*factor*(1.0f-factor) + v3*factor*factor;
	}

	//binary operators
	Vect4d operator+(const Vect4d & rhs) const
	{	
		return Vect4d(v[0]+rhs.v[0],v[1]+rhs.v[1],v[2]+rhs.v[2],v[3]+rhs.v[3]);	
	}

	Vect4d operator-(const Vect4d & rhs) const
	{	
		return Vect4d(v[0]-rhs.v[0],v[1]-rhs.v[1],v[2]-rhs.v[2],v[3]-rhs.v[3]);	
	}

	Vect4d operator*(const float rhs) const
	{	
		return Vect4d(v[0]*rhs,v[1]*rhs,v[2]*rhs,v[3]*rhs);	
	}

	Vect4d operator/(const float rhs) const
	{	
		return rhs==0.0f?Vect4d(0.0f,0.0f,0.0f,0.0f):Vect4d(v[0]/rhs,v[1]/rhs,v[2]/rhs,v[3]/rhs);	
	}

	//multiply by a float, eg 3*v
	friend Vect4d operator*(float scaleFactor, const Vect4d & rhs);

	bool operator==(const Vect4d & rhs) const;
	bool operator!=(const Vect4d & rhs) const
	{	return !((*this)==rhs);	}

	//self-add etc
	void operator+=(const Vect4d & rhs)
	{	
		v[0]+=rhs.v[0];v[1]+=rhs.v[1];v[2]+=rhs.v[2];v[3]+=rhs.v[3];	
	}

	void operator-=(const Vect4d & rhs)
	{	
		v[0]-=rhs.v[0];v[1]-=rhs.v[1];v[2]-=rhs.v[2];v[3]-=rhs.v[3];	
	}

	void operator*=(const float rhs)
	{	
		v[0]*=rhs; 
		v[1]*=rhs; 
		v[2]*=rhs; 
		v[3]*=rhs;	
	}

	void operator/=(const float rhs)
	{	
		if(rhs==0.0f)	return;
		else
		{	
			v[0]/=rhs; 
			v[1]/=rhs; 
			v[2]/=rhs; 
			v[3]/=rhs;	
		}
	}

	//unary operators
	Vect4d operator-(void) const {return Vect4d(-v[0],-v[1],-v[2],-v[3]);}
	Vect4d operator+(void) const {return (*this);}

	//cast to pointer to float for glVertex4fv etc
	operator float* () const {return (float*) this;}
	operator const float* () const {return (const float*) this;}

	operator Vect3d();							//convert v4d to v3d

	//member variables
	float v[4];
};

#endif	//Vect4d.h
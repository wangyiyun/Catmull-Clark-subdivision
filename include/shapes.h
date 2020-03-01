#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include "math/vect3d.h"


//Base class for shapes
class ShapesC
{
public:
	virtual void SetPos(Vect3d tmp);
	virtual void SetColor(GLubyte r,GLubyte b,GLubyte g);
	virtual void Render();
	virtual void Move(float x, float y, float z);

protected:
  Vect3d  pos;
  GLubyte color[3];
};

//derived class from ShapesC
class TeapotC: public ShapesC
{
public:
	virtual void Render();		
};

//derived class from ShapesC
class SphereC: public ShapesC
{
public:
	virtual void Render();		
};

//derived class from ShapesC
class DonutC: public ShapesC
{
public:
	virtual void Render();		
};

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>			//Standard template library class
#include "math/vect3d.h"    //for vector manipulation
using namespace std;
#include <GL/freeglut.h>

class Surface;

class Surface
{
public:
	Surface(int n0, int n1, int n2, int n3);
	Surface();
	bool operator< (const Surface another) const;
	int p0;
	int p1;
	int p2;
	int p3;
};

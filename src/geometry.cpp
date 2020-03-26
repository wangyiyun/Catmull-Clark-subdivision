#include "..\include\geometry.h"

Surface::Surface(int n0, int n1, int n2, int n3)
{
	this->p0 = n0;
	this->p1 = n1;
	this->p2 = n2;
	this->p3 = n3;
}

Surface::Surface()
{
}

bool Surface::operator<(const Surface another) const
{
	if (p0 != another.p0)
	{
		return p0 < another.p0;
	}
	if (p1 != another.p1)
	{
		return p1 < another.p1;
	}
	if (p2 != another.p2)
	{
		return p2 < another.p2;
	}
	return p3 < another.p3;
}

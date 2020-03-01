#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>			//Standard template library class
#include "math/vect3d.h"    //for vector manipulation
using namespace std;
#include <GL/freeglut.h>

class TreeNode
{
public:
	// root only
	TreeNode(Vect3d p, float r) {
		position = p;
		parent = p;
		weight = r;
		linkFace = 0;
		angleX = 0;
		angleY = 0;
		angleZ = 0;
		length = 0;
		leftNode = NULL;
		rightNode = NULL;
	}
	TreeNode(Vect3d parentPos, float x, float y, float z, float l, float r, float link)
	{
		Vect3d offset = Vect3d(0,1,0);
		angleX = x;
		angleY = y;
		angleZ = z;
		length = l;
		parent = parentPos;
		linkFace = link;
		offset = offset.GetRotatedX(angleX);
		offset = offset.GetRotatedY(angleY);
		offset = offset.GetRotatedZ(angleZ);
		offset.Normalize();
		position = parent + offset * length;

		weight = r;

		leftNode = NULL;
		rightNode = NULL;
	}

	Vect3d position;
	float weight;	// radious of the branch

	Vect3d parent;
	int linkFace;	//0: top, 1: left, 2: right
	float angleX;
	float angleY;
	float angleZ;
	float length;

	TreeNode* leftNode;
	TreeNode* rightNode;

	void DrawBranch();
	void GenTree();
	void ModifyAngleTree(float delta);
	void ModifyLengthTree(float delta);
	void Update();
	vector<TreeNode*> GetTreeNodes();
};
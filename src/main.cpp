/************************************/
/* CS 590CGS Lab framework			*/
/* (C) Yiyun Wang 2020				*/
/* bbenes ~ at ~ purdue.edu			*/
/* Press +,- to subdivide			*/
/*       , . to scale the branch	*/
/*       <,> to rotate the branch	*/
/*       w to save the obj file		*/
/************************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <algorithm>
#include <GL/freeglut.h>
#include "objGen.h"


//in house created libraries
#include "math/vect3d.h"    //for vector manipulation
#include "math/triangle.h"  //triangles
#include "trackball.h"
#include "geometry.h"	// the surface of the tree
#include "treeNode.h"	// the defination of the tree

#pragma warning(disable : 4996)
#pragma comment(lib, "freeglut.lib")

using namespace std;

//some trackball variables -> used in mouse feedback
TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;

bool needRedisplay = false;
GLfloat  sign = +1; //diretcion of rotation
const GLfloat defaultIncrement = 0.7f; //speed of rotation
GLfloat  angleIncrement = defaultIncrement;

//window size
GLint wWindow = 1200;
GLint hWindow = 800;

//this defines what will be rendered
//see Key() how is it controlled
bool pointsFlag = true;
bool curveFlag = true;
float modifyLength = 0.0f;
float modifyAngle = 0.0f;
int subLevel = 0;

const float PI = 3.1415926;

// funcs
void DrawSurface(Vect3d p0, Vect3d p1, Vect3d p2, Vect3d p3, Vect3d color);
void DrawLine(Vect3d a, Vect3d b, Vect3d color);
void DrawPoint(Vect3d a, Vect3d color);

//color
Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0), pink(1.0f, 0.5f, 1.0f);

//returns random number from <-1,1>
inline float random11() {
	return 2.f * rand() / (float)RAND_MAX - 1.f;
}

TreeNode* treeRoot;
vector<Vect3d> points;
vector<Surface> surfaces;

vector<Vect3d> sub_points;
vector<Surface> sub_surfaces;

void saveMesh()
{
	vector <TriangleC> tri;   //all the triangles will be stored here
	tri.clear();
	for (int i = 0; i < surfaces.size(); i++)
	{
		TriangleC tmp;
		tmp.Set(points[surfaces[i].p0], points[surfaces[i].p1], points[surfaces[i].p2]);
		tri.push_back(tmp);
		tmp.Set(points[surfaces[i].p0], points[surfaces[i].p2], points[surfaces[i].p3]);
		tri.push_back(tmp);
	}
	SaveOBJ(&tri, "MyTreeMesh.obj");

}

void initTree()
{
	treeRoot = new TreeNode(Vect3d(0, 0, 0), 1);
	treeRoot->GenTree();
}

void drawTree()
{
	treeRoot->DrawBranch();
}

void GenBox(TreeNode* node)
{
	// gen 8 new box points for a node and push them into points vector.
	vector <Vect3d> box;
	box.clear();
	box.push_back(Vect3d(1, -0.5, 0));
	box.push_back(Vect3d(0, -0.5, -1));
	box.push_back(Vect3d(-1, -0.5, 0));
	box.push_back(Vect3d(0, -0.5, 1));
	box.push_back(Vect3d(1, 0.5, 0));
	box.push_back(Vect3d(0, 0.5, -1));
	box.push_back(Vect3d(-1, 0.5, 0));
	box.push_back(Vect3d(0, 0.5, 1));

	float weight = node->weight;
	weight *= 0.2f;

	for (int j = 0; j < 8; j++)
	{
		box[j] = box[j].GetRotatedX(node->angleX);
		box[j] = box[j].GetRotatedY(node->angleY);
		box[j] = box[j].GetRotatedZ(node->angleZ);

		Vect3d nP = node->position + box[j] * weight;
		points.push_back(nP);
	}
}

void visitTreeNode(TreeNode* tn, int parentStart)
{
	//for this node
	GenBox(tn);	// points +8
	int start = points.size() - 8;

	// add surface
	if (tn->position == tn->parent)
	{
		// is root
		Surface bottom = Surface(0, 3, 2, 1);	// bottom
		surfaces.push_back(bottom);
	}
	else
	{
		if (tn->linkFace == 0)
		{
			// link to parent's top
			Surface linkFront = Surface(start + 0, start + 3, parentStart + 7, parentStart + 4);
			surfaces.push_back(linkFront);
			Surface linkBack = Surface(start + 2, start + 1, parentStart + 5, parentStart + 6);
			surfaces.push_back(linkBack);
			Surface linkLeft = Surface(start + 3, start + 2, parentStart + 6, parentStart + 7);
			surfaces.push_back(linkLeft);
			Surface linkRight = Surface(start + 1, start + 0, parentStart + 4, parentStart + 5);
			surfaces.push_back(linkRight);

		}
		else if (tn->linkFace == 1)
		{
			// link to parent's left side
			Surface linkFront = Surface(start + 0, start + 3, parentStart + 3, parentStart + 7);
			surfaces.push_back(linkFront);
			Surface linkBack = Surface(start + 2, start + 1, parentStart + 6, parentStart + 2);
			surfaces.push_back(linkBack);
			Surface linkLeft = Surface(start + 3, start + 2, parentStart + 2, parentStart + 3);
			surfaces.push_back(linkLeft);
			Surface linkRight = Surface(start + 1, start + 0, parentStart + 7, parentStart + 6);
			surfaces.push_back(linkRight);
		}
		else
		{
			// link to parent's right side
			Surface linkFront = Surface(start + 0, start + 3, parentStart + 4, parentStart + 0);
			surfaces.push_back(linkFront);
			Surface linkBack = Surface(start + 2, start + 1, parentStart + 1, parentStart + 5);
			surfaces.push_back(linkBack);
			Surface linkLeft = Surface(start + 3, start + 2, parentStart + 5, parentStart + 4);
			surfaces.push_back(linkLeft);
			Surface linkRight = Surface(start + 1, start + 0, parentStart + 0, parentStart + 1);
			surfaces.push_back(linkRight);
		}
	}

	// front face
	Surface frontFace = Surface(start + 0, start + 4, start + 7, start + 3);
	surfaces.push_back(frontFace);

	//back face
	Surface backFace = Surface(start + 1, start + 2, start + 6, start + 5);
	surfaces.push_back(backFace);

	if (tn->leftNode != NULL)
	{
		if (tn->leftNode->linkFace == 1)
		{
			// child links to my left side -> close my right surface
			Surface closeRight = Surface(start + 0, start + 1, start + 5, start + 4);
			surfaces.push_back(closeRight);
		}
		else if (tn->leftNode->linkFace == 2)
		{
			// child links to my right side -> close my left surface
			Surface closeLeft = Surface(start + 2, start + 3, start + 7, start + 6);
			surfaces.push_back(closeLeft);
		}
		else
		{
			// child links to my top -> close my left and right surface
			Surface closeLeft = Surface(start + 2, start + 3, start + 7, start + 6);
			surfaces.push_back(closeLeft);
			Surface closeRight = Surface(start + 0, start + 1, start + 5, start + 4);
			surfaces.push_back(closeRight);
		}
		visitTreeNode(tn->leftNode, start);
	}

	if (tn->rightNode != NULL)
	{
		//if (tn->rightNode->linkFace == 1)
		//{
		//	// child links to my left side -> close my right surface
		//	Surface closeRight = Surface(start + 0, start + 1, start + 5, start + 4);
		//	surfaces.push_back(closeRight);
		//}
		//else if (tn->rightNode->linkFace == 2)
		//{
		//	// child links to my right side -> close my left surface
		//	Surface closeLeft = Surface(start + 2, start + 3, start + 7, start + 6);
		//	surfaces.push_back(closeLeft);
		//}
		//else
		//{
		//	// child links to my top -> close my left and right surface
		//	Surface closeLeft = Surface(start + 2, start + 3, start + 7, start + 6);
		//	surfaces.push_back(closeLeft);
		//	Surface closeRight = Surface(start + 0, start + 1, start + 5, start + 4);
		//	surfaces.push_back(closeRight);
		//}
		visitTreeNode(tn->rightNode, start);
	}

	if (tn->leftNode == NULL && tn->rightNode == NULL)
	{
		// is leaf
		// top face
		Surface top = Surface(start + 4, start + 5, start + 6, start + 7);
		surfaces.push_back(top);
		Surface leafLeft = Surface(start + 3, start + 7, start + 6, start + 2);
		surfaces.push_back(leafLeft);
		Surface leafRight = Surface(start + 5, start + 4, start + 0, start + 1);
		surfaces.push_back(leafRight);
		return;
	}
}

Vect3d calTouchSurfMidForLine(int a, int b)
{
	vector <int> touchSurf;
	touchSurf.clear();
	for (int i = 0; i < surfaces.size(); i++)
	{
		if (a == surfaces[i].p0 || a == surfaces[i].p1 || a == surfaces[i].p2 || a == surfaces[i].p3)
		{
			if (b == surfaces[i].p0 || b == surfaces[i].p1 || b == surfaces[i].p2 || b == surfaces[i].p3)
			{
				touchSurf.push_back(i);
			}
		}
	}
	// get avg touch surf mid
	return (sub_points[touchSurf[0]] + sub_points[touchSurf[1]]) / 2;
}

struct EdgeMid
{
	int p0;
	int p1;
	int sub_points_num;
};

vector<EdgeMid> edge_mid_points;
struct TouchSurf
{
	int surf;
	int e0;
	int e1;
};
vector <TouchSurf> TouchSurfsForPoint;
vector <int> TouchEdgesForPoint;

int searchEdgeMid(int a, int b)
{
	for (int i = 0; i < edge_mid_points.size(); i++)
	{
		if ((a == edge_mid_points[i].p0 && b == edge_mid_points[i].p1) || (b == edge_mid_points[i].p0 && a == edge_mid_points[i].p1))
		{
			return edge_mid_points[i].sub_points_num;
		}
	}
	return -1;
}

int calEdgeMid(int a, int b)
{
	// a, b are numbers in vector points
	Vect3d edgeMid;
	int searchResult = searchEdgeMid(a, b);
	if (searchResult == -1)
	{
		// can't find this edge's mid point info
		// push it into sub_points and edge_mid_points
		EdgeMid tmp;
		tmp.p0 = a;
		tmp.p1 = b;
		// cal edge mid
		// find avg face mid for this edge
		Vect3d faceMid = calTouchSurfMidForLine(a, b);
		edgeMid = (faceMid + (points[a] + points[b]) / 2) / 2;
		// update edge mid table
		tmp.sub_points_num = sub_points.size();
		edge_mid_points.push_back(tmp);
		// put the new edge mid in to sub_points
		sub_points.push_back(edgeMid);
		return tmp.sub_points_num;
	}
	else
	{
		// have exist midpoint for this edge
		return searchResult;
	}
}

void calNewOrinPoint(int p)
{
	TouchSurfsForPoint.clear();
	TouchEdgesForPoint.clear();
	// find all touching surfaces and edges
	for (int i = 0; i < surfaces.size(); i++)
	{
		if (p == surfaces[i].p0 || p == surfaces[i].p1 || p == surfaces[i].p2 || p == surfaces[i].p3)
		{
			//find a touching surface!
			TouchSurf tmp;
			tmp.surf = i;
			// inside a touching surface, there will be 2 touching edges
			// save those two edges number with face
			if (p == surfaces[i].p0)
			{
				// touching edge: p3-p0, p0-p1
				tmp.e0 = surfaces[i].p3;
				tmp.e1 = surfaces[i].p1;
				TouchEdgesForPoint.push_back(surfaces[i].p3);
				TouchEdgesForPoint.push_back(surfaces[i].p1);
			}
			else if (p == surfaces[i].p1)
			{
				tmp.e0 = surfaces[i].p0;
				tmp.e1 = surfaces[i].p2;
				TouchEdgesForPoint.push_back(surfaces[i].p0);
				TouchEdgesForPoint.push_back(surfaces[i].p2);
			}
			else if (p == surfaces[i].p2)
			{
				tmp.e1 = surfaces[i].p1;
				tmp.e0 = surfaces[i].p3;
				TouchEdgesForPoint.push_back(surfaces[i].p1);
				TouchEdgesForPoint.push_back(surfaces[i].p3);
			}
			else
			{
				tmp.e0 = surfaces[i].p2;
				tmp.e1 = surfaces[i].p0;
				TouchEdgesForPoint.push_back(surfaces[i].p2);
				TouchEdgesForPoint.push_back(surfaces[i].p0);
			}
			TouchSurfsForPoint.push_back(tmp);
		}
	}
	// clear repeat edge
	sort(TouchEdgesForPoint.begin(), TouchEdgesForPoint.end());
	TouchEdgesForPoint.erase(unique(TouchEdgesForPoint.begin(), TouchEdgesForPoint.end()), TouchEdgesForPoint.end());

	if (TouchSurfsForPoint.size() != TouchEdgesForPoint.size())
	{
		std::cout << "ERROR: at point " << p << endl;
	}

	int n = TouchSurfsForPoint.size();

	// calculate mid of touching surfaces and edges
	Vect3d touchFaceMid = Vect3d(0, 0, 0);
	Vect3d touchEdgeMid = Vect3d(0, 0, 0);
	for (int i = 0; i < n; i++)
	{
		touchFaceMid += sub_points[TouchSurfsForPoint[i].surf];
	}
	touchFaceMid /= n;
	
	for (int i = 0; i < n; i++)
	{
		touchEdgeMid += points[TouchEdgesForPoint[i]] + points[p];
	}
	touchEdgeMid /= n * 2;

	//cout << p << ": " << TouchSurfsForPoint.size() << " " << TouchEdgesForPoint.size() << endl;

	Vect3d newP = (touchFaceMid + 2 * touchEdgeMid + (n - 3) * points[p]) / n;
	int newP_in_sub_points = sub_points.size();
	sub_points.push_back(newP);
	// for each touching face, search/calculate 2 touching edges' mid point and add an new sub surface
	// need points:
	// newP: newP_in_sub_points
	// surf mid: TouchSurfsForPoint[i].surf
	// edge0 mid: calEdgeMid(p, TouchSurfsForPoint[i].e0)
	// edge1 mid: calEdgeMid(p, TouchSurfsForPoint[i].e1)
	for (int i = 0; i < n; i++)
	{
		int edge0Mid = calEdgeMid(p, TouchSurfsForPoint[i].e0);
		int edge1Mid = calEdgeMid(p, TouchSurfsForPoint[i].e1);
		Surface newSurf = Surface(newP_in_sub_points, edge0Mid, TouchSurfsForPoint[i].surf, edge1Mid);
		sub_surfaces.push_back(newSurf);
	}
}

void subdivision()
{
	sub_points.clear();
	sub_surfaces.clear();
	edge_mid_points.clear();
	// calculate all surfaces' midpoints and push them to sub points vector
	for (int i = 0; i < surfaces.size(); i++)
	{
		Vect3d surfaceMid = (points[surfaces[i].p0] + points[surfaces[i].p1] + points[surfaces[i].p2] + points[surfaces[i].p3]) / 4;
		sub_points.push_back(surfaceMid);
	}
	//std::cout << "surf mid" << std::endl;
	
	for (int i = 0; i < points.size(); i++)
	{
		calNewOrinPoint(i);
	}

	//std::cout << "new orin point" << std::endl;
	points.swap(sub_points);
	surfaces.swap(sub_surfaces);
}

void initGeometry()
{
	points.clear();
	surfaces.clear();
	visitTreeNode(treeRoot, 0);
	//subdivision();
	for (int i = 0; i < subLevel; i++)
	{
		subdivision();
	}
}
int surfNum = 0;
void drawGeometry()
{
	//for (int i = 0; i < points.size(); i += 4)
	//{
	//	DrawPoint(points[i], red);
	//	DrawPoint(points[i + 1], yellow);
	//	DrawPoint(points[i + 2], blue);
	//	DrawPoint(points[i + 3], pink);
	//}
	for (int i = 0; i < points.size(); i++)
	{
		DrawPoint(points[i], pink);
	}
	////debug points
	//DrawPoint(points[debugPoint], red);
	//for (int i = 0; i < debugPoints.size(); i++)
	//{
	//	DrawPoint(points[debugPoints[i]], pink);
	//}
	//for (int i = 0; i < debugPoints.size(); i++)
	//{
	//	DrawPoint(debugPoints[i], pink);
	//}
	//for (int i = 0; i < debugSurfaces.size(); i++)
	//{
	//	DrawSurface(points[surfaces[debugSurfaces[i]].p0], points[surfaces[debugSurfaces[i]].p1], points[surfaces[debugSurfaces[i]].p2], points[surfaces[debugSurfaces[i]].p3], yellow);
	//}
	for (int i = 0; i < surfaces.size(); i++)
	{
		DrawSurface(points[surfaces[i].p0], points[surfaces[i].p1], points[surfaces[i].p2], points[surfaces[i].p3], green);
		//cout << surfaces[i].p0 << " " << surfaces[i].p1 << " " << surfaces[i].p2 << " " << surfaces[i].p3 << endl;
	}
	//for (int i = 0; i < sub_surfaces.size(); i++)
	//{
	//	DrawSurface(sub_points[sub_surfaces[i].p0], sub_points[sub_surfaces[i].p1], sub_points[sub_surfaces[i].p2], sub_points[sub_surfaces[i].p3], red);
	//}

}



/*********************************
Some OpenGL-related functions DO NOT TOUCH
**********************************/
//displays the text message in the GL window
void GLMessage(char* message)
{
	static int i;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0, 0, 255);
	glRasterPos2i(10, 10);
	for (i = 0; i < (int)strlen(message); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//called when a window is reshaped
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	//remember the settings for the camera
	wWindow = w;
	hWindow = h;
}

//Some simple rendering routines using old fixed-pipeline OpenGL
void DrawSurface(Vect3d p0, Vect3d p1, Vect3d p2, Vect3d p3, Vect3d color)
{
	glColor3fv(color);
	glBegin(GL_TRIANGLES);
	glVertex3fv(p0);
	glVertex3fv(p1);
	glVertex3fv(p2);

	glVertex3fv(p0);
	glVertex3fv(p2);
	glVertex3fv(p3);
	glEnd();
}

//draws line from a to b with color 
void DrawLine(Vect3d a, Vect3d b, Vect3d color) {

	glColor3fv(color);
	glBegin(GL_LINES);
	glVertex3fv(a);
	glVertex3fv(b);
	glEnd();
}

//draws point at a with color 
void DrawPoint(Vect3d a, Vect3d color) {

	glColor3fv(color);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3fv(a);
	glEnd();
}

/**********************
LAB related MODIFY
***********************/

//display coordinate system
void CoordSyst() {
	Vect3d a, b, c;
	Vect3d origin(0, 0, 0);
	Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);

	//draw the coordinate system 
	a.Set(1, 0, 0);
	b.Set(0, 1, 0);
	c.Set(Vect3d::Cross(a, b)); //use cross product to find the last vector
	glLineWidth(4);
	DrawLine(origin, a, red);
	DrawLine(origin, b, green);
	DrawLine(origin, c, blue);
	glLineWidth(1);

}

//this is the actual code for the lab
void Lab03() {
	Vect3d a, b, c;
	Vect3d origin(0, 0, 0);

	//CoordSyst();
	//drawTree();
	drawGeometry();
}

//the main rendering function
void RenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//set camera
	glMatrixMode(GL_MODELVIEW);
	trackball.Set3DViewCamera();
	//call the student's code from here
	Lab03();
}

//Add here if you want to control some global behavior
//see the pointFlag and how is it used
void Kbd(unsigned char a, int x, int y)//keyboard callback
{
	switch (a)
	{
	case 27: exit(0); break;
	case 'p': pointsFlag = !pointsFlag; break;
	case 'c': curveFlag = !curveFlag; break;
	case 32: {
		if (angleIncrement == 0) angleIncrement = defaultIncrement;
		else angleIncrement = 0;
		break;
	}
	case 'w':
		saveMesh();
		break;
	case '-': {
		subLevel--;
		if (subLevel < 0) subLevel = 0;
		initGeometry();
		std::cout << "INFO: Subdivision level " << subLevel << endl;
		break;
	}
	case '+': {
		if (subLevel < 4)subLevel++;
		subdivision();
		std::cout << "INFO: Subdivision level " << subLevel << endl;
		break;
	}
	case ',': {
		modifyLength -= 0.01f;
		if (modifyLength > -0.1) treeRoot->ModifyLengthTree(-0.01f);
		initGeometry();
		//cout << "len" << modifyLength << endl;
		break;
	}
	case '.': {
		modifyLength += 0.01f;
		if (modifyLength < 0.2) treeRoot->ModifyLengthTree(0.01f);
		initGeometry();
		//cout << "len" << modifyLength << endl;
		break;
	}
	case '<': {
		modifyAngle -= 1.0f;
		if (modifyAngle > -20) treeRoot->ModifyAngleTree(-1.0f);
		initGeometry();
		//cout << "angle" << modifyAngle << endl;
		break;
	}
	case '>': {
		modifyAngle += 1.0f;
		if (modifyAngle < 20) treeRoot->ModifyAngleTree(1.0f);
		initGeometry();
		//cout << "angle" << modifyAngle << endl;
		break;
	}

	case 'r': {

		break;
	}
	}
	glutPostRedisplay();
}


/*******************
OpenGL code. Do not touch.
******************/
void Idle(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1); //background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLMessage("Lab 3 - CS 590CGS");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (GLfloat)wWindow / (GLfloat)hWindow, 0.01, 100); //set the camera
	glMatrixMode(GL_MODELVIEW); //set the scene
	glLoadIdentity();
	gluLookAt(0, 10, 10, 0, 0, 0, 0, 1, 0); //set where the camera is looking at and from. 
	static GLfloat angle = 0;
	angle += angleIncrement;
	if (angle >= 360.f) angle = 0.f;
	glRotatef(sign * angle, 0, 1, 0);
	RenderObjects();
	glutSwapBuffers();
}

void Display(void)
{

}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseLeft = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		trackball.Set(false, x, y);
		mouseLeft = false;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseMid = true;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
	{
		trackball.Set(true, x, y);
		mouseMid = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseRight = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		trackball.Set(true, x, y);
		mouseRight = false;
	}
}

void MouseMotion(int x, int y) {
	if (mouseLeft)  trackball.Rotate(x, y);
	if (mouseMid)   trackball.Translate(x, y);
	if (mouseRight) trackball.Zoom(x, y);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Surface of Revolution");
	//GLenum err = glewInit();
	// if (GLEW_OK != err){
	// fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	//}
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd); //+ and -
	glutSpecialUpFunc(NULL);
	glutSpecialFunc(NULL);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	initTree();
	initGeometry();
	glutMainLoop();
	return 0;
}

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
#include <map>
#include <set>
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
map<int, Vect3d> points;
map<Vect3d, int> points_rev;
map<int, Surface> surfaces;
map<Surface, int> surfaces_rev;

map<int, Vect3d> sub_points;
map<Vect3d, int> sub_points_rev;
map<int, Surface> sub_surfaces;
map<Surface, int> sub_surfaces_rev;
map<pair<int, int>, vector<int>> edge_map;	// edge - surface mapper
map<int, set<int>> node_map;				// node - node mapper
map<int, int> surface_mid;
vector<Vect3d> points_ptr;
vector<Surface> surface_ptr;

template<typename  T> int newObject(map<int, T> &origin, map<T, int> &rev, T newObj)
{
	if (rev.find(newObj) != rev.end())
	{
		return rev[newObj];
	}
	int index = origin.size();
	origin[index] = newObj;
	rev[newObj] = index;
	return index;
}

void newEdge(int v0, int v1, int f)
{
	pair<int, int> index = make_pair(min(v0, v1), max(v0, v1));
	edge_map[index].push_back(f);
	node_map[v0].insert(v1);
	node_map[v1].insert(v0);
	return;
}

vector<int> findEdge(int v0, int v1)
{
	pair<int, int> index = make_pair(min(v0, v1), max(v0, v1));
	return edge_map[index];
}

void saveMesh()
{
	vector <TriangleC> tri;   //all the triangles will be stored here
	tri.clear();
	for (auto surface_index : surfaces)
	{
		Surface surface = surface_index.second;
		TriangleC tmp;
		tmp.Set(points[surface.p0], points[surface.p1], points[surface.p2]);
		tri.push_back(tmp);
		tmp.Set(points[surface.p0], points[surface.p2], points[surface.p3]);
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
		newObject(points, points_rev, nP);
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
		newObject(surfaces, surfaces_rev, bottom);
	}
	else
	{
		if (tn->linkFace == 0)
		{
			// link to parent's top
			Surface linkFront = Surface(start + 0, start + 3, parentStart + 7, parentStart + 4);
			Surface linkBack = Surface(start + 2, start + 1, parentStart + 5, parentStart + 6);
			Surface linkLeft = Surface(start + 3, start + 2, parentStart + 6, parentStart + 7);
			Surface linkRight = Surface(start + 1, start + 0, parentStart + 4, parentStart + 5);
			newObject(surfaces, surfaces_rev, linkFront);
			newObject(surfaces, surfaces_rev, linkBack);
			newObject(surfaces, surfaces_rev, linkLeft);
			newObject(surfaces, surfaces_rev, linkRight);

		}
		else if (tn->linkFace == 1)
		{
			// link to parent's left side
			Surface linkFront = Surface(start + 0, start + 3, parentStart + 3, parentStart + 7);
			Surface linkBack = Surface(start + 2, start + 1, parentStart + 6, parentStart + 2);
			Surface linkLeft = Surface(start + 3, start + 2, parentStart + 2, parentStart + 3);
			Surface linkRight = Surface(start + 1, start + 0, parentStart + 7, parentStart + 6);
			newObject(surfaces, surfaces_rev, linkFront);
			newObject(surfaces, surfaces_rev, linkBack);
			newObject(surfaces, surfaces_rev, linkLeft);
			newObject(surfaces, surfaces_rev, linkRight);
		}
		else
		{
			// link to parent's right side
			Surface linkFront = Surface(start + 0, start + 3, parentStart + 4, parentStart + 0);
			Surface linkBack = Surface(start + 2, start + 1, parentStart + 1, parentStart + 5);
			Surface linkLeft = Surface(start + 3, start + 2, parentStart + 5, parentStart + 4);
			Surface linkRight = Surface(start + 1, start + 0, parentStart + 0, parentStart + 1);
			newObject(surfaces, surfaces_rev, linkFront);
			newObject(surfaces, surfaces_rev, linkBack);
			newObject(surfaces, surfaces_rev, linkLeft);
			newObject(surfaces, surfaces_rev, linkRight);
		}
	}

	// front face
	Surface frontFace = Surface(start + 0, start + 4, start + 7, start + 3);
	newObject(surfaces, surfaces_rev, frontFace);

	//back face
	Surface backFace = Surface(start + 1, start + 2, start + 6, start + 5);
	newObject(surfaces, surfaces_rev, backFace);

	if (tn->leftNode != NULL)
	{
		if (tn->leftNode->linkFace == 1)
		{
			// child links to my left side -> close my right surface
			Surface closeRight = Surface(start + 0, start + 1, start + 5, start + 4);
			newObject(surfaces, surfaces_rev, closeRight);
		}
		else if (tn->leftNode->linkFace == 2)
		{
			// child links to my right side -> close my left surface
			Surface closeLeft = Surface(start + 2, start + 3, start + 7, start + 6);
			newObject(surfaces, surfaces_rev, closeLeft);
		}
		else
		{
			// child links to my top -> close my left and right surface
			Surface closeLeft = Surface(start + 2, start + 3, start + 7, start + 6);
			Surface closeRight = Surface(start + 0, start + 1, start + 5, start + 4);
			newObject(surfaces, surfaces_rev, closeLeft);
			newObject(surfaces, surfaces_rev, closeRight);
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
		Surface leafLeft = Surface(start + 3, start + 7, start + 6, start + 2);
		Surface leafRight = Surface(start + 5, start + 4, start + 0, start + 1);
		newObject(surfaces, surfaces_rev, top);
		newObject(surfaces, surfaces_rev, leafLeft);
		newObject(surfaces, surfaces_rev, leafRight);
		return;
	}
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

int cutEdge(int v0, int v1)
{
	vector<int> surfaces = findEdge(v0, v1);
	if (surfaces.size() != 2)
	{
		cout << "Error: There are no two faces adjacent to this edge" << endl;
	}
	Vect3d v2 = sub_points[surface_mid[surfaces[0]]];
	Vect3d v3 = sub_points[surface_mid[surfaces[1]]];
	return newObject(sub_points, sub_points_rev, (points[v0] + points[v1] + v2 + v3) / 4);
}

Vect3d newNode(int node_id)
{
	set<int> surface_ids;
	Vect3d Q = Vect3d(0, 0, 0);
	Vect3d R = Vect3d(0, 0, 0);

	for (int conn_id : node_map[node_id])
	{
		for (int surf_id : findEdge(conn_id, node_id))
		{
			surface_ids.insert(surf_id);
		}
		R += points[conn_id];
	}

	for (int surf_id : surface_ids)
	{
		Q += sub_points[surface_mid[surf_id]];
	}

	int n = node_map[node_id].size();
	R /= n;
	Q /= surface_ids.size();
	
	return (Q / n) + (R * 2 / n) + (points[node_id] * (n-3) / n);
}

void init_show()
{
	points_ptr.clear();
	surface_ptr.clear();
	for (auto point : points)
	{
		points_ptr.push_back(point.second);
	}
	for (auto surface : surfaces)
	{
		surface_ptr.push_back(surface.second);
	}
}

void subdivision()
{
	node_map.clear();
	edge_map.clear();
	surface_mid.clear();
	sub_points.clear();
	sub_points_rev.clear();
	sub_surfaces.clear();
	sub_surfaces_rev.clear();
	edge_mid_points.clear();

	// calculate all surfaces' midpoints and push them to sub points vector
	for (auto surface: surfaces)
	{
		int surf_i = surface.first;
		Surface surf_d = surface.second;

		Vect3d surface_mid_p = (points[surf_d.p0] + points[surf_d.p1] + points[surf_d.p2] + points[surf_d.p3]) / 4;
		surface_mid[surf_i] = newObject(sub_points, sub_points_rev, surface_mid_p);

		newEdge(surf_d.p0, surf_d.p1, surf_i);
		newEdge(surf_d.p1, surf_d.p2, surf_i);
		newEdge(surf_d.p2, surf_d.p3, surf_i);
		newEdge(surf_d.p3, surf_d.p0, surf_i);
	}
	//std::cout << "surf mid" << std::endl;

	for (auto surface : surfaces)
	{
		int surf_i = surface.first;
		Surface surf_d = surface.second;


		vector<int> newPoints;
		newPoints.push_back(newObject(sub_points, sub_points_rev, newNode(surf_d.p0)));
		newPoints.push_back(cutEdge(surf_d.p0, surf_d.p1));
		newPoints.push_back(newObject(sub_points, sub_points_rev, newNode(surf_d.p1)));
		newPoints.push_back(cutEdge(surf_d.p1, surf_d.p2));
		newPoints.push_back(newObject(sub_points, sub_points_rev, newNode(surf_d.p2)));
		newPoints.push_back(cutEdge(surf_d.p2, surf_d.p3));
		newPoints.push_back(newObject(sub_points, sub_points_rev, newNode(surf_d.p3)));
		newPoints.push_back(cutEdge(surf_d.p3, surf_d.p0));

		for (int i = 1; i < 8; i += 2)
		{
			newObject(sub_surfaces, sub_surfaces_rev, Surface(
				newPoints[i%8],
				newPoints[(i+1)%8],
				newPoints[(i+2)%8],
				surface_mid[surf_i])
			);
		}
	}

	//std::cout << "new orin point" << std::endl;
	points.swap(sub_points);
	points_rev.swap(sub_points_rev);
	surfaces.swap(sub_surfaces);
	surfaces_rev.swap(sub_surfaces_rev);

	init_show();
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
	init_show();
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
	for (auto point: points_ptr)
	{
		DrawPoint(point, pink);
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
	for (auto surface: surface_ptr)
	{
		DrawSurface(points_ptr[surface.p0], points_ptr[surface.p1], points_ptr[surface.p2], points_ptr[surface.p3], green);
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
void Kbd(unsigned char a, int x, int y) //keyboard callback
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

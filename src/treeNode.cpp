#include "..\include\treeNode.h"

vector<TreeNode*> nodes;

void TreeNode::DrawBranch()
{
	Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);
	for (int i = 0; i < 9; i++)
	{
		// draw current point
		glColor3fv(blue);
		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3fv(nodes[i]->position);
		glEnd();
		// draw segment to child
		if (nodes[i]->leftNode != NULL)
		{
			glColor3fv(yellow * nodes[i]->weight);
			glBegin(GL_LINES);
			glVertex3fv(nodes[i]->position);
			glVertex3fv(nodes[i]->leftNode->position);
			glEnd();
		}
		if (nodes[i]->rightNode != NULL)
		{
			glColor3fv(yellow * nodes[i]->weight);
			glBegin(GL_LINES);
			glVertex3fv(nodes[i]->position);
			glVertex3fv(nodes[i]->rightNode->position);
			glEnd();
		}
	}	
	return;
}

void TreeNode::GenTree()
{
	// by hardCode 9 points

	nodes.clear();
	// root
	nodes.push_back(this);
	//0: top, 1: left, 2: right
	TreeNode* p1 = new TreeNode(this->position, 0, 0, 0, 0.5, 0.9, 0);
	this->leftNode = p1;
	nodes.push_back(p1);

	TreeNode* p2 = new TreeNode(p1->position, 5, 20, 5, 0.7, 0.8, 0);
	p1->leftNode = p2;
	nodes.push_back(p2);

	TreeNode* p3 = new TreeNode(p2->position, 80, -30, 10, 0.4, 0.5, 1);
	p2->leftNode = p3;
	nodes.push_back(p3);

	TreeNode* p4 = new TreeNode(p3->position, 70, -10, 20, 0.3, 0.4, 0);
	p3->leftNode = p4;
	nodes.push_back(p4);

	TreeNode* p5 = new TreeNode(p2->position, -10, 20, -10, 0.5, 0.7, 0);
	p2->rightNode = p5;
	nodes.push_back(p5);

	TreeNode* p6 = new TreeNode(p5->position, -90, -20, 0, 0.3, 0.3, 2);
	p5->leftNode = p6;
	nodes.push_back(p6);

	TreeNode* p7 = new TreeNode(p5->position, -5, 30, 5, 0.3, 0.4, 0);
	p5->rightNode = p7;
	nodes.push_back(p7);

	TreeNode* p8 = new TreeNode(p7->position, 15, 40, 20, 0.2, 0.2, 0);
	p7->leftNode = p8;
	nodes.push_back(p8);
}

void TreeNode::ModifyAngleTree(float delta)
{
	for (int i = 0; i < 9; i++)
	{
		if (nodes[i]->leftNode != NULL)
		{
			nodes[i]->leftNode->angleX += delta * (i + 1) / (float)9;
			nodes[i]->leftNode->angleY += delta * (i + 1) / (float)9;
			nodes[i]->leftNode->angleZ += delta * (i + 1) / (float)9;
			nodes[i]->leftNode->Update();
		}
		if (nodes[i]->rightNode != NULL)
		{
			nodes[i]->rightNode->angleX += delta * (i + 1) / (float)9;
			nodes[i]->rightNode->angleY += delta * (i + 1) / (float)9;
			nodes[i]->rightNode->angleZ += delta * (i + 1) / (float)9;
			nodes[i]->rightNode->Update();

		}
	}
	return;
}

void TreeNode::ModifyLengthTree(float len)
{
	for (int i = 0; i < 9; i++)
	{
		if (nodes[i]->leftNode != NULL)
		{
			nodes[i]->leftNode->length += len;
			nodes[i]->leftNode->Update();
		}
		if (nodes[i]->rightNode != NULL)
		{
			nodes[i]->rightNode->angleX += len;
			nodes[i]->rightNode->Update();
		}
	}
	return;
}

void TreeNode::Update()
{
	Vect3d offset = Vect3d(0, 1, 0);
	offset = offset.GetRotatedX(angleX);
	offset = offset.GetRotatedY(angleY);
	offset = offset.GetRotatedZ(angleZ);
	offset.Normalize();
	position = parent + offset * length;
	if (leftNode != NULL)
	{
		leftNode->parent = position;
	}
	if (rightNode != NULL)
	{
		rightNode->parent = position;
	}
}

vector<TreeNode*> TreeNode::GetTreeNodes()
{
	return nodes;
}

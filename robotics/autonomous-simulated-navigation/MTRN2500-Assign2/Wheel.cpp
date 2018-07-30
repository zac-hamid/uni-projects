#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#elif defined(WIN32)
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include "Wheel.hpp"

Wheel::Wheel(bool isRolling, bool isSteering) : Cylinder(isRolling, isSteering)
{
	defaultRot = rotation;
}

Wheel::Wheel(double x_, double y_, double z_, double radius_, double depth_, 
	int slices_, bool isRolling, bool isSteering) : Cylinder(x_, y_, z_, radius_, depth_, slices_, isRolling, isSteering)
{
	defaultRot = rotation;
}

Wheel::Wheel(double x_, double y_, double z_, double radius_, double depth_, 
	int slices_, double rotation_, bool isRolling, bool isSteering) : Cylinder(x_, y_, z_, radius_, depth_, slices_, rotation_, isRolling, isSteering)
{
	defaultRot = rotation_;
}

Wheel::~Wheel()
{
}

void Wheel::draw()
{
	setColor(0, 0, 0);
	Cylinder::draw();

	glPushMatrix();
	double p1, p2 = 0;
	double theta = 0;

	glTranslated(x, y, z);
	glRotated(-rotation, 0, 1, 0);
	glRotated(roll, 0, 0, 1);

	glLineWidth(5);

	int numLines = 5;
	//Back face
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	for (int i = 0; i < numLines; i++) {
		theta = ((double)i / numLines) * 2 * M_PI;
		p1 = radius * sin(theta);
		p2 = radius * cos(theta);
		glVertex3d(0, 0, -depth / 2);
		glVertex3d(p1, p2, -depth / 2);
	}
	glEnd();

	//Front face
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	for (int i = 0; i < numLines; i ++) {
		theta = ((double)i / numLines) * 2 * M_PI;
		p1 = radius * sin(theta);
		p2 = radius * cos(theta);
		glVertex3d(0, 0, depth / 2);
		glVertex3d(p1, p2, depth / 2);
	}
	glEnd();

	glLineWidth(1);
	glPopMatrix();
}

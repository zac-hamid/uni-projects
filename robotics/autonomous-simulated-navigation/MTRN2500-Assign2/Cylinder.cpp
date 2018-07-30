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

#include <math.h>
#define _USE_MATH_DEFINES
#include "Shape.hpp"
#include "Cylinder.hpp"

#include <iostream>

Cylinder::Cylinder()
{
	this->isRolling = false;
	this->isSteering = false;
	radius = 1;
	depth = 1;
	slices = DEFAULT_CYLINDER_SLICES;
	roll = 0;
	rotation = 0;
}

Cylinder::Cylinder(bool isRolling, bool isSteering) : Shape()
{
	this->isRolling = isRolling;
	this->isSteering = isSteering;
	radius = 1;
	depth = 1;
	slices = DEFAULT_CYLINDER_SLICES;
	roll = 0;
	rotation = 0;
	defaultRot = 0;
}

Cylinder::Cylinder(double x_, double y_, double z_,
	double radius_, double depth_, int slices_, bool isRolling, bool isSteering) : Shape(x_,y_,z_)
{

	this->isRolling = isRolling;
	this->isSteering = isSteering;
	this->radius = radius_;
	this->depth = depth_;
	this->slices = slices_;
	defaultRot = 0;

	roll = 0;
	rotation = 0;
}

Cylinder::Cylinder(double x_, double y_, double z_,
	double radius_, double depth_, int slices_, 
	double rotation_, bool isRolling, bool isSteering) : Shape(x_,y_,z_,rotation_)
{

	this->isRolling = isRolling;
	this->isSteering = isSteering;
	this->radius = radius_;
	this->depth = depth_;
	this->slices = slices_;

	defaultRot = rotation_;
	roll = 0;
}

Cylinder::~Cylinder()
{
}

double Cylinder::getRadius()
{
	return radius;
}

double Cylinder::getDepth()
{
	return depth;
}

double Cylinder::getRoll()
{
	return roll;
}

bool Cylinder::getIsRolling() 
{
	return isRolling;
}

bool Cylinder::getIsSteering() 
{
	return isSteering;
}

int Cylinder::getSlices()
{
	return slices;
}

void Cylinder::setRadius(double radius)
{
	this->radius = radius;
}

void Cylinder::setDepth(double depth)
{
	this->depth = depth;
}

void Cylinder::setSlices(int slices)
{
	this->slices = slices;
}

void Cylinder::setRoll(double roll)
{
	this->roll = roll;
}

void Cylinder::rollCylinder(double rollAmount)
{
	roll += rollAmount;
}

void Cylinder::rotateCylinder(double rotation)
{
	this->rotation += rotation;
}

void Cylinder::draw()
{
	double p1, p2 = 0;
	double theta = 0;
	double j = radius;

	glPushMatrix();
	positionInGL();
	glRotated(roll, 0, 0, 1);
	setColorInGL();

	glBegin(GL_QUAD_STRIP);

	for (int i = 0; i <= slices; i++)
	{
		theta = i * (2 * M_PI / slices);
		p1 = radius * sin(theta);
		p2 = radius * cos(theta);
		glVertex3d(p1, p2, -depth / 2);
		glVertex3d(p1, p2, depth / 2);
	}
	glEnd();

	//Back face
	glBegin(GL_POLYGON);
	for (int i = 0; i < slices; i++) {
		theta = i * (2 * M_PI / slices);
		p1 = radius * sin(theta);
		p2 = radius * cos(theta);
		glVertex3d(p1, p2, -depth / 2);
	}
	glEnd();

	//Front face
	glBegin(GL_POLYGON);
	for (int i = 0; i < slices; i++) {
		theta = i * (2 * M_PI / slices);
		p1 = radius * sin(theta);
		p2 = radius * cos(theta);
		glVertex3d(p1, p2, depth / 2);
	}
	glEnd();
	glPopMatrix();
}

void Cylinder::steerCylinder(double amount)
{
	this->setRotation(defaultRot + amount);
}

/*
*/
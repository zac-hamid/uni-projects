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

#include "Shape.hpp"
#include "TrapezoidalPrism.hpp"

TrapezoidalPrism::TrapezoidalPrism() : Shape()
{
	base_length = 1;
	top_length = 1;
	left_offset = 0;
	height = 1;
	depth = 1;
}

TrapezoidalPrism::TrapezoidalPrism(double x_, double y_, double z_,
	double base_length_, double top_length_, double left_offset_, double height_, double depth_) : Shape(x_, y_, z_)
{
	base_length = base_length_;
	top_length = top_length_;
	left_offset = left_offset_;
	height = height_;
	depth = depth_;
}

TrapezoidalPrism::TrapezoidalPrism(double x_, double y_, double z_, 
	double base_length_, double top_length_, double left_offset_, 
	double height_, double depth_, double rotation_) : Shape(x_, y_, z_, rotation_)
{
	base_length = base_length_;
	top_length = top_length_;
	left_offset = left_offset_;
	height = height_;
	depth = depth_;
}

double TrapezoidalPrism::getBaseLength()
{
	return base_length;
}

double TrapezoidalPrism::getTopLength()
{
	return top_length;
}

double TrapezoidalPrism::getHeight()
{
	return height;
}

double TrapezoidalPrism::getDepth()
{
	return depth;
}

double TrapezoidalPrism::getLeftOffset()
{
	return left_offset;
}

void TrapezoidalPrism::setBaseLength(double baseLength)
{
	this->base_length = baseLength;
}

void TrapezoidalPrism::setTopLength(double topLength)
{
	this->top_length = topLength;
}

void TrapezoidalPrism::setHeight(double height)
{
	this->height = height;
}

void TrapezoidalPrism::setDepth(double depth)
{
	this->depth = depth;
}

void TrapezoidalPrism::setLeftOffset(double offset)
{
	this->left_offset = offset;
}

void TrapezoidalPrism::draw()
{
	glPushMatrix();
	positionInGL();
	setColorInGL();
	double i = base_length / 2;
	double k = depth / 2;
	double right_offset = fabs(base_length - (left_offset + top_length));
	glBegin(GL_QUADS);
	
		//Front face
		glVertex3d(-i+left_offset,height,-k);
		glVertex3d(-i,0,-k);
		glVertex3d(i,0,-k);
		glVertex3d(i-right_offset,height,-k);

		//Back face
		glVertex3d(-i,0,k);
		glVertex3d(-i+left_offset,height,k);
		glVertex3d(i-right_offset,height,k);
		glVertex3d(i,0,k);

		//Top Quad
		glVertex3d(-i+left_offset,height,k);
		glVertex3d(-i+left_offset,height,-k);
		glVertex3d(i-right_offset,height,-k);
		glVertex3d(i-right_offset,height,k);

		//Bottom Quad
		glVertex3d(i,0,k);
		glVertex3d(i,0,-k);
		glVertex3d(-i,0,-k);
		glVertex3d(-i,0,k);

		//Left Quad
		glVertex3d(-i+left_offset,height,-k);
		glVertex3d(-i+left_offset,height,k);
		glVertex3d(-i,0,k);
		glVertex3d(-i,0,-k);

		//Right Quad
		glVertex3d(i-right_offset,height,-k);
		glVertex3d(i-right_offset,height,k);
		glVertex3d(i,0,k);
		glVertex3d(i,0,-k);
	glEnd();
	glPopMatrix();
}

TrapezoidalPrism::~TrapezoidalPrism()
{
}
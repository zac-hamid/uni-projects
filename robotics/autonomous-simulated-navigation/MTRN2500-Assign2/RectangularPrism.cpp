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

#include "Shape.hpp"
#include "RectangularPrism.hpp"

RectangularPrism::RectangularPrism() : Shape()
{
	width = 1.0;
	height = 1.0;
	depth = 1.0;
}

RectangularPrism::RectangularPrism(double x_, double y_, double z_, double x_length, double y_length, double z_length) : Shape(x_, y_, z_)
{
	width = x_length;
	height = y_length;
	depth = z_length;
}

RectangularPrism::RectangularPrism(double x_, double y_, double z_, double x_length, double y_length, double z_length, double rotation_) : Shape(x_, y_, z_, rotation_)
{
	width = x_length;
	height = y_length;
	depth = z_length;
}

double RectangularPrism::getWidth()
{
	return width;
}

double RectangularPrism::getHeight()
{
	return height;
}

double RectangularPrism::getDepth()
{
	return depth;
}

void RectangularPrism::setWidth(double width)
{
	this->width = width;
}

void RectangularPrism::setHeight(double height)
{
	this->height = height;
}

void RectangularPrism::setDepth(double depth)
{
	this->depth = depth;
}

void RectangularPrism::draw()
{
	glPushMatrix();
	positionInGL();
	setColorInGL();
	double i = width / 2;
	double k = depth / 2;

	glBegin(GL_QUADS);
		//positive-depth face
		glVertex3d(-i, height, k);
		glVertex3d(i, height, k);
		glVertex3d(i, 0, k);
		glVertex3d(-i, 0, k);

		//negative-depth face
		glVertex3d(i, height, -k);
		glVertex3d(-i, height, -k);
		glVertex3d(-i, 0, -k);
		glVertex3d(i, 0, -k);

		//positive-width face
		glVertex3d(i, height, k);
		glVertex3d(i, height, -k);
		glVertex3d(i, 0, -k);
		glVertex3d(i, 0, k);

		//negative-width face
		glVertex3d(-i, height, k);
		glVertex3d(-i, height, -k);
		glVertex3d(-i, 0, -k);
		glVertex3d(-i, 0, k);

		//positive-height face
		glVertex3d(-i, height, -k);
		glVertex3d(i, height, -k);
		glVertex3d(i, height, k);
		glVertex3d(-i, height, k);

		//negative-height face
		glVertex3d(-i, 0, -k);
		glVertex3d(i, 0, -k);
		glVertex3d(i, 0, k);
		glVertex3d(-i, 0, k);
	glEnd();

	glPopMatrix();
}

RectangularPrism::~RectangularPrism()
{
}

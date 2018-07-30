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
#include "TriangularPrism.hpp"

#define PI 3.141592653589793238462643383

TriangularPrism::TriangularPrism() : Shape(){
	theta = 60.0;
	width = 1.0;
	side_length = 1.0;
	depth = 1.0;
	rotation = rotation + 180;
}

TriangularPrism::TriangularPrism(double x_, double y_, double z_,
	double width_, double side_length_, double angle_between_, double depth_) : Shape(x_,y_,z_) {
	width = width_;
	side_length = side_length_;
	theta = angle_between_;
	depth = depth_;
	rotation = rotation + 180;
}

TriangularPrism::TriangularPrism(double x_, double y_, double z_, 
	double width_, double side_length_, double angle_between_, double depth_, double rotation_) : Shape(x_,y_,z_,rotation_){
	width = width_;
	side_length = side_length_;
	theta = angle_between_;
	depth = depth_;
	rotation = rotation + 180;
}

double TriangularPrism::getWidth() {
	return width;
}

double TriangularPrism::getSideLength() {
	return side_length;
}

double TriangularPrism::getAngleBetween() {
	return theta;
}

double TriangularPrism::getDepth() {
	return depth;
}

double TriangularPrism::getPerpendicularHeight() {
	return side_length * sin((PI * theta) / 180);
}

void TriangularPrism::setWidth(double width)
{
	this->width = width;
}

void TriangularPrism::setSideLength(double sideLength)
{
	this->side_length = sideLength;
}

void TriangularPrism::setAngleBetween(double angle)
{
	this->theta = angle;
}

void TriangularPrism::setDepth(double depth)
{
	this->depth = depth;
}

void TriangularPrism::draw() {
	double i = width / 2;
	double j = getPerpendicularHeight() / 2;
	double k = depth / 2;
	double l = side_length * cos((PI * theta) / 180);

	glPushMatrix();
	positionInGL();
	setColorInGL();
	
	glBegin(GL_TRIANGLES);
		//Front triangle
		glVertex3d(-i,0,-k);
		glVertex3d(i-l,2*j,-k);
		glVertex3d(i,0,-k);

		//Back triangle
		glVertex3d(-i,0,k);
		glVertex3d(i-l,2*j,k);
		glVertex3d(i,0,k);
	glEnd();
	glBegin(GL_QUADS);
		//Left Quad
		glVertex3d(i, 0, k);
		glVertex3d(i, 0, -k);
		glVertex3d(i-l, 2*j, -k);
		glVertex3d(i-l, 2*j, k);

		//Right Quad
		glVertex3d(-i, 0, k);
		glVertex3d(-i, 0, -k);
		glVertex3d(i-l, 2*j, -k);
		glVertex3d(i-l, 2*j, k);
		
		//Bottom Quad
		glVertex3d(-i, 0, -k);
		glVertex3d(i, 0, -k);
		glVertex3d(i, 0, k);
		glVertex3d(-i, 0, k);
	glEnd();
	glPopMatrix();
}

TriangularPrism::~TriangularPrism() {

}
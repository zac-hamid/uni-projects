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
#include <vector>
#include <iostream>
#include "MyVehicle.hpp"
#include "Cylinder.hpp"
#include "RectangularPrism.hpp"
#include "TrapezoidalPrism.hpp"
#include "TriangularPrism.hpp"
#include "Wheel.hpp"

using namespace std;

RectangularPrism *body;
TrapezoidalPrism *cover;
Cylinder *frontWheelL, *frontWheelR, *backWheelL, *backWheelR;

MyVehicle::MyVehicle() {
	frontWheelL = new Cylinder(1, 0.3, 1, 0.5, 0.6, 32, 0, true, true);
	frontWheelR = new Cylinder(1, 0.3, -1, 0.5, 0.6, 32, 0, true, true);
	backWheelL = new Cylinder(-1, 0.3, 1, 0.5, 0.6, 32, 0, true, false);
	backWheelR = new Cylinder(-1, 0.3, -1, 0.5, 0.6, 32, 0, true, false);
	cover = new TrapezoidalPrism(0, 1.1, 0, 3, 1.5, 0.75, 0.8, 2, 180); 
	body = new RectangularPrism(0, 0.3, 0, 2, 0.8, 3.6, 90);

	addShape(cover);
	addShape(body);

	addShape(frontWheelL);
	addShape(frontWheelR);
	addShape(backWheelL);
	addShape(backWheelR);

	body->setColor(0, 1, 0);
	frontWheelL->setColor(0.49, 0.43, 0.35);
	frontWheelR->setColor(0.49, 0.43, 0.35);
	backWheelL->setColor(0.49, 0.43, 0.35);
	backWheelR->setColor(0.49, 0.43, 0.35);
	cover->setColor(0.1, 0.1, 0.5);
	driver->setObstacleDetectRadius(7.5f);
}

void MyVehicle::draw()
{
	glPushMatrix();
	
	positionInGL();
	setColorInGL();

	for (vector<Shape*>::iterator it = shapes.begin(); it != shapes.end(); it++) {
		(*it)->draw();
	}

	glPopMatrix();

}

MyVehicle::~MyVehicle() {
}
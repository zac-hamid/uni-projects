#ifndef MTRN3500_TRIANGULARPRISM_H
#define MTRN3500_TRIANGULARPRISM_H

#include "Shape.hpp"

class TriangularPrism : public Shape {
public:
	TriangularPrism();
	TriangularPrism(double x_, double y_, double z_,
		double width_, double side_length_, double angle_between_, double depth_);
	TriangularPrism(double x_, double y_, double z_, 
		double width_, double side_length_, double angle_between_, double depth_, double rotation_);
	~TriangularPrism();

	double getWidth();
	double getSideLength();
	double getAngleBetween();
	double getDepth();
	double getPerpendicularHeight();

	void setWidth(double width);
	void setSideLength(double sideLength);
	void setAngleBetween(double angle);
	void setDepth(double depth);

	void draw();

private:
	double width;
	double side_length;
	double theta;
	double depth;
};
#endif
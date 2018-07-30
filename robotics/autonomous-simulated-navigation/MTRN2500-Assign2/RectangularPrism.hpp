#ifndef MTRN3500_RECTANGULARPRISM_H
#define MTRN3500_RECTANGULARPRISM_H
#include "Shape.hpp"

class RectangularPrism : public Shape {
public:
	RectangularPrism();
	RectangularPrism(double x_, double y_, double z_, 
					double x_length, double y_length, double z_length);
	RectangularPrism(double x_, double y_, double z_, 
					double x_length, double y_length, double z_length, double rotation_);
	~RectangularPrism();

	double getWidth();
	double getHeight();
	double getDepth();

	void setWidth(double width);
	void setHeight(double height);
	void setDepth(double depth);

	void draw();

private:
	double width;
	double height;
	double depth;
};


#endif
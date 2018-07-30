#ifndef MTRN3500_TRAPEZOIDALPRISM_H
#define MTRN3500_TRAPEZOIDALPRISM_H

#include "Shape.hpp"

class TrapezoidalPrism : public Shape {
public:
	TrapezoidalPrism();
	TrapezoidalPrism(double x_, double y_, double z_, 
		double base_length_, double top_length_, double left_offset_, 
		double height_, double depth_);
	TrapezoidalPrism(double x_, double y_, double z_, 
		double base_length_, double top_length_, double left_offset_, 
		double height_, double depth_, double rotation_);

	double getBaseLength();
	double getTopLength();
	double getHeight();
	double getDepth();
	double getLeftOffset();

	void setBaseLength(double baseLength);
	void setTopLength(double topLength);
	void setHeight(double height);
	void setDepth(double depth);
	void setLeftOffset(double offset);

	void draw();

	~TrapezoidalPrism();

private:
	double base_length;
	double top_length;
	double height;
	double depth;
	double left_offset;
};

#endif
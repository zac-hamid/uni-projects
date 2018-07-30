#ifndef MTRN3500_CYLINDER_H
#define MTRN3500_CYLINDER_H

#include "Shape.hpp"

#define DEFAULT_CYLINDER_SLICES 32

class Cylinder : public Shape {
public:
	Cylinder();
	Cylinder(bool isRolling, bool isSteering);
	Cylinder(double x_, double y_, double z_, double radius_, double depth_, int slices_, bool isRolling, bool isSteering);
	Cylinder(double x_, double y_, double z_, double radius_, double depth_, int slices_, double rotation_, bool isRolling, bool isSteering);
	~Cylinder();

	double getRadius();
	double getDepth();
	double getRoll();
	bool getIsRolling();
	bool getIsSteering();
	int getSlices();

	void setRadius(double radius);
	void setDepth(double depth);
	void setSlices(int slices);
	void setRoll(double roll);

	void rollCylinder(double rollAmount);
	void rotateCylinder(double rotation);
	void steerCylinder(double amount);

	void draw();
protected:
	double radius;
	double depth;
	bool isRolling;
	bool isSteering;
	double roll;
	double defaultRot;

	int slices;
};
#endif
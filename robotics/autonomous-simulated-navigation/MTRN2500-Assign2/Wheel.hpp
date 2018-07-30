#ifndef MTRN3500_WHEEL_H
#define MTRN3500_WHEEL_H

#include "Cylinder.hpp"

class Wheel : public Cylinder {
public:
	Wheel(bool isRolling, bool isSteering);
	Wheel(double x_, double y_, double z_, double radius_, double depth_, int slices_, bool isRolling, bool isSteering);
	Wheel(double x_, double y_, double z_, double radius_, double depth_, int slices_, double rotation_, bool isRolling, bool isSteering);
	~Wheel();

	void draw();

private:
	Cylinder tire;
};

#endif
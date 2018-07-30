#ifndef MTRN3500_MYVEHICLE_H
#define MTRN3500_MYVEHICLE_H

#include "Vehicle.hpp"

class MyVehicle : public Vehicle{
public:
	MyVehicle();
	void draw();
	~MyVehicle();
};

#endif

#ifndef MTRN3500_VEHICLE_H
#define MTRN3500_VEHICLE_H

#include <iostream>
#include "Shape.hpp"
#include "VectorMaths.hpp"
#include <vector>
#include "AutonomousDriver.hpp"

class Vehicle : public Shape {
	friend void remoteDriver(Vehicle * vehicle, double x, double z, double r, double speed_, double steering_);
public:

	Vehicle();
	virtual ~Vehicle();

	virtual void update(double dt);
	virtual void update(double speed_, double steering_, double dt);

	virtual void draw() = 0;

	static const int MAX_FORWARD_SPEED_MPS = 10;
	static const int MAX_BACKWARD_SPEED_MPS = -4;
	static const int MAX_LEFT_STEERING_DEGS = 15;
	static const int MAX_RIGHT_STEERING_DEGS = -15;
	static const int VEHICLE_BOUNDS = 3;

	double getSpeed() const { return speed; }
	double getSteering() const { return steering; }

	void setSpeed(double speed);
	void setSteering(double steering);

	AutonomousDriver* getDriver();

	std::vector<Shape*>& getShapes();

	void addShape(Shape * shape);

	void setGoals(std::deque<GoalState> goals);

protected:
	double speed;     // m/s
	double steering;  // degrees

	std::vector<Shape *> shapes;
	AutonomousDriver* driver;
};

double clamp(double a, double n, double b);

#endif // for MTRN3500_VEHICLE_H


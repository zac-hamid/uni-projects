
#define _USE_MATH_DEFINES
#include <vector>
#include <math.h>
#include "Obstacle.hpp"
#include "ObstacleManager.hpp"
#include "Vehicle.hpp"
#include "Cylinder.hpp"

using namespace std;

Vehicle::Vehicle() {
	speed = steering = 0;
	driver = new AutonomousDriver(this);
};

Vehicle::~Vehicle()
{ 
	// clean-up added shapes
	for(int i = 0; i < shapes.size(); i++) {
		delete shapes[i];
	}
}

void Vehicle::update(double dt)
{
	speed = clamp(MAX_BACKWARD_SPEED_MPS, speed, MAX_FORWARD_SPEED_MPS);
	steering = clamp(MAX_LEFT_STEERING_DEGS, steering, MAX_RIGHT_STEERING_DEGS);

	// update position by integrating the speed
	x += speed * dt * cos(rotation * 3.1415926535 / 180.0);
	z += speed * dt * sin(rotation * 3.1415926535 / 180.0);

	// update heading
	rotation += dt * steering * speed;

	while (rotation > 360) rotation -= 360;
	while (rotation < 0) rotation += 360;


	if(fabs(speed) < .1)
		speed = 0;
	if(fabs(steering) < .1)
		steering = 0;

	for (vector<Shape*>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
		if (dynamic_cast<Cylinder*>(*it) != NULL) {
			Cylinder* c = (Cylinder*)(*it);
			if (c->getIsSteering()) {
				c->steerCylinder(this->getSteering());
			}
			if (c->getIsRolling()) {
				double rotationDeg = (((this->getSpeed()*dt) / (2 * M_PI * c->getRadius())) * 360.0);
				c->rollCylinder(-rotationDeg);
			}
		}
	}
}

void Vehicle::update(double speed_, double steering_, double dt) 
{
	//Obstacle avoidance
	if (driver->getIsEnabled()) {
		driver->update();
	}
	else {
		speed = speed + (speed_ - speed) * dt * 4;
		steering = steering + (steering_ - steering) * dt * 6;
	}
	update(dt);
}


void Vehicle::setSpeed(double speed)
{
	this->speed = clamp(MAX_BACKWARD_SPEED_MPS, speed, MAX_FORWARD_SPEED_MPS);
}

void Vehicle::setSteering(double steering)
{
	this->steering = clamp(MAX_LEFT_STEERING_DEGS, steering, MAX_RIGHT_STEERING_DEGS);
}

AutonomousDriver* Vehicle::getDriver()
{
	return driver;
}

std::vector<Shape*>& Vehicle::getShapes()
{
	return shapes;
}

void Vehicle::addShape(Shape * shape)
{
	shapes.push_back(shape);
}

void Vehicle::setGoals(std::deque<GoalState> goals)
{
	driver->setGoals(goals);
}

double clamp(double a, double n, double b) {

	if (a < b) {
		if (n < a) n = a;
		if (n > b) n = b;
	} else {
		if (n < b) n = b;
		if (n > a) n = a;
	}

	return n;

};


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
#include "AutonomousDriver.hpp"
#include "Vehicle.hpp"
#include "Obstacle.hpp"
#include "ObstacleManager.hpp"
#include "VectorMaths.hpp"
#include <math.h>

static GoalState targetGoal;

AutonomousDriver::AutonomousDriver(Vehicle* vehicle)
{
	this->subject = vehicle;
	this->obstacle_detect_radius = 10.0f;
	this->enabled = false;
	this->goalUpdateCount = 3;
	this->obstacle_FOV = 90.0f;
}

void AutonomousDriver::update()
{
	static int updateCounter;
	if (enabled) {
		if (goalsToVisit.size() > 0) {
			if (updateCounter >= goalUpdateCount) {
				targetGoal = *goalsToVisit.begin();
				double smallestDistance = sqrt(pow((goalsToVisit.begin()->x - subject->getX()), 2) + pow((goalsToVisit.begin()->z - subject->getZ()), 2));
				//Testing visitedGoals
				for (std::deque<GoalState>::iterator it = goalsToVisit.begin(); it != goalsToVisit.end(); ++it) {
					double dist = sqrt(pow((it->x - subject->getX()), 2) + pow((it->z - subject->getZ()), 2));
					if (dist <= smallestDistance) {
						smallestDistance = dist;
						targetGoal = (*it);
					}
					std::cout << it - goalsToVisit.begin() << ": " << (*it).x << ", " << (*it).z << std::endl;
				}

				if (smallestDistance <= GOAL_VISITED_DISTANCE) {
					visitedGoals.push_back(targetGoal);
				}

				std::cout << "Target Goal: " << targetGoal.x << ", " << targetGoal.z << std::endl;
				std::cout << "Distance: " << smallestDistance << std::endl;

				updateCounter = 0;
				std::cout << "-------------------" << std::endl;
			}

			float toTargetGoal[2] = { targetGoal.x - subject->getX(), targetGoal.z - subject->getZ() };
			float vehicleRotationRad = (subject->getRotation() / 360) * 2 * M_PI;
			float playerForward[2] = { cos(vehicleRotationRad) , sin(vehicleRotationRad) };
			float signedAngleBetween = scos::VectorMaths::signedAngle(toTargetGoal, playerForward);

			signedAngleBetween *= 360 / (2 * M_PI);

			subject->setSteering(-(signedAngleBetween/45)*Vehicle::MAX_LEFT_STEERING_DEGS);

			/* Obstacle checking */
			obstacles = ObstacleManager::get()->getObstacles(subject->getX(), subject->getZ(), obstacle_detect_radius);
			if (obstacles.size() > 0) {
				subject->setSpeed(Vehicle::MAX_FORWARD_SPEED_MPS*exp(-(double)obstacles.size() / 3) + 1);
			}
			else {
				subject->setSpeed(Vehicle::MAX_FORWARD_SPEED_MPS);
			}
			float totalAngle = 0.0f;
			int numObstaclesChecked = 0;
			for (std::list<Obstacle>::iterator obIt = obstacles.begin(); obIt != obstacles.end(); ++obIt) {
				float toObstacle[2] = { obIt->getX() - subject->getX(), obIt->getZ() - subject->getZ() };
				signedAngleBetween = scos::VectorMaths::signedAngle(toObstacle, playerForward);
				signedAngleBetween *= 360 / (2 * M_PI);

				if (signedAngleBetween < obstacle_FOV && signedAngleBetween > -obstacle_FOV) {

					float distance = sqrt(pow(toObstacle[0],2) + pow(toObstacle[1],2));
					float radius = obIt->getRadius();
					totalAngle += (pow(radius+1,3)/(distance+1)) * (signedAngleBetween/abs(signedAngleBetween))*(1 - (abs(signedAngleBetween) / obstacle_FOV)) * Vehicle::MAX_LEFT_STEERING_DEGS;
					numObstaclesChecked++;
				}
			}
			if (numObstaclesChecked > 0) {
				float steering = totalAngle / numObstaclesChecked;
				std::cout << "STEERING CORRECTION: " << steering << std::endl;
				subject->setSteering(subject->getSteering() + totalAngle / numObstaclesChecked);
			}
			/*------------------*/
		}
		else {
			//If there are no more goals, set speed to 0
			subject->setSpeed(0.0);
		}
		updateCounter++;
	}
}

void AutonomousDriver::isEnabled(bool enabled)
{
	this->enabled = enabled;
}

bool AutonomousDriver::getIsEnabled()
{
	return enabled;
}

//Makes sure that visited goals are not added back to goalsToVisit
void AutonomousDriver::setGoals(std::deque<GoalState> goals)
{
	goalsToVisit.clear();
	for (std::deque<GoalState>::iterator goalsIter = goals.begin(); goalsIter != goals.end(); ++goalsIter) {
		bool found = false;
		for (std::deque<GoalState>::iterator it = visitedGoals.begin(); it != visitedGoals.end(); ++it) {
			if (it->x == goalsIter->x && it->z == goalsIter->z) {
				found = true;
			}
		}
		if (!found) {
			goalsToVisit.push_back(*goalsIter);
		}
	}
}

void AutonomousDriver::setObstacleDetectRadius(float radius)
{
	this->obstacle_detect_radius = radius;
}

void AutonomousDriver::setObstacleFOV(float fov)
{
	this->obstacle_FOV = fov;
}

void AutonomousDriver::setVehicle(Vehicle * v)
{
	this->subject = v;
}
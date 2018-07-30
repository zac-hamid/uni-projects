#ifndef MTRN3500_AUTONOMOUS_DRIVER_H
#define MTRN3500_AUTONOMOUS_DRIVER_H

#define GOAL_VISITED_DISTANCE 10.0

#include <deque>
#include "Messages.hpp"
#include "Obstacle.hpp"
#include "ObstacleManager.hpp"

class Vehicle;
class AutonomousDriver {
public:
	AutonomousDriver(Vehicle* vehicle);
	void update();
	void isEnabled(bool enabled);
	bool getIsEnabled();
	void setGoals(std::deque<GoalState> goals);
	void setObstacleDetectRadius(float radius);
	void setObstacleFOV(float fov);
	void setVehicle(Vehicle* v);


private:
	bool enabled;
	int goalUpdateCount;
	Vehicle* subject;
	std::deque<GoalState> goalsToVisit;
	std::deque<GoalState> visitedGoals;
	std::list<Obstacle> obstacles;

	float obstacle_detect_radius;
	float obstacle_FOV;
};

#endif
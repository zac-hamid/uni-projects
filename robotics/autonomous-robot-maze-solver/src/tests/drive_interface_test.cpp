#include <Arduino.h>
#include "hardware.h"
#include "trajectory_controller.h"

#include "locomotion.cpp.h"
#include "sensors.cpp.h"
#include "comms.cpp.h"


drive::TrajectoryController traj_controller;

void setup() {

}

void loop() {
    traj_controller.tick();

    if (traj_controller.is_complete()) {
        traj_controller.set_forwards_goal(10);
    }

}

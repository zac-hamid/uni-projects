#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
//#include "units.inc"
//#include "locomotion.cpp.h"
//#include "sensors.cpp.h"
#include "hardware_definition.h"
#include "encoded_wheel.h"
#include "robot_constants.h"
#include "trajectory_controller.h"
//#include "comms.cpp.h"
#include "math_utils.h"

//#define BT_ONLY_TEST

drive::TrajectoryController traj_controller;

void rotate_left() {
#ifndef BT_ONLY_TEST
    traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::LEFT, PI / 2.0);
#endif
}

void rotate_right() {
#ifndef BT_ONLY_TEST
    traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::RIGHT, PI / 2.0);
#endif
}

void forward() {
    hardware::bluetooth::print("Setting forwards goal");
#ifndef BT_ONLY_TEST
    traj_controller.set_forwards_goal(units::millimeters{250});
#endif
    hardware::bluetooth::print("Setting forwards goal... done");
}

void stop() {
#ifndef BT_ONLY_TEST
    traj_controller.cancel_all_goals();
#endif
}

void setup() {
    hardware::serial::enable(115200);
    hardware::bluetooth::enable(9600);

    hardware::bluetooth::print("Enabling shite");

#ifndef BT_ONLY_TEST
    hardware::bluetooth::print_line("i2c");
    hardware::i2c::enable();
    hardware::bluetooth::print_line("imu");
    hardware::imu::enable();
    hardware::bluetooth::print_line("l_lidar");
    hardware::left_lidar::enable();
    hardware::bluetooth::print_line("r_lidar");
    hardware::right_lidar::enable();

    // Wait until we get our first reading
    while(!hardware::imu::update());
    // After this wait for IMU stability
    hardware::imu::stabilize();
#endif

    hardware::bluetooth::print("good to go boys\n\n");
}

void loop() {
    if (hardware::bluetooth::input_byte_in_buffer()) {
        char c = hardware::bluetooth::read();
        switch(c) {
            case 'f': forward(); break;
            case 'l': rotate_left(); break;
            case 'r': rotate_right(); break;
            case 's': stop(); break;
            default: hardware::serial::print(c); hardware::serial::flush(); break;
        }
    }

#ifndef BT_ONLY_TEST
    traj_controller.tick();
#endif
}



#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "sensors.cpp.h"
#include "hardware_definition.h"
#include "encoded_wheel.h"
#include "robot_constants.h"
#include "trajectory_controller.h"
#include "math_utils.h"
#include "pose.h"

drive::TrajectoryController traj_controller;

void speed_controller_demo() {
    static int curr = 0;
    static unsigned long last = millis();
    while(hardware::bluetooth::input_byte_in_buffer() == 0) {
        if (millis() - last > 150) {
            curr = (curr + 1)%60;

            hardware::bluetooth::print("set to ");
            hardware::bluetooth::print_line(curr);
            traj_controller.set_velocity_left(RPM_TO_RADS_S(curr));
            traj_controller.set_velocity_right(RPM_TO_RADS_S(curr));
        }
        traj_controller.tick();
    }
}

void set_wheel_power_demo() {
    int curr = Serial2.parseInt();
    hardware::bluetooth::print("Set wheel power to ");
    hardware::bluetooth::print_line(curr);
    hardware::left_motor::forward(units::percentage{curr});
    hardware::right_motor::forward(units::percentage{curr});
}

void rotate_left_demo() {
    traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::LEFT, PI / 2.0);
}

void rotate_right_demo() {
    traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::RIGHT, PI / 2.0);
}

void forward_demo() {
    hardware::bluetooth::print("Setting forwards goal");
    traj_controller.set_forwards_goal(units::millimeters{250});
    hardware::bluetooth::print("Setting forwards goal... done");
}

void odom_demo() {
    auto last_print_time = 0;
    while(1) {
        traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::LEFT, PI / 2.0);
        traj_controller.tick();
        auto now = millis();
        if (abs(now - last_print_time) > 500) {
            hardware::bluetooth::print(traj_controller.get_pose().x);
            hardware::bluetooth::print(" ");
            hardware::bluetooth::print(traj_controller.get_pose().y);
            hardware::bluetooth::print(" ");
            hardware::bluetooth::print(traj_controller.get_pose().theta);
            hardware::bluetooth::print("\n");
            last_print_time = now;
        }

    }
}

void speed_control_demo() {
    auto last_step_start = millis();
    traj_controller.set_velocity_left(RPM_TO_RADS_S(200));
    traj_controller.set_velocity_right(RPM_TO_RADS_S(200));

    while(1) {
        traj_controller.tick();
        // Input square wave
        if (millis() - last_step_start > 1000) {
            traj_controller.set_velocity_left(RPM_TO_RADS_S(0));
            traj_controller.set_velocity_right(RPM_TO_RADS_S(0));
        } 
        if (millis() - last_step_start > 2000) {
            return;
        }
    }
}

void speed_tune() {
    float p, i, d;
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    p = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    i = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    d = Serial2.parseFloat();

    traj_controller.set_speed_tunings(p, i, d);

}

void forward_tune() {
    float p, i, d;
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    p = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    i = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    d = Serial2.parseFloat();

    traj_controller.set_forward_tunings(p, i, d);

}

void drift_tune() {
    float p, i, d;
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    p = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    i = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    d = Serial2.parseFloat();

    traj_controller.set_drift_lidar_tunings(p, i, d);

}

void rotate_tune() {
    float p, i, d;
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    p = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    i = Serial2.parseFloat();
    while(hardware::bluetooth::input_byte_in_buffer() == 0);
    d = Serial2.parseFloat();

    traj_controller.set_rotation_tunings(p, i, d);

}

void setup() {
    hardware::serial::enable(115200);
    hardware::bluetooth::enable(9600);

    hardware::i2c::enable();
    hardware::imu::enable();
    hardware::left_lidar::enable();
    hardware::right_lidar::enable();
    hardware::front_lidar::enable();

    // Wait until we get our first reading
    while(!hardware::imu::update());
    // After this wait for IMU stability
    hardware::imu::stabilize();

    hardware::bluetooth::print("hi");

    traj_controller.reset();
}

void select_mode(int mode) {
    switch(mode) {
        case '1': speed_controller_demo(); break;
        case '2': rotate_left_demo(); break;
        case '3': rotate_right_demo(); break;
        case 'f': forward_demo(); break;
        case 'g': forward_tune(); break;
        case 'd': drift_tune(); break;
        case 'o': odom_demo(); break;
        case 's': speed_control_demo(); break;
        case 't': speed_tune(); break;
        case 'r': rotate_tune(); break;
        case 'p': set_wheel_power_demo(); break;
        case 'c': traj_controller.cancel_all_goals(); break;
        default: break;
    }
}

void loop() {
    if (hardware::bluetooth::input_byte_in_buffer()) {
        int mode = hardware::bluetooth::read();
        select_mode(mode);
    }

    if (hardware::serial::input_byte_in_buffer()) {
        int mode = hardware::serial::read();
        select_mode(mode);

    }


    traj_controller.tick();
}



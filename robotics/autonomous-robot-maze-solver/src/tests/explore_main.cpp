#include "hardware.h"
#include "trajectory_controller.h"
#include "explore.h"

drive::TrajectoryController traj_controller;
Explore explore = Explore(traj_controller);

void setup() {
    //set Serial2 comms to 115200 bps
    Serial.begin(9600);
    //Serial.begin(115200);
    //hardware::Serial::enable(115200);
    //set bluetooth to 9600 bps
    Serial2.begin(9600);
    //hardware::bluetooth::enable(9600);
    //enable comms to devices
    hardware::i2c::enable();
    //hardware::i2c::clock(hardware::i2c::fast_mode);
    hardware::imu::enable();
    hardware::front_lidar::enable();
    hardware::left_lidar::enable();
    hardware::right_lidar::enable();
    hardware::display::enable();
    hardware::display::clear();

    traj_controller.reset();
}

void loop() {
    traj_controller.tick();

    if (!explore.complete()) {
       explore.tick();
    }
}

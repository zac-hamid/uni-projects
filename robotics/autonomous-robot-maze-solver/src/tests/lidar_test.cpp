
#include <Arduino.h>
#include "hardware.h"
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

    hardware::serial::print("hi");
}

void loop() {
    delay(1000);
    unsigned long start = micros();
    
    units::millimeters dist_r = hardware::right_lidar::distance();
    units::millimeters dist_l = hardware::left_lidar::distance();
    units::millimeters dist_f = hardware::front_lidar::distance();
    //hardware::right_lidar::distance();

    hardware::serial::print(dist_l.count());
    hardware::serial::print(" ");
    hardware::serial::print(dist_f.count());
    hardware::serial::print(" ");
    hardware::serial::print_line(dist_r.count());

    unsigned long end = micros();
    hardware::serial::print("Read took ");
    hardware::serial::print_line(end - start);

}

#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "units.inc"
#include "sensors.cpp.h"
#include "hardware_definition.h"
#include "robot_constants.h"
#include "trajectory_controller.h"
#include "math_utils.h"

// Input-related variables
char input_byte = '\n';
String input = "";
bool cleared = false;
bool input_displayed = false;

drive::TrajectoryController tc;

void drive_forward(int);
void rotate(float);

String getValue(String, char, int);

void getInput(void);
void clearInput(void);

void setup() {
    input = "";
    Serial.begin(115200);
    // hardware::bluetooth::enable(9600);
    Serial2.begin(9600);
    hardware::i2c::enable();

    hardware::left_lidar::enable();
    hardware::right_lidar::enable();

    hardware::imu::enable();
    // Wait until we get our first reading
    while(!hardware::imu::update());
    // After this wait for IMU stability
    hardware::imu::stabilize();
}

void tick() {
    tc.tick();
}

void loop() {
    tick();

    getInput();
    if (input == "1") {
        Serial2.println("Moving forward 7 cells...");
        drive_forward(6);
        Serial2.println("Moved successfully!");
        clearInput();
    } else if (input == "2") {
        Serial2.println("Doing square wave...");

        drive_forward(1);
        rotate(90);
        drive_forward(1);
        rotate(90);
        drive_forward(1);
        rotate(-90);
        drive_forward(1);
        rotate(-90);
        drive_forward(1);
        rotate(90);
        drive_forward(1);
        rotate(90);
        drive_forward(1);
        rotate(-90);
        drive_forward(1);
        rotate(-90);
        drive_forward(1);

        Serial2.println("Moved successfully!");
        clearInput();
    } else if (input == "rl") {
        Serial2.println("Rotating 90 degrees to the left...");

        rotate(90);

        Serial2.println("Rotated successfully!");
        clearInput();
    } else if (input == "rr") {
        Serial2.println("Rotating 90 degrees to the right...");

        rotate(-90);

        Serial2.println("Rotated successfully!");
        clearInput();
    } else if (input == "d") {
        Serial2.println("Moving forward by 1 cell...");

        drive_forward(1);

        Serial2.println("Moved successfully!");
        clearInput();
    } else if (input == "yaw") {
        hardware::imu::update();
        Serial2.println(hardware::imu::yaw());
    }

    // May use this for on the fly tuning of PID
    if (getValue(input, ' ', 0) == "tune") {
        if (getValue(input, ' ', 1) == "drive") {
            tc.set_speed_tunings(getValue(input, ' ', 2).toFloat(), getValue(input, ' ', 3).toFloat(), getValue(input, ' ', 4).toFloat());
        } else if (getValue(input, ' ', 1) == "rotate") {
            tc.set_rotation_tunings(getValue(input, ' ', 2).toFloat(), getValue(input, ' ', 3).toFloat(), getValue(input, ' ', 4).toFloat());    
        } else if (getValue(input, ' ', 1) == "drift") {
            if (getValue(input, ' ', 2) == "lidar") {
                tc.set_drift_lidar_tunings(getValue(input, ' ', 3).toFloat(), getValue(input, ' ', 4).toFloat(), getValue(input, ' ', 5).toFloat());
            } else if (getValue(input, ' ', 2) == "imu") {
                tc.set_drift_imu_tunings(getValue(input, ' ', 3).toFloat(), getValue(input, ' ', 4).toFloat(), getValue(input, ' ', 5).toFloat());
            }
        }
        Serial2.println("updated tunings to: ");
        Serial2.print(getValue(input, ' ', 2).toFloat());
        Serial2.print(" ");
        Serial2.print(getValue(input, ' ', 3).toFloat());
        Serial2.print(" ");
        Serial2.println(getValue(input, ' ', 4).toFloat());
        clearInput();
    } else if (getValue(input, ' ', 0) == "rotate") { // Set a rotation goal manually
        rotate(getValue(input, ' ', 1).toDouble());
        clearInput();
    } else if (getValue(input, ' ', 0) == "drive") { // Set a forwards drive goal manually
        drive_forward(getValue(input, ' ', 1).toDouble());
        clearInput();
    }
}

void drive_forward(int cells) {
    tc.set_forwards_goal(units::millimeters{cells * MAZE_CELL_SIZE});
    while(!tc.is_complete()) {
        tick();
    }
}

void rotate(float deg) {
    if (deg > 0) {
        tc.set_rotate_goal(drive::TrajectoryController::LEFT, DEG2RAD(deg));
    } else {
        tc.set_rotate_goal(drive::TrajectoryController::RIGHT, DEG2RAD(-deg));
    }
    while(!tc.is_complete()) tick();
}

// Function to read the input
void getInput() {
    do {
        // Has anything been typed?
        if (Serial2.available() > 0) {
            if (!cleared) {
                clearInput();
                cleared = true;
                input_displayed = false;
            }
            input_byte = Serial2.read();
            if (input_byte != '\n') input += input_byte;
        }
    } while(input_byte != '\n');
    cleared = false;
}

void clearInput() {
    input = "";
}

// Just for some testing stuff found on StackOverflow
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
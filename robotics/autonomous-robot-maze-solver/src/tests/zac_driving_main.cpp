#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "sensors.cpp.h"
#include "locomotion.cpp.h"
#include "hardware_definition.h"

// Input-related variables
char input_byte = '\n';
String input = "";
bool cleared = false;
bool input_displayed = false;


// IMU stuff
float world_yaw = 0.0f;
bool first_detection = false;
float yaw_threshold = 2.0f;
unsigned long yaw_wait_time = 3000000;
unsigned long prev_yaw_time = 0;
unsigned long curr_yaw_time = 0;
uint8_t direction = 0;

void getInput(void);
void clearInput(void);

void setup() {
    input = "";
    Serial.begin(115200);
    hardware::i2c::enable();
    hardware::imu::enable();
    
}

void loop() {
    getInput();

    if (input == "1" || first_detection) {
        if (!input_displayed) {
            Serial.println("Task #1 chosen...");
            input_displayed = true;
        }
        if (!first_detection) {
            first_detection = true;
            Serial.println("Waiting for first IMU reading...");
            // Wait until we get our first reading
            while(!hardware::imu::update());
            // After this wait for IMU stability
            hardware::imu::stabilize();
            // Get yaw offset for calculations
            world_yaw = hardware::imu::yaw();
            Serial.print("World yaw: ");
            Serial.println(world_yaw);
            Serial.println("Starting facing North (not true North)");
        } else {
            hardware::imu::update();
            float yaw_diff = hardware::imu::yaw() - world_yaw;
            Serial.print("Current yaw = ");
            Serial.println(hardware::imu::yaw());
            
            for (int i = -3; i <= 3; i++) {
                if (i != 0) {
                	// Is the change in yaw from initial a multiple of 90 degrees?
                    if (yaw_diff >= (i*90 - yaw_threshold) && yaw_diff <= (i*90 + yaw_threshold)) {
                        // Figure out which way is being faced out of N E S W
                        int facing_index = 0;
                        if (i < 0) {
                            facing_index = 3 - ((-1)*i - 1);
                        } else {
                            facing_index = i;
                        }

                        switch (facing_index) {
                            case 0:
                                Serial.println("Facing North!");
                                break;
                            case 1:
                                Serial.println("Facing East!");
                                break;
                            case 2:
                                Serial.println("Facing South!");
                                break;
                            case 3:
                                Serial.println("Facing West!");
                                break;
                            default:
                                Serial.println("I don't know what direction this is...");
                                break;
                        }
                        
                        // If we're facing the same direction as last time
                        if (facing_index == direction) {
                            // Update timer
                            if (curr_yaw_time == 0) prev_yaw_time = micros();
                            curr_yaw_time += micros() - prev_yaw_time;
                            // Have we been facing the same direction for a certain amount of time? If so, scan walls and update wall array
                            // if (curr_yaw_time >= yaw_wait_time) {
                            //     Serial.println("Facing wall for long enough!");

                            //     first_detection = false;
                            //     clearInput();
                            // }
                        } else {
                            // Reset timer, update direction
                            curr_yaw_time = 0;
                            direction = facing_index;
                        }
                    }
                }
            }
        }
    } else if (input == "2") {
        if (!input_displayed) {
            Serial.println("Task #2 chosen...");
            input_displayed = true;
        }
    } else if (input == "3") {
        if (!input_displayed) {
            Serial.println("Task #3 chosen...");
            input_displayed = true;
        }
    } else if (input == "4") {
        if (!input_displayed) {
            Serial.println("Task #4 chosen...");
            input_displayed = true;
        }
    } else if (input == "test") {
        if (!input_displayed) {
            Serial.println("Test task chosen...");
            input_displayed = true;
        }
    }
}

// Function to read the input
void getInput() {
    do {
        // Has anything been typed?
        if (Serial.available() > 0) {
            if (!cleared) {
                clearInput();
                cleared = true;
                input_displayed = false;
            }
            input_byte = Serial.read();
            if (input_byte != '\n') input += input_byte;
        }
    } while(input_byte != '\n');
    cleared = false;
}


void clearInput() {
    input = "";
}
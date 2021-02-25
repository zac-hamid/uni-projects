#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "locomotion.cpp.h"
#include "sensors.cpp.h"
#include "hardware_definition.h"
#include "encoded_wheel.h"
#include "robot_constants.h"



void setup() {
    hardware::right_motor::enable();
    hardware::left_motor::enable();
}


void loop() {
    int val = 10;

    while (val < 80) {
        hardware::right_motor::forward(units::percentage{val});
        hardware::left_motor::forward(units::percentage{val});

        delay(100);
        val++;
    }
}
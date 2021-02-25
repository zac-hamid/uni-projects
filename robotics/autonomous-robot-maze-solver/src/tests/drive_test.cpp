#include <Arduino.h>
#include <hardware.h>
#include <hardware_definition.h>
#include <locomotion.cpp.h>
#include <locomotion.h>
#include <sensors.cpp.h>


void setup() {
    Serial.begin(9600);

    hardware::left_wheel::enable();
    hardware::left_motor::enable();
    hardware::right_wheel::enable();
    hardware::right_motor::enable();

    hardware::left_motor::forward(units::percentage{80});
    hardware::left_motor::forward(units::percentage{40});


}

void loop() {
    delay(2000);
    hardware::left_motor::stop();
    hardware::right_motor::stop();
}

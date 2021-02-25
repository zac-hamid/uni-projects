#include <Arduino.h>
#include "Hardware/hardware.h"
#include "Hardware/locomotion.cpp.h"
#include "Hardware/units.h"
#include "Hardware/hardware_definition.h"
#include <pins_arduino.h>

hardware::digital_pin<12> led{};
hardware::digital_pin<8> led2{};
hardware::motor<hardware::digital_pin<4>, hardware::digital_pin<5>> motor_2;
hardware::motor<hardware::digital_pin<7>, hardware::digital_pin<6>> motor_1;

double curr = 0;
void setup() {
    led.config_io_mode(hardware::io_mode::output);
    motor_2.enable();
    motor_1.enable();

    Serial.begin(9600);

    hardware::left_encoder::enable();
}

void loop() {
    curr += 0.5;
    if (curr > 100) curr = 0;
    motor_1.backward(units::percentage{100});
    motor_2.backward(units::percentage{100});
    delay(1000);
    //led.pwm_write(units::percentage{curr});
    //led2.pwm_write(units::percentage{curr});
    auto encoder_count = hardware::left_encoder::count();

    Serial.println(encoder_count);
}

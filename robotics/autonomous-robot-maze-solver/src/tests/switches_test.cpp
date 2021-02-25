#include <Arduino.h>
#include "hardware.h"

// VISION DIP IS DIP 1, EXPLORE IS DIP 4
void setup() {
    hardware::vision_dip::config_io_mode(hardware::io_mode::input);
    hardware::explore_dip::config_io_mode(hardware::io_mode::input);
    hardware::reset_button::config_io_mode(hardware::io_mode::input);
}


void loop() {
    
    // Can change order depending on what the desired behaviour of DIP switches is
    // Right now, if both are on, vision is prioritised
    if (hardware::vision_dip::read() == hardware::logic_level::high) {
        // Vision setting enabled
    } else if (hardware::explore_dip::read() == hardware::logic_level::high) {
        // Explore setting enabled
    }

    if (hardware::reset_button::read() == hardware::logic_level::high) {
        // Reset button currently pressed, maybe we wanna check if it's pressed over a few loops to be sure it wasn't accidental

    }
}
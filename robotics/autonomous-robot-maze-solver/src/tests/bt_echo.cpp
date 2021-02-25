#include "hardware.h"
#include "comms.h"

void setup() {
    hardware::bluetooth::enable(9600);
    hardware::serial::enable(115200);
}

void loop() {

    if (hardware::bluetooth::input_byte_in_buffer()) {
        char c = hardware::bluetooth::read();

        hardware::serial::print(c);
    }

    if (hardware::serial::input_byte_in_buffer()) {
        char c = hardware::serial::read();
        hardware::bluetooth::print(c);
    }
}

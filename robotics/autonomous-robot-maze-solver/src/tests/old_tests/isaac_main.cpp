#include <Arduino.h>
#include "hardware.h"
#include "hardware_definition.h"
#include "comms.cpp.h"
#include "comms.h"
#include "plan.h"

using namespace hardware;

char msg[200];

void setup() {
    serial::enable(115200);
    bluetooth::enable(9600);
}

void loop() {
    if (bluetooth::input_byte_in_buffer()) {
        maze_layout_message msg = receive_maze_layout(); 
        maze_layout maze = parse_maze_layout(msg);
        Plan p{maze};
        p.solve(); 
    }
}

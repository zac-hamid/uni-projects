#include "hardware.h"
#include "hardware_definition.h"
#include "comms.h"
#include "common_maze_rep.h"
#include "comms.cpp.h"
#include "units.h"

void setup() {
    hardware::bluetooth::enable(115200);
    hardware::serial::enable(115200);
}

void loop() {
    common::MazeRep maze;

    auto valid = hardware::receive_common_maze_layout<hardware::serial>(
            maze, 
            units::milliseconds(100000000));
    
    if (valid) {
        hardware::serial::print("Recieved valid maze!\n");
        char msg[300];
        maze.to_string(msg, 300);
        hardware::serial::print(msg);
        hardware::serial::flush();
    } else {
        //hardware::serial::print("valid false!\n chksum = ");
        //hardware::serial::print(maze.chk_);
    }
    
}

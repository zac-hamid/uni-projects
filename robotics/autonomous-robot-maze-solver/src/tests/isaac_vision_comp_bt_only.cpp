#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "encoded_wheel.h"
#include "robot_constants.h"
#include "trajectory_controller.h"
#include "math_utils.h"
#include "queue.h"
#include "plan.h"

hardware::maze_layout maze;
hardware::maze_layout_message msg;  
Queue<Move> moves;

void setup() {
	hardware::bluetooth::enable(9600);

	hardware::bluetooth::print_line(F("===================="));
	hardware::bluetooth::print_line(F("=     MTRN4110     ="));
	hardware::bluetooth::print_line(F("=    EncodeThis    ="));
	hardware::bluetooth::print_line(F("=       ....       ="));
	hardware::bluetooth::print_line(F("=    Vision run    ="));
	hardware::bluetooth::print_line(F("====================\n"));
	hardware::bluetooth::print_line(
            F("... waiting for vision received ..."));
	uint8_t i = 0; 

	bool maze_received = false;
	while (!maze_received) {
		hardware::bluetooth::print_line(F("Trying to connect ..."));
		if (hardware::bluetooth::input_byte_in_buffer()) {
			msg = hardware::receive_maze_layout();
			hardware::bluetooth::print_line(F("... maze received ..."));
			if (msg.foundMaze()) {
				maze = hardware::parse_maze_layout(msg); 
				print_maze_serial(maze);
				Plan p{maze};
				moves = p.solve(); 
				hardware::bluetooth::print_line(F("... maze received ..."));
				maze_received = true; 
			} else {
				hardware::bluetooth::print_line(F("Bad read maze :( "));
			}
		}
		delay(100);
	}


    hardware::bluetooth::print(F("Move Queue length: "));
    hardware::bluetooth::print_line(moves.size());
}

void loop() {
	
}


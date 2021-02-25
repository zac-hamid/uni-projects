#include <Arduino.h>
// #include "hardware.h"
//#include "comms.h"
#include "comms.cpp.h"
#include "units.h"
// #include "hardware_definition.h"
#include <pins_arduino.h>

using namespace hardware;

void setup() {
	serial::enable(9600);
	delay(50);
	serial::print_line("BEGIN!");

	display::enable();
	display::clear();
	// display::print("hi");
	maze_layout_message msg = receive_maze_layout();
	maze_layout maze{3,2};
	print_maze_serial(maze);
	serial::print_line("=========");
	maze =	parse_maze_layout(msg); 
	// serial::print_line("PRINTING MAZE");
	print_maze_serial(maze);
}
void loop() {
}
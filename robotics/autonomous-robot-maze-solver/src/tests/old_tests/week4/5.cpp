#include <Arduino.h>
#include "comms.cpp.h"
#include "units.h"
#include <pins_arduino.h>

using namespace hardware;

void setup() {
	serial::enable(115200);
	bluetooth::enable(115200); 
	//print_maze_serial(parse_maze_layout(receive_maze_layout()));
}
void loop() {
	print_maze_serial(parse_maze_layout(receive_maze_layout()));
}
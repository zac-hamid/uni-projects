// #include <Arduino.h>
// #include "comms.cpp.h"
// #include "units.h"
// #include <pins_arduino.h>

// using namespace hardware;

// String str = "";
// char buf[100] = ""; 

// void setup() {
// 	serial::enable(115200);
// 	bluetooth::enable(115200);
// }
// void loop() {
// 	if (Serial.available() > 0) {
//         str = ""; 
//         while (Serial.available() > 0) {
//             delay(5);
// 			str += static_cast<char>(serial::read());
//         }
// 		str.toCharArray(buf, str.length()); 
// 		bluetooth::print_line(buf);
//     }
// }
/*****************/
/******* 1 *******/
/*****************/
// #include <Arduino.h>
// #include "comms.cpp.h"
// #include "units.h"
// #include <pins_arduino.h>

// using namespace hardware;

// String str = "";
// char buf[100] = ""; 

// void setup() {
// 	serial::enable(115200);
// 	bluetooth::enable(115200);
// 	serial::print_line("BEGIN!");
// }
// void loop() {
// 	if (serial::input_byte_in_buffer() > 0) {
// 		//serial::print("S0: ");
//         str = "";
//         while (serial::input_byte_in_buffer() > 0) {
// 			str+= static_cast<char>(serial::read());
// 			delay(5);
//         }
// 		str.toCharArray(buf, str.length());
// 		serial::print_line(buf);
// 		if (str.substring(0,2) == "AT") {
// 			serial::print_line("OK");
// 		}
//     }
// }
/*****************/
/******* 2 *******/
/*****************/
// #include <Arduino.h>
// #include "comms.cpp.h"
// #include "units.h"
// #include <pins_arduino.h>

// using namespace hardware;

// String str = "";
// char buf[100] = ""; 

// void setup() {
// 	serial::enable(115200);
// 	bluetooth::enable(115200);
//     display::enable();
//     display::clear();
// }
// void loop() {
// 	if (Serial1.available() > 0) {
// 		serial::print("BT: ");
//         str = "";
//         while (Serial1.available() > 0) {
//             delay(5);
// 			str += static_cast<char>(bluetooth::read());
//         }
// 		str.toCharArray(buf, str.length()); 
// 		serial::print_line(buf);
// 		display::clear(); 
//         display::print(buf);
// 		//delay(100);
//     }
// }
// #include <Arduino.h>
// #include "comms.cpp.h"
// #include "units.h"
// #include <pins_arduino.h>

// using namespace hardware;

// String str = "";
// char buf[100] = ""; 

// void setup() {
// 	serial::enable(115200);
// 	bluetooth::enable(115200);
//     display::enable();
//     display::clear();
// }
// void loop() {
// 	if (bluetooth::input_byte_in_buffer() > 0) {
// 		delay(10);
//         bluetooth::read_bytes(buf, bluetooth::input_byte_in_buffer());
// 		serial::print_line(buf);
// 		display::clear(); 
// 		for ()
//         display::print(buf);
//     }
// }
/*****************/
/******* 5 *******/
/*****************/
#include <Arduino.h>
#include "comms.cpp.h"
#include "units.h"
#include "comms.h"
#include <pins_arduino.h>

using namespace hardware;

void setup() {
	serial::enable(115200);
	bluetooth::enable(115200); 
}
void loop() {
	print_maze_serial(parse_maze_layout(receive_maze_layout()));
}

/***********************/
/******* GENERAL *******/
/***********************/
// #include <Arduino.h>
// #include "comms.cpp.h"
// #include "units.h"
// #include <pins_arduino.h>

// using namespace hardware;

// String str = "";
// char buf[100] = ""; 

// void setup() {
// 	serial::enable(9600);
// 	bluetooth::enable(115200);
// 	serial::print_line("BEGIN!");
// }

// void loop() {
// 	if (bluetooth::input_byte_in_buffer() > 0) {
// 		bluetooth::read_bytes(buf, bluetooth::input_byte_in_buffer());
// 		serial::print(buf);
// 	} 

// 	if (serial::input_byte_in_buffer() > 0) {
// 		serial::read_bytes(buf, serial::input_byte_in_buffer());
// 		serial::print(buf);
// 		bluetooth::print(buf);
// 	} 
// }

#include <Arduino.h>
#include "comms.cpp.h"
#include "units.h"
#include <pins_arduino.h>

using namespace hardware;

String str = "";
char buf[100] = ""; 

void setup() {
	serial::enable(115200);
	bluetooth::enable(115200);
}
void loop() {
	if (Serial1.available() > 0) {
        str = ""; 
        while (Serial1.available() > 0) {
            delay(5);
			str += static_cast<char>(bluetooth::read());
        }
		str.toCharArray(buf, str.length()); 
		serial::print_line(buf);
    }
}
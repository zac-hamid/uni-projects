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
	if (Serial.available() > 0) {
        str = ""; 
        while (Serial.available() > 0) {
            delay(5);
			str += static_cast<char>(serial::read());
        }
		str.toCharArray(buf, str.length()); 
		bluetooth::print_line(buf);
    }
}
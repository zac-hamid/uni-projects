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
	serial::print_line("BEGIN!");
}
void loop() {
	if (Serial.available() > 0) {
		//serial::print("S0: ");
        str = "";
        while (Serial.available() > 0) {
			str+= static_cast<char>(serial::read());
			delay(5);
        }
		str.toCharArray(buf, str.length());
		serial::print_line(buf);
		if (str.substring(0,2) == "AT") {
			serial::print_line("OK");
		}
    }
}
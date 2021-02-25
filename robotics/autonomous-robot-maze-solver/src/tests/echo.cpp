#include <Arduino.h>
#include <HardwareSerial.h>

void setup() {
    Serial.begin(9600, SERIAL_8N1);
    pinMode(13, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        char c;
        c = Serial.read();
        Serial.write(c);
        Serial.flush();
    }
}

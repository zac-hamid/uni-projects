#include <Arduino.h>

#include "hardware.h"
#include "hardware_definition.h"
#include "comms.h"
#include "comms.cpp.h"
#include "queue.h"

using namespace hardware;

Queue<int> q; 
String str; 
char buf[200];

void setup() {
    serial::enable(9600);    
}

void loop() {
    q.push(1);
    delay(2000);
}
#ifndef COMMS_H_
#define COMMS_H_

#include "Arduino.h"
//#include "comms.cpp.h"
#include "external_DFRobot_LCD.h"

#include "maze_layout.h"

// #define PRINT_WALLS 1
// #define SHOW_MSG 1

namespace hardware {
  void print_maze_serial(maze_layout& maze);
  char * receive_command(HardwareSerial &serial);
}

#endif // COMMS_H_ 

#ifndef CONTROLLER_CONSTANTS_H_
#define CONTROLLER_CONSTANTS_H_

#include <avr/pgmspace.h>

#include "units.h"

namespace drive {

static const PROGMEM unsigned int ENCODER_VELOCITY_UPDATE_TIME_MS{10};
static const PROGMEM unsigned int TRAJ_CONTROLLER_TICK_PERIOD{50};
static const PROGMEM float ROTATION_TOLERANCE{0.05}; // rads
static const PROGMEM float LINEAR_TOLERANCE{10}; // mm
static const PROGMEM float GOAL_CIRCLE_TOLERANCE{5}; // mm
static const PROGMEM float GOAL_ARC_ANGLE_TOLERANCE{3.14}; // rads

// Maze constants
const PROGMEM float MAZE_CELL_SIZE{250.0 * 0.96};
const PROGMEM float LIDAR_WALL_DIST_LIMIT{150}; // mm
const PROGMEM float LIDAR_TARGET_DIST{78}; // mm
const PROGMEM unsigned char FRONT_LIDAR_STOP_TICKS{4}; // ticks
const PROGMEM unsigned char FRONT_LIDAR_STOP_DIST{65}; // mm
const PROGMEM unsigned char FRONT_LIDAR_STOP_TOLERANCE{4}; // mm

// PID Tuning parameters for speed PID controllers for left and right wheels
//const PROGMEM float KP_SPEED{1.5}; // 1.91
const PROGMEM float KP_SPEED{24}; // 1.91
// Double check these values
//const PROGMEM float KI_SPEED{0.19099 / 0.01};
const PROGMEM float KI_SPEED{0.009};
//const PROGMEM float KD_SPEED{0.95492 * 0.01};
//const PROGMEM float KD_SPEED{2 * 0.01};
const PROGMEM float KD_SPEED{0.005};

// PID Tuning parameters for the rotation (IMU) PID controller
//const PROGMEM float KP_ROTATE{6.3};
//const PROGMEM float KI_ROTATE{1.1};
//const PROGMEM float KD_ROTATE{1.8};
const PROGMEM float KP_ROTATE{15};
const PROGMEM float KI_ROTATE{0.0};
const PROGMEM float KD_ROTATE{0.2};

// PID Tuning parameters for the drift PID controller; Need tuning
// const PROGMEM float KP_DRIFT_IMU{6.3};
const PROGMEM float KP_DRIFT_IMU{6.3}; // 0.07
const PROGMEM float KI_DRIFT_IMU{1.1};    // 0
const PROGMEM float KD_DRIFT_IMU{1.8};    // 0

const float KP_DRIFT_LIDAR{0.02}; // 0.07
const float KI_DRIFT_LIDAR{0};    // 0.01
const float KD_DRIFT_LIDAR{0.001};    // 0

// PID Tuning parameters for the forward PID controller (controlling speed based on distance to goal); Need tuning
const float KP_FORWARD{0.5};
const float KI_FORWARD{0.01}; // 0.005
const float KD_FORWARD{0.00}; // 0.01
}

#endif

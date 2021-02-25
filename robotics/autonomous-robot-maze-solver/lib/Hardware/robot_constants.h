#ifndef HARDWARE_ROBOT_CONSTANTS_H_
#define HARDWARE_ROBOT_CONSTANTS_H_

#include <Arduino.h>

const PROGMEM float MAX_RPM{60};

const PROGMEM float MIN_MOTOR_PERCENTAGE{15};          //Minimum percentage before the motors can't push the car
const PROGMEM float MAX_MOTOR_PERCENTAGE{80};          //Limited to 80% to prevent overvolting the motors

const PROGMEM float COUNTS_PER_REV{1400};
const PROGMEM float REVS_PER_COUNT{1/COUNTS_PER_REV};
const PROGMEM float RADS_PER_COUNT{2.0*PI/COUNTS_PER_REV};
const PROGMEM float COUNTS_PER_RAD{1 / RADS_PER_COUNT};

const PROGMEM float WHEEL_RADIUS_MM{42/2.0};           // millimetres
const PROGMEM float WHEEL_SEPARATION{102.0};           // millimetres
const PROGMEM float MAX_WHEEL_VELOCITY{10.0};           // rads/s
const PROGMEM float WHEEL_OFFSET{24.0};  // Wheel offset from center of vehicle

namespace hardware {
namespace locomotion {
/***********************************************/
/***************** misc  ***********************/
/***********************************************/
static const PROGMEM int PWM_MAX = 255;

/***********************************************/
/***************** analog_pin ******************/
/***********************************************/
const PROGMEM float BOARD_VOLTAGE = 5.0;
const PROGMEM float INTERNAL_VOLTAGE = 1.1;

}
}
#endif

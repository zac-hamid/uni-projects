#ifndef HARDWARE_DEFINITION_H
#define HARDWARE_DEFINITION_H
#include <Arduino.h> 
#include "hardware.h"

#include "robot_constants.h"

#include "leo.h"
#include <math.h>


namespace hardware
{
/**
 * \brief The pins namespace contains definition of the functionality of all the
 * pins on Arduino.
 */
namespace pins    // TODO student to define.
{

/*
    List of available Mega 2560 interrupt pins: 2, 3, 18, 19, 20, 21
*/

/**
 * \brief The imu_interrupt is used by imu to signal data ready.
 */
using imu_interrupt = interrupt<digital_pin<2U>>;

/**
 * \brief The in1 is motor drive channel 1 direction
 */
using m1_dir = digital_pin<4U>;

/**
 * \brief The in2 is motor drive channel 1 speed
 */
using m1_enable = digital_pin<5U>;

/**
 * \brief The in3 is motor drive channel 2 direction
 */
using m2_dir = digital_pin<7U>;

/**
 * \brief The in4 is motor drive channel 2 speed
 */
using m2_enable = digital_pin<6U>;

/**
 * \brief The sonar_trigger is ultrasound trigger pin connected to a0.
 */
// @ZAC needs to be changed
using sonar_trigger = analog_pin<digital_pin<40U>>;

/**
 * \brief The ultrasound_echo is ultrasound echo pin connected to a1.
 */
using sonar_echo = analog_pin<digital_pin<40U>>;

// MUST BE INTERUPT PIN!!!!
using left_encoder_a = interrupt<digital_pin<18U>>;
using left_encoder_b = digital_pin<9U>;

// MUST BE INTERUPT PIN!!!!
using right_encoder_a = interrupt<digital_pin<19U>>;
using right_encoder_b = digital_pin<11U>;

using left_lidar_enable = digital_pin<12U>;
using right_lidar_enable = digital_pin<13U>;

using front_lidar_enable = digital_pin<39U>;

constexpr auto left_lidar_address = 0x22;
constexpr auto right_lidar_address = 0x42;
constexpr auto front_lidar_address = 0x76;
}    // namespace pins


using explore_dip = digital_pin<49U>;
using vision_dip = digital_pin<47U>;
using reset_button = digital_pin<45U>;
/**
 * \brief The led is on board led pin
 */
using led = digital_pin<13U>;
using err_led = digital_pin<53U>;
using green_led = digital_pin<52U>;
using traj_cntrl_pb = digital_pin<31U>;

using left_encoder = encoder<pins::left_encoder_a, pins::left_encoder_b>;
using left_wheel_encoder = wheel<pins::left_encoder_a, pins::left_encoder_b>;
using left_wheel = left_wheel_encoder;

using right_encoder = encoder<pins::right_encoder_a, pins::right_encoder_b>;
using right_wheel_encoder = wheel<pins::right_encoder_a, pins::right_encoder_b>;
using right_wheel = right_wheel_encoder;

/**
 * \brief The left_motor bind left motor to the correct pins.
 */
using left_motor = motor<pins::m1_dir, pins::m1_enable>;

/**
 * \brief The right_motor bind right motor to the correct pins.
 */
using right_motor = motor<pins::m2_dir, pins::m2_enable>;

/**
 * \brief The ultrasound is ultrasound sensor template bind to the trigger and
 * echo pin.
 */
using front_sonar = sonar<pins::sonar_trigger, pins::sonar_echo>;

/**
 * \brief This serial is used to communicate with pc via usb.
 */
using serial = serial_api<serial_tag<0u>>;

/**
 * \brief The serial is used to communicate with Bluetooth module.
 */
using bluetooth = serial_api<serial_tag<2u>>;

using left_lidar = lidar<lidar_tag<0>>;

using right_lidar = lidar<lidar_tag<1>>;

using front_lidar = lidar<lidar_tag<2>>;


}    // namespace hardware
#endif // HARDWARE_DEFINITION_H

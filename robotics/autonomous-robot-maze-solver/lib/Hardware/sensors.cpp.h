#ifndef SENSORS_CPP_H_
#define SENSORS_CPP_H_

#include "sensors.h"

#include <Arduino.h>
#include <pins_arduino.h>
#include <limits.h>
#include <Wire.h>

#include "hardware.h"
#include "hardware_definition.h"
#include "units.h"
//#include "locomotion.cpp.h"

// Includes for the IMU
#include "external_MPU6050_6Axis_MotionApps20.h"
#include "external_MPU6050_helper_3dmath.h"

#include "external_VL6180X.h"

namespace hardware {


// Done to enforce the pin number being a uint8_t for passing to digitalPinToInterrupt since it is a macro
#define pin_tToInterrupt(pin) (digitalPinToInterrupt(static_cast<uint8_t>(pin::pin_number)))

/*****************************/
/*** Interrupt functions ****/
/****************************/
template <typename pin>
auto interrupt<pin>::attach_interrupt (void (*callback) (), 
interrupt_mode mode = interrupt_mode::rising) -> void {
    // digitalPinToInterrupt changes a digital pin to a corresponding interrupt number, and checks to make sure the
    // selected pin has interrupt functionality (pins with interrupt functionality vary between different boards)
    switch(mode) {
       case interrupt_mode::change:
            attachInterrupt(pin_tToInterrupt(pin), callback, CHANGE);
            break;
        case interrupt_mode::falling:
            attachInterrupt(pin_tToInterrupt(pin), callback, FALLING);
            break;
        case interrupt_mode::rising:
            attachInterrupt(pin_tToInterrupt(pin), callback, RISING);
            break;
        case interrupt_mode::low:
            attachInterrupt(pin_tToInterrupt(pin), callback, LOW);
            break;
        default:
            break;
    }
}

template <typename pin>
auto interrupt<pin>::detach_interrupt () -> void {
    detachInterrupt(pin_tToInterrupt(pin));
}

// The VL6180X has a default I2C address of 0x29
/*****************************/
/***** LIDAR functions ******/
/***************************/

template<uint8_t id>
class lidar_tag {
    public:
        // Function to provide access to the ID of the LIDAR for determining which LIDAR to perform functions on
        static auto get_id() -> uint8_t {
            return id;
        }
};

template <typename lidar_enable>
void enable_lidar(VL6180X& vl, unsigned int lidar_address) {
    lidar_enable::config_io_mode(io_mode::output);
    // Enable LIDAR for I2C setup
    lidar_enable::write(logic_level::low);
    sleep_for(1000_ms);
    lidar_enable::write(logic_level::high);
    sleep_for(50_ms);
    // Set up LIDAR, set a new I2C address
    vl.init();
    vl.configureDefault();
    vl.setAddress(lidar_address);

    // Set range scale of LIDAR
    vl.setScaling(VL6180X_SCALE_FACTOR);
    // Reduce range max convergence time and ALS integration
    // time to 30 ms and 50 ms, respectively, to allow 10 Hz
    // operation (as suggested by Table 6 ("Interleaved mode
    // limits (10 Hz operation)") in the datasheet).
    vl.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
    vl.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);

    vl.setTimeout(500);

    // stop continuous mode if already active
    vl.stopContinuous();
    // in case stopContinuous() triggered a single-shot
    // measurement, wait for it to complete
    delay(300);
    // start interleaved continuous mode with period of 100 ms

    // THIS NUMBER IS VERY IMPORTANT AND A PAIN IN THE ARSE TO WORK OUT!!!!
    // LOOK AT SECTION 2.4.4 of the VL6180X datasheet for equations (kinda)
    vl.startRangeContinuous(40); // 60
}

template<typename tag>
auto lidar<tag>::enable() -> void {
    // Set SHDN pins on LIDARS to be output
    pins::right_lidar_enable::config_io_mode(io_mode::output);
    pins::front_lidar_enable::config_io_mode(io_mode::output);
    
    switch(tag::get_id()) {
        case 0:
            enable_lidar<pins::left_lidar_enable>(vl_left, 
                    pins::left_lidar_address);

            hardware::bluetooth::print_line("Left LIDAR enabled!");
            break;
        case 1:
            enable_lidar<pins::right_lidar_enable>(vl_right, 
                    pins::right_lidar_address);
            hardware::bluetooth::print_line("Right LIDAR enabled!");
            break;
        case 2:
            enable_lidar<pins::front_lidar_enable>(vl_front,
                    pins::front_lidar_address);
            hardware::bluetooth::print_line("Front LIDAR enabled!");
            break;

        default:
            break;
    }
}


template<typename tag>
auto lidar<tag>::distance() -> units::millimeters {
    uint16_t range = 0;
    switch(tag::get_id()) {
        case 0:
            range = read_lidar_distance(vl_left, "left");
            break;
        case 1:
            // Right lidar code
            range = read_lidar_distance(vl_right, "right");
            break;
        case 2:
            // Front lidar code
            range = read_lidar_distance(vl_front, "front");
            break;
        default:
            break;
    }
    
    return units::millimeters(range);
}


/*****************************/
/***** Sonar functions ******/
/***************************/
template <class trigger_pin, class echo_pin>
auto sonar<trigger_pin, echo_pin>::enable() -> void {
    // Set up direction of pins
    trigger_pin::config_io_mode(io_mode::output);
    echo_pin::config_io_mode(io_mode::input);
    hardware::bluetooth::print_line("Sonar enabled!");
}

template <class trigger_pin, class echo_pin>
auto sonar<trigger_pin, echo_pin>::distance() -> units::millimeters {
    // Clear trigger pin
    trigger_pin::write(logic_level::low);
    sleep_for(2_us);

    // Set trigger pin to HIGH state for 10 microseconds to trigger the 8 pulses
    trigger_pin::write(logic_level::high);
    sleep_for(10_us);
    trigger_pin::write(logic_level::low);

    // Polling approach, returns duration in microseconds
    unsigned long duration = pulseIn(echo_pin::pin_number, HIGH);
    return units::millimeters((double)(duration * 0.034/2) * 10);
}
}

#endif

#ifndef LOCOMOTION_CPP_H_
#define LOCOMOTION_CPP_H_

/* Note this is a cpp.h file for a good reason
 * When you have templated classes, you really need to implement them in a header file
 * Why? Because to generate the code for say digital_pin<13> in your .cpp file, it needs access to ALL the code.
 * You cannot just compile a library and then link to it like you normally would
 */

#include <Arduino.h>
#include <limits.h>
#include <pins_arduino.h>

#include "hardware.h"
#include "robot_constants.h"
#include "units.h"

#include <math_utils.h>


/***********************************************/
/*************** digital_pin *******************/
/***********************************************/
namespace hardware {



template<pin_t pin>
auto digital_pin<pin>::config_io_mode (io_mode mode) -> void {
    switch (mode) {
        case io_mode::input: pinMode(pin, INPUT); break;
        case io_mode::output: pinMode(pin, OUTPUT); break;
        case io_mode::input_pullup: pinMode(pin, INPUT_PULLUP); break;
        case io_mode::unset: 
        default: return;
    }
}

template<pin_t pin>
auto digital_pin<pin>::read () -> logic_level {
    int val = digitalRead(pin);
    return val == HIGH ? logic_level::high : logic_level::low;
}

template<pin_t pin>
auto digital_pin<pin>::write (logic_level level) -> void {
    int val = (level == logic_level::high ? HIGH : LOW);
    digitalWrite(pin, val);
}

template<pin_t pin>
auto digital_pin<pin>::high () -> void {
    write(logic_level::high);
}

template<pin_t pin>
auto digital_pin<pin>::low () -> void {
    write(logic_level::low);
}

template<pin_t pin>
auto digital_pin<pin>::pwm_write (units::percentage duty_cycle) -> void {
    int val = (duty_cycle.count() / 100.0) * locomotion::PWM_MAX;
    analogWrite(pin, val);
}
}

/***********************************************/
/*************** analog_pin ********************/
/***********************************************/
namespace hardware {


template<typename base>
auto analog_pin<base>::set_analog_reference (
        analog_reference ref) -> void {
    ref_ = ref;
}
 
template<typename base>
auto analog_pin<base>::analog_read () -> units::volts {
    // Select the correct reference
    uint8_t mode = locomotion::to_mode(ref_);
    analogReference(mode);
    // First few readings may not be accurate according to https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
    // So lets take a few in case it changed
    analogRead(base::pin_number);
    analogRead(base::pin_number);
    analogRead(base::pin_number);
    analogRead(base::pin_number);

    // Hopefully the transients have settled now...
    int rawVal = analogRead(base::pin_number);
    
    double volts = (rawVal/(double)conversion_unit) * locomotion::to_range(ref_);

    return units::volts(volts);
}

/***********************************************/
/****************** motor **********************/
/***********************************************/
/* Interface for the DFRobot DRI0009 motor driver
 * See https://wiki.dfrobot.com/Arduino_Motor_Shield__L298N___SKU_DRI0009_
 * Note, template parameters are digital_pin's (or similar)
 * First parameter is the direction pin
 * When direction pin is set to low, goes forwards
 * When direction pin is set to high, goes backwards
 * Second parameter is the enable pin which is pulsed to allow for speed control
 */
template <class direction_pin, class enable_pin>
auto motor<direction_pin, enable_pin>::enable() -> void {
    // Why would you not define a bloody interface for your pins if you want to do it this way!!!!!!
    // This will just give confusing errors when you do motor<1,2>();
    //
    // Anyway... we essentially assume that pin_a and pin_b have the
    // same functions as digital_pin and go from there...
    
    // Configure the pins
    direction_pin::config_io_mode(io_mode::output);
    enable_pin::config_io_mode(io_mode::output);

    // Disable the motor (to start with)
    stop();
}

template <class direction_pin, class enable_pin>
auto motor<direction_pin, enable_pin>::stop() -> void {
    // Disable the motor by setting enable to 0
    enable_pin::low();
}

template <class direction_pin, class enable_pin>
auto motor<direction_pin, enable_pin>::forward(units::percentage velocity)
    -> void {
    // Put motor into forwards mode
    direction_pin::high();
    // Duty cycle is % of max velocity
    // Assuming everything is nice and linear
    // Max duty cycle is 255
    
    // Map 0-100 to MIN-MAX
    int duty_cycle = REMAP_RANGE<int>(velocity.count(), 0, 100, MIN_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
    enable_pin::pwm_write(units::percentage{duty_cycle});
}

template <class direction_pin, class enable_pin>
auto motor<direction_pin, enable_pin>::backward(units::percentage velocity)
    -> void {
    // Put motor into backwards mode
    direction_pin::low();
    // Duty cycle is % of max velocity
    // Assuming everything is nice and linear
    int duty_cycle = REMAP_RANGE<int>(velocity.count(), 0, 100, MIN_MOTOR_PERCENTAGE, MAX_MOTOR_PERCENTAGE);
    enable_pin::pwm_write(units::percentage{duty_cycle});
}

/***********************************************/
/****************** encoder ********************/
/***********************************************/

template <typename pin_a, typename pin_b>
auto encoder<pin_a, pin_b>::enable() -> void {
    // TODO: Check if you need pullups
    // Set both pins to input mode
    encoder_pin_a::config_io_mode(io_mode::input);
    encoder_pin_b::config_io_mode(io_mode::input);
    // Attach the interrupt
    // Note we use templating to generate a suitable interrupt
    /*
    encoder_pin_a::attach_interrupt(
            &encoder_callback<encoder_pin_a, encoder_pin_b>, 
            interrupt_mode::change);
            */
    pinMode(encoder_pin_a::pin_number, INPUT);
    attachInterrupt(
            digitalPinToInterrupt((uint8_t)encoder_pin_a::pin_number),
            interrupt_callback,
            CHANGE);
}

template <typename pin_a, typename pin_b>
auto encoder<pin_a, pin_b>::count() -> encoder_count {
    return monitor_.count;
}

template <typename pin_a, typename pin_b>
auto encoder<pin_a, pin_b>::reset() -> void {
    monitor_.count = 0;
}

template <typename pin_a, typename pin_b>
auto encoder<pin_a, pin_b>::get_count() -> long long int {
    return monitor_.count;
}

template <typename pin_a, typename pin_b>
auto encoder<pin_a, pin_b>::interrupt_callback() -> void {
    using int_pin = pin_a;
    
    const hardware::logic_level curr_int_state = int_pin::read();

    const hardware::logic_level last_int_state =
        to_logic_level(monitor_.last_int_state);

    const hardware::logic_level curr_state_b = pin_b::read();

    // Find the current direction
    if (curr_int_state == hardware::logic_level::high &&
            last_int_state == hardware::logic_level::low) {
        if (curr_state_b == hardware::logic_level::low &&
                monitor_.direction == hardware::drive_direction::forward) {
            // Drive has been reversed
            monitor_.direction = hardware::drive_direction::backward;
        } else if (curr_state_b == hardware::logic_level::high &&
                monitor_.direction == hardware::drive_direction::backward) {
            // Drive now forwards
            monitor_.direction = hardware::drive_direction::forward;
        }
    }

    // Update monitor
    monitor_.count += (monitor_.direction == hardware::drive_direction::forward ? 1 : -1);
    monitor_.last_int_state = to_bool(curr_int_state);
}

template <typename pin_a, typename pin_b>
encoder_monitor encoder<pin_a, pin_b>::monitor_;

/***********************************************/
/****************** wheel **********************/
/***********************************************/
template <typename pin_a, typename pin_b>
auto wheel<pin_a, pin_b>::position () -> units::millimeters {
    // Use arc length is R*Theta
    // Theta = counts * converstion factor
    double val = encoder<pin_a, pin_b>::count() * 
        RADS_PER_COUNT * // angle travelled in rads
        WHEEL_RADIUS_MM; // Convert to length

    return units::millimeters{val};
}

} // namespace hardware

#endif //LOCOMOTION_CPP_H_

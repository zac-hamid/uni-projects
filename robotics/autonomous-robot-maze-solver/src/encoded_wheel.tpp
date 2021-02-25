#ifndef ENCODED_WHEEL_TPP_
#define ENCODED_WHEEL_TPP_
#include "encoded_wheel.h"

#include <soft_utils.h>

namespace drive {

template <class wheel, class motor>
EncodedWheel<wheel, motor>::EncodedWheel(bool print_data) :
    motor_speed_controller_{KP_SPEED, KI_SPEED, KD_SPEED, ENCODER_VELOCITY_UPDATE_TIME_MS},
    print_data_{print_data},
    moving_{false},
    output_percent_{0.0f},
    target_vel_{0.0f},
    last_vel_{0.0f},
    prev_counts_{0},
    prev_count_time_ms_{0},
    mutex_{true}
{
    wheel::enable();
    motor::enable();

    mutex_ = false;
}

template <class wheel, class motor>
float EncodedWheel<wheel, motor>::get_desired_velocity() {
    return target_vel_;
}

template <class wheel, class motor>
void EncodedWheel<wheel, motor>::tick() {
    if (!test_and_set(mutex_)) return;

    if (!moving_) {
        return;
    }

    unsigned long curr_time = millis();
    unsigned long time_diff = curr_time - prev_count_time_ms_;

    if (time_diff >= ENCODER_VELOCITY_UPDATE_TIME_MS) {

        // Time to update velocity
                
        // Calculate velocity
        long long int curr_counts = wheel::count();
        long long int count_diff = curr_counts - prev_counts_;
        // Use rads/s for EVERYTHING
        float curr_vel = (count_diff / (double)(time_diff)) * 1000 * RADS_PER_COUNT; 

        prev_count_time_ms_ = millis();
        prev_counts_ = curr_counts;

        auto error = target_vel_ - curr_vel;

        // Decide to stop the motors
        // Stop if zero crossing detectd
        /*
        if (target_vel_ == 0.0 && (curr_vel * last_vel_ < 0)) {
            stop();
        }
        */

        motor_speed_controller_.compute(target_vel_, error, output_percent_);

        last_vel_ = curr_vel;

        if (print_data_) {
            /*
             * Print formatted data for making graphs 
             */

            if (hardware::bluetooth::output_buffer_space() > 25) {
                hardware::bluetooth::print(millis());
                hardware::bluetooth::print(" ");
                hardware::bluetooth::print(target_vel_);
                hardware::bluetooth::print(" ");
                hardware::bluetooth::print(curr_vel);
                hardware::bluetooth::print(" ");
                hardware::bluetooth::print(output_percent_);
                hardware::bluetooth::print("\n");
                //if (output_percent_ < 0) output_percent_ = 0;
            }
        }

        /*
        if (abs(output_percent_) < MIN_MOTOR_PERCENTAGE) {
            output_percent_ = 0;
        }*/
        
        if (output_percent_ > 0) {
            motor::forward(units::percentage{output_percent_});
        } else {
            motor::backward(units::percentage{output_percent_});
        }
    }
    mutex_ = false;
}
template <class wheel, class motor>
void EncodedWheel<wheel, motor>::reset_counter() {
    mutex_ = true;

    wheel::reset();
    prev_counts_ = 0;
    prev_count_time_ms_ = 0;

    mutex_ = false;
}
}
#endif

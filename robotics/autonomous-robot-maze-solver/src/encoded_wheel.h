#ifndef ENCODED_WHEEL_
#define ENCODED_WHEEL_

// #include "units.h"
#include "hardware.h"
//#include "locomotion.cpp.h"
#include "PID.h"
#include "controller_constants.h"
#include "math_utils.h"

namespace drive {

static const float ZERO_VEL_TOLERANCE{RPM_TO_RADS_S(5)};

template <class wheel, class motor>
class EncodedWheel {
    private:
        bool print_data_;
        // long long target_counts_;
        bool moving_;

        float output_percent_;
        // rads / second
        float target_vel_;
        // rads / second
        float last_vel_;

        long long int prev_counts_;
        unsigned long prev_count_time_ms_;

        PID motor_speed_controller_;

        bool mutex_;

    public:
        EncodedWheel() :
            EncodedWheel{false} {}
        EncodedWheel(bool print_data);

        void stop() {
            mutex_ = true;

            moving_ = false;
            last_vel_ = 0.0;
            target_vel_ = 0.0;
            motor::stop();

            mutex_ = false;
        }

        void set_velocity(float rads_second) {
            mutex_ = true;

            target_vel_ = rads_second;
            moving_ = true;
            if (fabsf(target_vel_) > MAX_WHEEL_VELOCITY) {
                target_vel_ = target_vel_ > 0 ? MAX_WHEEL_VELOCITY: -MAX_WHEEL_VELOCITY;
            }

            if (target_vel_ == 0) stop();

            mutex_ = false;
        }

        bool moving() {
            return moving_;
        }

        float get_desired_velocity();

        // Needs to be called routinely!!!
        void tick();

        void reset_counter();

        // long long get_target_counts() {
        //     return target_counts_;
        // }

        void set_tunings(float kp, float ki, float kd) {
            motor_speed_controller_.set_tunings(kp, ki, kd);
        }
    
};

} // namespace drive

#include "encoded_wheel.tpp"

#endif

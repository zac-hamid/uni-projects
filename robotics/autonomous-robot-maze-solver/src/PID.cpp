#include <Arduino.h>
#include "PID.h"

// #define DEBUG

PID::PID(float kp, float ki, float kd, unsigned int sample_time_ms) {
    sample_time_ms_ = sample_time_ms;
    kp_ = kp;
    ki_ = ki * (sample_time_ms_/1000.0);
    kd_ = kd / (sample_time_ms_/1000.0);
    prev_error_ = 0;
    error_sum_ = 0;
    max_output_ = 100.0;
}

// Returns whether or not output was actually updated with a new value
bool PID::compute(float goal, float error, float& output) {

    // MAKE SURE PID IS CALLED EVERY SAMPLE_TIME_MS
    error_sum_ += error;
    if (error_sum_ > 4000) error_sum_ = 4000;
    else if (error_sum_ < -4000) error_sum_ = -4000;

#ifdef DEBUG
    Serial.println("IN PID COMPUTE()");
    Serial.print("error = "); Serial.println(error);
    Serial.print("error_sum = "); Serial.println(error_sum_);

    Serial.print("kp = "); Serial.println(kp_);
    Serial.print("ki = "); Serial.println(ki_);
    Serial.print("kd = "); Serial.println(kd_);


    Serial.print("kp * error = "); Serial.println(kp_ * error);
    Serial.print("ki * error_sum = "); Serial.println(ki_ * error_sum_);
    Serial.print("kd_ * (error - prev_error_) = "); Serial.println(kd_ * (error - prev_error_));
#endif

    output = (kp_ * error) + ki_ * error_sum_ + (kd_ * (error - prev_error_));

#ifdef DEBUG
    Serial.print("pre-clamped output = "); Serial.println(output);
#endif
    // NOW CLAMP THE OUTPUT

    if (output < -1*max_output_) output = -1*max_output_;
    else if (output > max_output_) output = max_output_;

    prev_error_ = error;

    return true;
}

void PID::set_tunings(float kp, float ki, float kd){
    if (kp < 0 || ki < 0 || kd < 0) return;

    kp_ = kp;
    ki_ = ki * (sample_time_ms_/1000.0);
    kd_ = kd / (sample_time_ms_/1000.0);
}

void PID::set_max_output(float max_output) {
    max_output_ = max_output;
}

void PID::reset() {
    prev_error_ = 0;
    error_sum_ = 0;
}

String PID::generate_report(float goal, float output) {
    String str{32};
    str += millis();
    str += " ";
    str += goal;
    str += " ";
    str += prev_error_;
    str += " ";
    str += output;
}

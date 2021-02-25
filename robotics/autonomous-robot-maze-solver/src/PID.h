#ifndef PID_H_
#define PID_H_

#include <Arduino.h>

class PID {
    private:
        float kp_;
        float ki_;
        float kd_;
        float error_sum_;
        float prev_error_;
        unsigned long sample_time_ms_;
        unsigned long prev_time_;
        float max_output_;

    public:
        PID(float kp, float ki, float kd, unsigned int sample_time_ms);

        bool compute(float goal, float error, float& output);
        void set_tunings(float kp, float ki, float kd);
        void set_max_output(float max_output);
        void reset();
        String generate_report(float goal, float output);
};

#endif

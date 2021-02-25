#include <Arduino.h>

#include "odometry.h"
#include "hardware.h"

namespace drive {
    
OdometryMonitor::OdometryMonitor() :
    right_wheel_prev_counts_{0},
    left_wheel_prev_counts_{0},
    current_estimate_{0, 0, 0},
    prev_tick_ms_{0}
{
    // nop
}

void OdometryMonitor::tick() {

    if (millis() - prev_tick_ms_ < ODOM_TICK_PERIOD) return;

    // Get encoder readings
    long long int right_wheel_curr_counts = hardware::right_wheel::count();
    long long int left_wheel_curr_counts = hardware::left_wheel::count();

    // Get difference in encoder readings since last tick()
    long long int right_wheel_count_diff = right_wheel_curr_counts - right_wheel_prev_counts_;
    long long int left_wheel_count_diff = left_wheel_curr_counts - left_wheel_prev_counts_;

    // Get distance travelled by each wheel in mm since the last tick()
    float right_wheel_dist_mm = right_wheel_count_diff * (2 * PI * WHEEL_RADIUS_MM) / COUNTS_PER_REV;
    float left_wheel_dist_mm = left_wheel_count_diff * (2 * PI * WHEEL_RADIUS_MM) / COUNTS_PER_REV;

    // Get the forward distance change and angle change
    float forward_dist = (left_wheel_dist_mm + right_wheel_dist_mm) / 2;
    //float angle_change = atan2(right_wheel_dist_mm - left_wheel_dist_mm, WHEEL_SEPARATION);
    // float angle_change = (1 / WHEEL_SEPARATION) * (right_wheel_dist_mm - left_wheel_dist_mm);

    // Use IMU for heading
    float angle_change = NORMALIZE_THETA(hardware::imu::yaw() - current_estimate_.theta);

    // Update pose based on encoder readings
    current_estimate_.x += forward_dist * cos(current_estimate_.theta + (angle_change/2));
    current_estimate_.y += forward_dist * sin(current_estimate_.theta + (angle_change/2));
    current_estimate_.theta = NORMALIZE_THETA(current_estimate_.theta + angle_change);

    // Update previous counts
    right_wheel_prev_counts_ = right_wheel_curr_counts;
    left_wheel_prev_counts_ = left_wheel_curr_counts;
}

// Want to reset all odometry
// This is called once a new move forward command is given, so that we can start estimating pose again relative to our
// new position
void OdometryMonitor::reset_odometry() {
    current_estimate_.x = 0.0f;
    current_estimate_.y = 0.0f;

    while (!hardware::imu::update());
    current_estimate_.theta = hardware::imu::yaw();

    right_wheel_prev_counts_ = hardware::right_encoder::count();
    left_wheel_prev_counts_ = hardware::left_encoder::count();
    

}
    
} // namespace drive

#ifndef DRIVE_ODOMETRY_H_
#define DRIVE_ODOMETRY_H_

#include "pose.h"
#include "hardware.h"
#include "encoded_wheel.h"
#include "math_utils.h"
#include "hardware.h"

namespace drive {

const int ODOM_TICK_PERIOD = 4; // ms

class OdometryMonitor {
  // use hardware::left_wheel and hardware::right_wheel in here
  private:
      long long int right_wheel_prev_counts_;
      long long int left_wheel_prev_counts_;
      Pose current_estimate_;
      unsigned long int prev_tick_ms_ = 0;
  public:
    OdometryMonitor();
    void tick();
    Pose current_pose() const {return current_estimate_;};

    void reset_odometry();

    // Delete the copy constructor etc, we only want 1 copy (use new)
    OdometryMonitor(const OdometryMonitor&) = delete;
    OdometryMonitor& operator=(const OdometryMonitor&) = delete;
};

} // namespace drive

#endif

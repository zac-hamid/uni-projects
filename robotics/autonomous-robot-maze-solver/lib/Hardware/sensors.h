#ifndef SENSORS_H_
#define SENSORS_H_

// Includes for the IMU
#include "external_MPU6050_6Axis_MotionApps20.h"
#include "external_MPU6050_helper_3dmath.h"

#include "external_VL6180X.h"

#define VL6180X_SCALE_FACTOR 1  // Valid scale factor is 1, 2 or 3, sets range of LIDAR to factor * 20cm, higher scale is less resolution
#define YPR_BUFFER_SIZE 50      // Size of buffer for determining stability of IMU
#define STABILITY_THRESHOLD 0.01    // Threshold for determining stability of IMU

namespace hardware {
    extern VL6180X vl_left;
    extern VL6180X vl_right;
    extern VL6180X vl_front;
    extern MPU6050 mpu;

    uint16_t read_lidar_distance(VL6180X& vl, const char *lidar_name);
}

#endif

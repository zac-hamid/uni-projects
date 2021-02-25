#include "sensors.h"

#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#include "hardware.h"
//#include "sensors.cpp.h"

//#include "comms.cpp.h"

#include "external_MPU6050_6Axis_MotionApps20.h"
#include "external_MPU6050_helper_3dmath.h"

#include "external_VL6180X.h"

namespace hardware {

static const char DMP_FIFO_BUFF_SIZE = 64;

VL6180X vl_left;
VL6180X vl_right;
VL6180X vl_front;
MPU6050 mpu;

static float imu_yaw = 0.0f;
static float imu_pitch = 0.0f;
static float imu_roll = 0.0f;

static float x_accel = 0.0f;
static float y_accel = 0.0f;
static float z_accel = 0.0f;

static float yaw_buffer[YPR_BUFFER_SIZE];
static float pitch_buffer[YPR_BUFFER_SIZE];
static float roll_buffer[YPR_BUFFER_SIZE];
static uint8_t num_readings = 0;
static uint8_t current_reading = 0;

static volatile bool imu_int_called = false;
static float yaw_pitch_roll[3];
static VectorFloat grav;
static VectorInt16 aa, aaReal;
static Quaternion q;

/*
 * Fifo MPU shite
 */
uint16_t dmp_packet_size;
uint8_t dmp_fifo_buffer[DMP_FIFO_BUFF_SIZE];

/*****************************/
/***** I2C functions ********/
/***************************/
auto i2c::enable() -> void {
    Wire.begin();
    //clock(i2c::fast_mode);
    hardware::bluetooth::print_line("I2C Started!");
}

auto i2c::clock(clock_t clock) -> void {
    // Clock speed is based on type of processor present
    Wire.setClock(clock);
}

/*****************************/
/***** IMU functions ********/
/***************************/
// IMU has a default I2C address of 0x68

auto interrupt_update() -> void {
    imu_int_called = true;
}


auto imu::enable() -> bool {
    // Configure interrupt pin
    pins::imu_interrupt::config_io_mode(io_mode::input);
    pins::imu_interrupt::attach_interrupt(&interrupt_update);
    
    // Initialize the IMU and test the connection to see if it's reachable
    mpu.initialize();
    if (!mpu.testConnection()) {
        hardware::bluetooth::print_line("Connection to MPU6050 failed!");
        return false;
    }
    
    // Initialize the DMP module on the IMU and check if it was successful
    uint8_t initialize_status = mpu.dmpInitialize();
    
    if (initialize_status != 0) {
        hardware::bluetooth::print("dmpInitialize() failed with error code: ");
        hardware::bluetooth::print_line(initialize_status);
        return false;
    }
    
    // Enable DMP and reset FIFO buffer
    mpu.setDMPEnabled(true);
    mpu.resetFIFO();
    
    // Offsets of MPU6050 were found through a calibration sketch
    mpu.setXGyroOffset(135);
    mpu.setYGyroOffset(-8);
    mpu.setZGyroOffset(18);
    mpu.setXAccelOffset(-1682);
    mpu.setYAccelOffset(859);
    mpu.setZAccelOffset(1121);

    dmp_packet_size = mpu.dmpGetFIFOPacketSize();
    
    if (mpu.dmpGetFIFOPacketSize() != DMP_FIFO_BUFF_SIZE)  {
        hardware::bluetooth::print_line(F("MPU6050 ERROR DMP_FIFO_BUFF_SIZE WRONG!!!"));
    }
    // RETURN TRUE IF ENABLED SUCCESSFULLY
    hardware::bluetooth::print_line(F("Connected to MPU6050 Successfully!"));
    return true;
}

auto imu::update() -> bool {
    // Check if interrupt was actually called since last update, if not, there is no packets ready
    //if (imu_int_called) {
        auto fifocount = mpu.getFIFOCount();
        if (fifocount < dmp_packet_size) {
            return false; // wait for next interrupt!
        }
        //if (imu_int_called && !mpu.dmpPacketAvailable()) return false;
        if (!mpu.dmpPacketAvailable()) return false;
        imu_int_called = false;
        int mpu_status = mpu.getIntStatus();
        
        // Check to see if the FIFO buffer has overflowed, if it has reset the buffer
        if ((mpu_status & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifocount >= 1024) {
            mpu.resetFIFO();
            hardware::bluetooth::print_line(F("MPU buf ovf!"));
            return false;
        // Check to see if DMP has caused an interrupt (data is ready to be read)
        } else if (mpu_status & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
            unsigned long timeout_us = 5000; // Timeout of 5ms (hopefully enough)
            unsigned long current_us = micros();
            unsigned long prev_us = micros();
            // Timeout SHOULDN'T OCCUR, but just to prevent getting caught in a loop
            while(!mpu.dmpPacketAvailable()) {
                current_us += micros() - prev_us;
                if (current_us >= timeout_us) {
                    hardware::bluetooth::print_line("Couldn't retrieve MPU6050 DMP Packet! Timeout occurred!");
                    return false;
                }
                prev_us = current_us;
            }
            
            
            /*
             * Someone has been naughty and delayed!
             */
            while (fifocount >= dmp_packet_size) {
                mpu.getFIFOBytes(dmp_fifo_buffer, dmp_packet_size);
                fifocount -= dmp_packet_size;
            }
            mpu.resetFIFO();
            
            mpu.dmpGetQuaternion(&q, dmp_fifo_buffer);
            mpu.dmpGetGravity(&grav, &q);
            
            mpu.dmpGetYawPitchRoll(yaw_pitch_roll, &q, &grav);
            
            mpu.dmpGetAccel(&aa, dmp_fifo_buffer);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &grav);
            
            imu_yaw = yaw_pitch_roll[0] * 180/M_PI;
            imu_pitch = yaw_pitch_roll[1] * 180/M_PI;
            imu_roll = yaw_pitch_roll[2] * 180/M_PI;
            
            // store values in cyclic buffer
            yaw_buffer[num_readings % YPR_BUFFER_SIZE] = imu_yaw;
            pitch_buffer[num_readings % YPR_BUFFER_SIZE] = imu_pitch;
            roll_buffer[num_readings % YPR_BUFFER_SIZE] = imu_roll;
            if (num_readings >= YPR_BUFFER_SIZE) {
                current_reading = (num_readings + 1) % YPR_BUFFER_SIZE;
            }

            num_readings++;
            x_accel = aaReal.x;
            y_accel = aaReal.y;
            z_accel = aaReal.z;
            return true;
        }
    //}
    
    return false;
}

auto imu::yaw() -> float {
    return imu_yaw * DEG_TO_RAD;
}

auto imu::pitch () -> float {
    return imu_pitch * DEG_TO_RAD;
}

auto imu::roll () -> float {
    return imu_roll * DEG_TO_RAD;
}

auto imu::stabilize () -> void {
    // YPR should all be within stability threshold, the difference between oldest value in buffer 
    // and most recent value in buffer should be <= STABILITY_THRESHOLD, if not, update imu values
    hardware::bluetooth::print_line("Stabilizing IMU...");
    while((abs(yaw_buffer[num_readings % YPR_BUFFER_SIZE] - yaw_buffer[current_reading]) > STABILITY_THRESHOLD)
        && (abs(pitch_buffer[num_readings % YPR_BUFFER_SIZE] - pitch_buffer[current_reading]) > STABILITY_THRESHOLD)
        && (abs(roll_buffer[num_readings % YPR_BUFFER_SIZE] - roll_buffer[current_reading]) > STABILITY_THRESHOLD)) {
        imu::update();
    }
    hardware::bluetooth::print_line("IMU Stabilized!");
}

uint16_t read_lidar_distance(VL6180X& vl, const char *lidar_name) {
    // Read a single-shot range from LIDAR
    uint16_t range = vl.readRangeContinuousMillimeters();
    if (vl.timeoutOccurred()) {
        hardware::bluetooth::print(F("Timeout occurred on reading from "));
        hardware::bluetooth::print(lidar_name);
        hardware::bluetooth::print_line(F(" LIDAR!"));
    }
    // If unsuccessful, last_status will be non-zero
    if (vl.last_status != 0) {
        hardware::bluetooth::print(F("Unable to read range from left LIDAR! Error code: "));
        hardware::bluetooth::print(F("Unable to read range from "));
        hardware::bluetooth::print(lidar_name);
        hardware::bluetooth::print(F(" LIDAR! Status "));
        hardware::bluetooth::print_line(vl.last_status);
    }

    return range;
}

}

#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "sensors.cpp.h"
#include "hardware_definition.h"

#define MEDIAN_FILT_BUFFER_SIZE 10

// Input-related variables
char input_byte = '\n';
String input = "";
bool cleared = false;
bool input_displayed = false;

// Median filter variables for HC-SR04 Ultrasonic Sensor
double last_sonar_readings[MEDIAN_FILT_BUFFER_SIZE];
uint8_t num_readings = 0;


// Task #2 variables
double sonar_threshold = 40.0; // Threshold of 3cm
double sonar_distance = 50.0; // Hand will be held approx. 5cm away from sonar sensor
unsigned long sonar_time = 2000000; // 2 s, time for hand to be in front of sonar sensor
unsigned long current_time = 0;
unsigned long prev_micros = 0;
bool hand_in_front = false;


// Task #3 variables
bool walls[3]; // boolean array for storing existence of left, front and right walls (respectively)
double walls_threshold = 40.0;
double walls_distance = 60.0;


// Task #4 variables
bool world_walls[4]; // Order is NESW
float world_yaw = 0.0f;
bool first_detection = false;
float yaw_threshold = 10.0f;
unsigned long yaw_wait_time = 3000000;
unsigned long prev_yaw_time = 0;
unsigned long curr_yaw_time = 0;
uint8_t direction = 0;
// Use same walls threshold and distance as task #3

void getInput(void);
void clearInput(void);
double findMedian(double[], uint8_t);

#define MOD(A, B) (((A % B) + B) % B)

void setup() {
    input = "";
    Serial.begin(115200);
    hardware::i2c::enable();
    //hardware::i2c::clock(hardware::i2c::fast_mode);
    hardware::imu::enable();
    hardware::front_sonar::enable();
    hardware::left_lidar::enable();
    hardware::right_lidar::enable();
    
    // initialize median filter buffer
    for (int i = 0; i < MEDIAN_FILT_BUFFER_SIZE; i++) last_sonar_readings[i] = 0.0;
}

void loop() {
    getInput();
    
    // Using a median filter for the sonar readings so want to keep it reading from the sonar since sonar is used for all tasks
    last_sonar_readings[num_readings++ % MEDIAN_FILT_BUFFER_SIZE] = hardware::front_sonar::distance().count();
    
    if (input == "1") {
        if (!input_displayed) {
            Serial.println("Task #1 chosen...");
            Serial.print(" \t\t\t\t\t\t\t\tIMU     \t\t\t\t\t\t\t\t    |  Sonar (mm)   |    LIDAR (mm)  \n");
            Serial.println("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
            Serial.print("  Roll (deg)\t\tPitch (deg)\t\tYaw (deg)\t\tX (mm/s^2)\t\tY (mm/s^2)\t\tZ (mm/s^2) \t\t\t    |  Left   Right \n");
            Serial.println("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
        
            input_displayed = true;
        }
        hardware::imu::update();
        
        Serial.print("     ");
        Serial.print(hardware::imu::roll()); // Roll
        Serial.print("\t\t    ");
        Serial.print(hardware::imu::pitch()); // Pitch]
        Serial.print("\t\t    ");
        Serial.print(hardware::imu::yaw()); // Yaw
        Serial.print("\t\t    ");
        Serial.print(hardware::imu_x_accel()); // X acceleration
        Serial.print("\t\t    ");
        Serial.print(hardware::imu_y_accel()); // Y acceleration
        Serial.print("\t\t    ");
        Serial.print(hardware::imu_z_accel()); // Z acceleration
        Serial.print("\t\t");
        
        Serial.print(findMedian(last_sonar_readings, MEDIAN_FILT_BUFFER_SIZE)); // Sonar
        Serial.print("\t\t");
        
        Serial.print(hardware::left_lidar::distance().count(), 0); // left lidar
        Serial.print("\t");
        Serial.println(hardware::right_lidar::distance().count(), 0); // right lidar
        
    } else if (input == "2") {
        if (!input_displayed) {
            Serial.println("Task #2 chosen...");
            input_displayed = true;
        }
        double median = findMedian(last_sonar_readings, MEDIAN_FILT_BUFFER_SIZE);
        double diff = median - sonar_distance;
        //Serial.print("Distance = ");
        //Serial.println(median);
        if ((diff >= (-1*sonar_threshold)) 
                && (diff <= sonar_threshold)) {
            if (current_time == 0) prev_micros = micros();
            
            current_time += micros() - prev_micros;
            
            if (current_time >= sonar_time) {
                hand_in_front = true;
            }
        } else {
            if (hand_in_front && (diff < (-2*sonar_threshold) || diff > (2*sonar_threshold))) {
                Serial.println("Start!");
                hand_in_front = false;
                current_time = 0;
            }
        }
        prev_micros = micros();
    } else if (input == "3") {
        if (!input_displayed) {
            Serial.println("Task #3 chosen...");
            input_displayed = true;
        }
        unsigned long ll_dist = hardware::left_lidar::distance().count();
        unsigned long rl_dist = hardware::right_lidar::distance().count();
        double sonar_dist = findMedian(last_sonar_readings, MEDIAN_FILT_BUFFER_SIZE);
        
        double left_diff = ll_dist - walls_distance;
        double right_diff = rl_dist - walls_distance;
        double sonar_diff = sonar_dist - walls_distance;
        
        walls[0] = ((left_diff >= (-1*walls_threshold)) && (left_diff <= walls_threshold));
        walls[1] = ((sonar_diff >= (-1*walls_threshold)) && (sonar_diff <= walls_threshold));
        walls[2] = ((right_diff >= (-1*walls_threshold)) && (right_diff <= walls_threshold));
        
        Serial.print(walls[0],BIN);
        Serial.print("\t");
        Serial.print(walls[1],BIN);
        Serial.print("\t");
        Serial.println(walls[2],BIN);
        input = "";
    } else if (input == "4" || first_detection) {
        if (!input_displayed) {
            Serial.println("Task #4 chosen...");
            input_displayed = true;
        }
        
        if (!first_detection) {
            for (int i = 0; i < 4; i++) world_walls[i] = 0;
            
            // Same code to task #3, just detect the walls
            unsigned long ll_dist = hardware::left_lidar::distance().count();
            unsigned long rl_dist = hardware::right_lidar::distance().count();
            double sonar_dist = findMedian(last_sonar_readings, MEDIAN_FILT_BUFFER_SIZE);
        
            double left_diff = ll_dist - walls_distance;
            double right_diff = rl_dist - walls_distance;
            double sonar_diff = sonar_dist - walls_distance;
        
            world_walls[0] = ((sonar_diff >= (-1*walls_threshold)) && (sonar_diff <= walls_threshold));
            world_walls[1] = ((right_diff >= (-1*walls_threshold)) && (right_diff <= walls_threshold));
            world_walls[3] = ((left_diff >= (-1*walls_threshold)) && (left_diff <= walls_threshold));
            
            Serial.print("Initial wall state: ");
            for (int i = 0; i < 4; i++) {
                Serial.print(world_walls[i]);
                Serial.print(" ");
            }
            
            Serial.println("");
            
            first_detection = true;
            
            // Wait until we get our first reading
            while(!hardware::imu::update());
            // After this wait for IMU stability
            hardware::imu::stabilize();
            // Get yaw offset for calculations
            world_yaw = hardware::imu::yaw();
            Serial.print("World yaw: ");
            Serial.println(world_yaw);
        } else {
            hardware::imu::update();
            float yaw_diff = hardware::imu::yaw() - world_yaw;
            Serial.print("Current yaw = ");
            Serial.println(hardware::imu::yaw());
            
            for (int i = -3; i <= 3; i++) {
                if (i != 0) {
                	// Is the change in yaw from initial a multiple of 90 degrees?
                    if (yaw_diff >= (i*90 - yaw_threshold) && yaw_diff <= (i*90 + yaw_threshold)) {
                        // Figure out which way is being faced out of N E S W
                        int facing_index = 0;
                        if (i < 0) {
                            facing_index = 3 - ((-1)*i - 1);
                        } else {
                            facing_index = i;
                        }
                        
                        // If we're facing the same direction as last time
                        if (facing_index == direction) {
                            // Update timer
                            if (curr_yaw_time == 0) prev_yaw_time = micros();
                            curr_yaw_time += micros() - prev_yaw_time;
                            // Have we been facing the same direction for a certain amount of time? If so, scan walls and update wall array
                            if (curr_yaw_time >= yaw_wait_time) {
                                unsigned long ll_dist = hardware::left_lidar::distance().count();
                                unsigned long rl_dist = hardware::right_lidar::distance().count();
                                double sonar_dist = findMedian(last_sonar_readings, MEDIAN_FILT_BUFFER_SIZE);
        
                                double left_diff = ll_dist - walls_distance;
                                double right_diff = rl_dist - walls_distance;
                                double sonar_diff = sonar_dist - walls_distance;
                        
                                world_walls[facing_index] = ((sonar_diff >= (-1*walls_threshold)) && (sonar_diff <= walls_threshold));
                                // MOD macro is used in case we turn in the negative direction
                                world_walls[MOD(facing_index - 1, 4)] = ((left_diff >= (-1*walls_threshold)) && (left_diff <= walls_threshold));
                                world_walls[MOD(facing_index + 1, 4)] = ((right_diff >= (-1*walls_threshold)) && (right_diff <= walls_threshold));
                                
                                Serial.print("Final wall state: ");
                                for (int i = 0; i < 4; i++) {
                                    Serial.print(world_walls[i]);
                                    Serial.print(" ");
                                }
                                
                                Serial.println("");
                                first_detection = false;
                                clearInput();
                            }
                        } else {
                            // Reset timer, update direction
                            curr_yaw_time = 0;
                            direction = facing_index;
                        }
                    }
                }
            }
        }
    } else if (input == "test") {
        if (!input_displayed) {
            Serial.println("Test task chosen...");
            input_displayed = true;
        }
    }
}

// Function to read the input
void getInput() {
    do {
        // Has anything been typed?
        if (Serial.available() > 0) {
            if (!cleared) {
                clearInput();
                cleared = true;
                input_displayed = false;
            }
            input_byte = Serial.read();
            if (input_byte != '\n') input += input_byte;
        }
    } while(input_byte != '\n');
    cleared = false;
}


void clearInput() {
    input = "";
}

// Function to find the median of an array a[] of size size
double findMedian(double a[], uint8_t size) {
    if (size == 0) return a[0];
    
    double temp[size];
    double median;
    
    // Copy array to temp
    for (int i = 0; i < size; i++) temp[i] = a[i];
    
    // BubbleSort algorithm
    for (int i = 0; i < (size - 1); i++) {
        for (int j = 0; j < (size - (i + 1)); j++) {
            if (temp[j] > temp[j+1]) {
                double temp_val = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = temp_val;
            }
        }
    }
    
    // If size is odd
    if (size % 2 != 0) {
        uint8_t x = (size / 2) + 1;
        median = temp[x];
    } else {
        uint8_t x = (size / 2);
        median = (temp[x] + temp[x+1]) / 2;
    }
    return median;
}

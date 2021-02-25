#ifndef EXPLORE_H_
#define EXPLORE_H_

#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "queue.h"
#include "trajectory_controller.h"
#include "maze_layout.h"
#include "comms.h"

#define MEDIAN_FILT_BUFFER_SIZE 10
#define CELL_LENGTH 250
#define RED_LED_PIN 53
#define GREEN_LED_PIN 52

static const int start_hand_wait_time = 3;
static const int lidar_wall_detect_dist = 170;
static const uint8_t rows = 5;
static const uint8_t cols = 9;
static const uint8_t maxDim = (rows < cols) ? cols : rows;
static const uint8_t minDim = (rows < cols) ? rows : cols;

enum orientations {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

enum states {
    WAITING,
    STARTING,
    FIND_OPENING,
    TRAPPED,
    RECALIBRATE,
    FIND_ORIENTATION,
    EXPLORING,
    COMPLETE,
    DO_NOTHING,
    TEST
};

enum drive_cmds {
    MOVE,
    TURN
};

struct Drive_Cmd {
    enum drive_cmds cmd;
    enum orientations final_orientation;
};

class Explore {
private:
    enum orientations initialHeading;//used to record initial heading
    enum orientations orientation;
    uint8_t xPos;
    uint8_t yPos;

    double findMedian(double[], uint8_t);
    bool left_wall_detect();
    bool right_wall_detect();
    bool front_wall_detect();
    double get_l_lidar_dist();
    double get_r_lidar_dist();
    double get_f_lidar_dist();

    //maze class functions
    void transposeMaze();
    void initialiseMaze();
    void addWalls();
    void printMaze();
    void printFinalMaze();
    void generateBox(int cols, int rows);
    void floodFill(int xGoal, int yGoal);

    //vehicle stuff
    drive::TrajectoryController& tc;
    bool is_moving;
    bool is_rotating;
    enum orientations orientation_after_turning;
    void moveForward();
    void turn(int heading);
    
    Queue<Drive_Cmd> driveQueue;

    //hand start parameters
    double start_hand_distance; // Hand will be held approx. 4.5cm away from lidar sensor
    int start_hand_timer;


    //lidar parameters
    unsigned long l_lidar_dist;
    unsigned long r_lidar_dist;
    double l_lidar_readings[MEDIAN_FILT_BUFFER_SIZE];
    double r_lidar_readings[MEDIAN_FILT_BUFFER_SIZE];
    double f_lidar_readings[MEDIAN_FILT_BUFFER_SIZE];

    //vehicle explore state
    enum states state;

    //maze arrays
    int8_t flood[maxDim][maxDim];
    uint8_t vertWalls[maxDim][maxDim + 1];
    uint8_t horWalls[maxDim + 1][maxDim];

    //pose
    bool dimDetermined;
    int xGoal;
    int yGoal;

    bool centreReached;

public:
    Explore(drive::TrajectoryController& tc);
    void tick();
    bool complete();
    hardware::maze_layout parseMaze();

private:
    void yield() {
        tc.tick();
    }
};

#endif

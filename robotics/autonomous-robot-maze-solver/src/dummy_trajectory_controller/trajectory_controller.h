#ifndef DRIVE_TRAJECTORY_CONTROLLER_H_
#define DRIVE_TRAJECTORY_CONTROLLER_H_

#include <Arduino.h>
#include "math_utils.h"
#include "encoded_wheel.h"
#include "units.h"
#include "odometry.h"
#include "pose.h"
#include "hardware.h"
#include "PID.h"
//#include "sensors.cpp.h"
//#include "locomotion.cpp.h"
//#include "comms.cpp.h"

#include "queue.h"

#include "controller_constants.h"


namespace drive {


class TrajectoryController {
  public:
    /*
     * Interface for Explore, planning etc. to interact with
     * The basic idea is you set non-blocking goals, then wait until is_complete() is true
     * You can monitor the progress with get_pose(), but this only uses wheel odometry so be wary
     * tick() MUST be called regularly (at a minimun every 5ms! The more often the better!) or bad shit will happen
     */
    enum Direction {
      LEFT,
      RIGHT
    };

    TrajectoryController();
  
    void tick() {}

    // up to you if you want distance to be cm/m/cells
    void set_forwards_goal(units::millimeters distance) {}

    void set_forwards_goal(units::cells num_cells) {
      set_forwards_goal(units::millimeters{num_cells.count() * MAZE_CELL_SIZE});
    }

    void set_forwards_until_opening() {
      // Not implemented!
      // TODO
    }

    // angle in rads!
    void set_rotate_goal(Direction dir, float angle) {}

    void cancel_all_goals() {}

    bool is_complete() const {return true;};

    //Pose get_pose() const {return odom_.current_pose();};

    void reset() {}
};

} // namespace drive

#endif

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

struct TrajectoryCommand {
  enum Type {
    ROTATE,
    FORWARD
  };

  Type type_;
  float position_[3];
  float velocity_[2];
  float duration_;

  float start_position_[3];
  unsigned long start_time_ms_;
  bool first_tick_ = true;

  TrajectoryCommand(const Type& type, const float& position_x, const float& position_y, const float& position_theta, const float& duration) :
      TrajectoryCommand{type, position_x, position_y, position_theta, 0, 0, 0, duration}
  {
    //nop
  }

  TrajectoryCommand(const Type& type, const float& position_x, const float& position_y, const float& position_theta,
          const float& start_x, const float& start_y, const float& start_theta, const float& duration) :
    type_{type},
    position_{position_x, position_y, position_theta},
    duration_{duration},
    start_position_{start_x, start_y, start_theta},
    start_time_ms_{millis()},
    first_tick_{true}
  {
    //nop
  }

  friend bool operator==(const TrajectoryCommand& lhs, const TrajectoryCommand& rhs) {
    return lhs.type_ == rhs.type_ && lhs.position_[0] == rhs.position_[0] && lhs.duration_ == rhs.duration_ && lhs.start_position_[0] == rhs.start_position_[0]
        && lhs.start_position_[1] == rhs.start_position_[1];
  }

  String msg() {
    String str{32};
    str += "Cmd type: ";
    str += type_;
    str += " to: ";
    str += position_[0];
    str += " ";
    str += position_[1];
    str += " ";
    str += position_[2];
    str += " from ";
    str += start_position_[0];
    str += " ";
    str += start_position_[1];
    str += " ";
    str += start_position_[2];
    str += " duration ";
    str += duration_;
    
    return str;
  }
};

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
  
    void tick();

    // up to you if you want distance to be cm/m/cells
    void set_forwards_goal(units::millimeters distance);

    void set_forwards_goal(units::cells num_cells) {
      set_forwards_goal(units::millimeters{num_cells.count() * MAZE_CELL_SIZE});
    }

    void set_forwards_until_opening() {
      // Not implemented!
      // TODO
    }

    // angle in rads!
    void set_rotate_goal(Direction dir, float angle);

    void cancel_all_goals() {
      left_wheel_.stop();
      right_wheel_.stop();
      rot_executor_.preempt();
      forward_executor_.preempt();
      commands_.reset();
    }

    bool is_complete() const {return commands_.is_empty();};

    Pose get_pose() const {return odom_.current_pose();};

    void reset();

    /*
     * End interface for explore etc.
     */
    void interrupt_cb();
    static void static_interrupt_cb();

    // FOR TESTING ONLY
    void set_velocity_left(float rads_s) {left_wheel_.set_velocity(rads_s);};
    void set_velocity_right(float rads_s) {right_wheel_.set_velocity(rads_s);};

    /*
     * PID trimmers
     */
    void set_rotation_tunings(float kp, float ki, float kd);
    void set_speed_tunings(float kp, float ki, float kd);
    void set_forward_tunings(float kp, float ki, float kd);
    void set_drift_imu_tunings(float kp, float ki, float kd);
    void set_drift_lidar_tunings(float kp, float ki, float kd);

  private:
    OdometryMonitor odom_;
    EncodedWheel<hardware::left_wheel_encoder, hardware::left_motor> left_wheel_;
    EncodedWheel<hardware::right_wheel_encoder, hardware::right_motor> right_wheel_;

    Queue<TrajectoryCommand> commands_;

    Pose last_goal_;

  private:
    class  TrajectoryExecutor {
      private:
        TrajectoryController& controller_;
        unsigned long last_tick_ms_ = 0;
        unsigned int tick_period_;
      protected:
        void set_last_tick(const unsigned long& t) {last_tick_ms_ = t;}

        // Make a cubic trajectory from [0->1]
        // Returns the position at time time
        float cubic_step(const unsigned int& duration_ms, const unsigned int& time_ms) {
            if (time_ms > duration_ms) return 1;
            const float duration = duration_ms / 1000.0;
            const float time = time_ms / 1000.0;
            const float a2 = 3 / (float)(duration * duration);
            const float a3 = -2 / (float) (duration * duration * duration);
            
            return CLAMP<float>(time * time * (a2 + a3 * time), 0.0, 1.0);
        }

        // Make a cubic trajectory from [0->1]
        // Returns the velocity at time time
        float cubic_step_vel(const unsigned int& duration_ms, const unsigned int& time_ms) {
            const float duration = duration_ms / 1000.0;
            const float time = time_ms / 1000.0;
            const float a2 = 3 / (float)(duration * duration);
            const float a3 = -2 / (float) (duration * duration * duration);
            
            return time * (2*a2 + 3*a3 * time);
        }

        Pose current_pose() const {return controller_.get_pose();}

      public:
        TrajectoryExecutor(TrajectoryController& controller, unsigned int tick_period) : 
            controller_{controller}, last_tick_ms_{0}, tick_period_{tick_period} {};
        // Arduino is shite so no virtual destructors for us
        // DO NOT MALLOC IN SUB CLASSES!!!!
        ~TrajectoryExecutor() = default;
        virtual bool tick(TrajectoryCommand&) = 0;
        virtual void preempt() = 0;
        virtual bool active() {return true;};

        bool ready_to_tick() {return millis() - last_tick_ms_ > tick_period_;}

        // Push a command ahead of this one
        void push_front_command(const TrajectoryCommand& command) {
          controller_.push_front_command(command);
        }

        void set_left_wheel_velocity(float vel) {
          controller_.set_velocity_left(vel);
        }

        void set_right_wheel_velocity(float vel) {
          controller_.set_velocity_right(vel);
        }

        void set_wheel_velocity(float vel_l, float vel_r) {
          controller_.set_velocity_left(vel_l);
          controller_.set_velocity_right(vel_r);
        }

        void set_last_goal(const Pose& goal) {
            controller_.set_last_goal(goal);
        }
    };

    class RotationExecutor : public TrajectoryExecutor {
      private:
        PID rot_pid_;
      public:
        RotationExecutor(TrajectoryController& controller, unsigned int tick_period);
        ~RotationExecutor() = default;
        virtual bool tick(TrajectoryCommand&) override;
        virtual void preempt() override;
        void set_rotate_tunings(float p, float i, float d) {rot_pid_.set_tunings(p, i, d);};
    };

    class ForwardExecutor : public TrajectoryExecutor {
      private:
        PID forward_pid_;
        PID drift_pid_;
        unsigned char num_frames_wall_infront_ = 0;
      public:
        ForwardExecutor(TrajectoryController& controller, unsigned int tick_period);
        ~ForwardExecutor() = default;
        virtual bool tick(TrajectoryCommand&) override;
        virtual void preempt() override;
        void set_forward_tunings(float p, float i, float d) {forward_pid_.set_tunings(p, i, d);}
        void set_drift_tunings(float p, float i, float d) {drift_pid_.set_tunings(p, i, d);}
        float calculate_drift();
        bool in_goal_state(const TrajectoryCommand& cmd);
        void calculate_forward_goal(const TrajectoryCommand& cmd, float& goal_dist, float& dist_err);
    };

    class IdleExecutor : public TrajectoryExecutor {
      public:
        IdleExecutor(TrajectoryController& controller, unsigned int tick_period) : TrajectoryExecutor{controller, tick_period} {};
        ~IdleExecutor() = default;
        // Never complete!
        virtual bool tick(TrajectoryCommand&) override {return false;};
        virtual void preempt() override {};
        virtual bool active() override {return false;};
    };

    friend class TrajectoryExecutor;

    RotationExecutor rot_executor_;
    ForwardExecutor forward_executor_;
    IdleExecutor idle_executor_;

  private:
    void push_front_command(const TrajectoryCommand& command) {
      commands_.push_front(command);
    }

    void set_last_goal(const Pose& goal) {
        last_goal_ = goal;
    }
};

extern TrajectoryController traj_controller;

} // namespace drive

#endif

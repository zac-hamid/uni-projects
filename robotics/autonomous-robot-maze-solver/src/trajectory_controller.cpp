#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "trajectory_controller.h"
#include "math_utils.h"
#include "controller_constants.h"

#include <TimerOne.h>

drive::TrajectoryController drive::traj_controller;

inline float line_dist(const float& x1, const float& y1, const float& x2, const float& y2) {
    return sqrtf(sq(x1-x2) + sq(y1-y2));
}

inline float line_dist(const float p1[2], const float p2[2]) {
    return line_dist(p1[0], p1[1], p2[0], p2[1]);
}

namespace drive {

TrajectoryController::TrajectoryController() :
    odom_{},
    last_goal_{0, 0, 0},
    rot_executor_{*this, TRAJ_CONTROLLER_TICK_PERIOD},
    forward_executor_{*this, TRAJ_CONTROLLER_TICK_PERIOD},
    idle_executor_{*this, TRAJ_CONTROLLER_TICK_PERIOD}
{
    hardware::led::config_io_mode(hardware::io_mode::output);
    hardware::led::high();

    hardware::traj_cntrl_pb::config_io_mode(hardware::io_mode::input_pullup);

    //hardware::i2c::enable();
    //hardware::imu::enable();
    //hardware::left_lidar::enable();
    //hardware::right_lidar::enable();

    //while(!hardware::imu::update());
    //hardware::imu::stabilize();

    hardware::led::low();

    hardware::err_led::config_io_mode(hardware::io_mode::output);

    /*
     * Setup interrupt
     */
    Timer1.initialize(100000); // 10ms
    Timer1.attachInterrupt(TrajectoryController::static_interrupt_cb);
}

void TrajectoryController::reset() {
    commands_.reset();
    odom_.reset_odometry();
    last_goal_ = odom_.current_pose();
}

void TrajectoryController::static_interrupt_cb() {
    sei();
    traj_controller.interrupt_cb();
}

void TrajectoryController::interrupt_cb() {
    left_wheel_.tick();
    right_wheel_.tick();
}

void TrajectoryController::tick() {
    hardware::logic_level pb_pressed = hardware::traj_cntrl_pb::read();
    if (hardware::to_bool(pb_pressed)) {

    }


    hardware::err_led::high();
    hardware::imu::update();
    // Update wheels before odometry so it is using the most up to date values
    left_wheel_.tick();
    right_wheel_.tick();

    // Should I reset odometry everytime a move command is called? And rotate command if odometry is implemented?

    odom_.tick();

    if (!commands_.is_empty()) {
        auto& command = commands_.peek();
        bool succ = rot_executor_.tick(command);
        succ |= forward_executor_.tick(command);

        if (succ) {
            left_wheel_.stop();
            right_wheel_.stop();

            commands_.erase(command);
            hardware::bluetooth::print_line("act done");
            //last_goal_ = odom_.current_pose();
        }
    }
    hardware::err_led::low();
}

void TrajectoryController::set_forwards_goal(units::millimeters distance) {
    cancel_all_goals();

    const Pose& curr_pose = get_pose();
    
    float duration = distance.count()*2;

    const float x_dist = distance.count() * cos(last_goal_.theta);
    const float y_dist = distance.count() * sin(last_goal_.theta);

    const float x_target = x_dist + last_goal_.x;
    const float y_target = y_dist + last_goal_.y;
    const float theta_target = last_goal_.theta;

    TrajectoryCommand cmd{TrajectoryCommand::Type::FORWARD, 
            x_target, y_target, theta_target, curr_pose.x, curr_pose.y, curr_pose.theta, duration};

    commands_.push_front(cmd);

    hardware::bluetooth::print_line(cmd.msg().c_str());

    last_goal_ = Pose{x_target, y_target, theta_target};
}

// EVERYTRHING IS RADS
void TrajectoryController::set_rotate_goal(Direction dir, float yaw) {
    const Pose& pose = get_pose();
    float duration = (fabs(yaw) / PI) * 1100; // 2 seconds per 180 degrees
    // Take 1 second
    // Adjust target for starting yaw
    yaw *= (dir == TrajectoryController::Direction::LEFT ? 1 : -1);
    yaw += last_goal_.theta;
    yaw = NORMALIZE_THETA(yaw);
    TrajectoryCommand cmd{TrajectoryCommand::Type::ROTATE, last_goal_.x, last_goal_.y, yaw, 
        pose.x, pose.y, pose.theta, duration};

    cmd.start_position_[0] = last_goal_.theta;

    cancel_all_goals();
    commands_.push_front(cmd);

    hardware::bluetooth::print("Rotate goal: ");
    hardware::bluetooth::print_line(yaw);
    hardware::bluetooth::print("start: ");
    hardware::bluetooth::print_line(cmd.start_position_[2]);
    hardware::bluetooth::print("duration: ");
    hardware::bluetooth::print_line(duration);

    last_goal_ = Pose{last_goal_.x, last_goal_.y, cmd.position_[2]};
}

void TrajectoryController::set_speed_tunings(float p, float i, float d) {
    left_wheel_.set_tunings(p, i, d);
    right_wheel_.set_tunings(p, i, d);
}

void TrajectoryController::set_rotation_tunings(float p, float i, float d) {
    rot_executor_.set_rotate_tunings(p, i, d);
}

void TrajectoryController::set_forward_tunings(float p, float i, float d) {
    forward_executor_.set_forward_tunings(p, i, d);
}

void TrajectoryController::set_drift_lidar_tunings(float p, float i, float d) {
    forward_executor_.set_drift_tunings(p, i, d);
}

/*********************************/
/******** RotationExecutor *******/
/*********************************/

TrajectoryController::RotationExecutor::RotationExecutor(TrajectoryController& controller, unsigned int tick_period) :
    TrajectoryExecutor{controller, tick_period},
    rot_pid_{KP_ROTATE, KI_ROTATE, KD_ROTATE, TRAJ_CONTROLLER_TICK_PERIOD}
{
    // nop
}

bool TrajectoryController::RotationExecutor::tick(TrajectoryCommand& cmd) {
    if (cmd.type_ != TrajectoryCommand::Type::ROTATE) return false;
    if (!ready_to_tick()) return false;

    if (cmd.first_tick_) {
        cmd.start_time_ms_ = millis();
        cmd.first_tick_ = false;
        hardware::bluetooth::print_line(F("first tick on new ROTATE cmd"));
    }

    float range = NORMALIZE_THETA(cmd.position_[2] - cmd.start_position_[2]);
    //hardware::bluetooth::print("range ");
    //hardware::bluetooth::print(range);

    float target_change = (range * cubic_step(cmd.duration_, millis() - cmd.start_time_ms_));
    float target = NORMALIZE_THETA(target_change + cmd.start_position_[2]);
    //hardware::bluetooth::print("target");
    //hardware::bluetooth::print(target);
    float curr_yaw = hardware::imu::yaw();
    //hardware::bluetooth::print("curr_yaw");
    //hardware::bluetooth::print_line(curr_yaw);
    float error = NORMALIZE_THETA(target - curr_yaw);

    float rot_output;
    rot_pid_.compute(target, error, rot_output);

    /*
    if (hardware::bluetooth::output_buffer_space() > 32) {
        hardware::bluetooth::print(millis());
        hardware::bluetooth::print(" ");
        hardware::bluetooth::print(target);
        hardware::bluetooth::print(" ");
        hardware::bluetooth::print(error);
        hardware::bluetooth::print(" ");
        hardware::bluetooth::print(rot_output);
        hardware::bluetooth::print("\n");
    }*/

    // if (hardware::bluetooth::output_buffer_space() > 32) {
    //     hardware::bluetooth::print("heading ");
    //     hardware::bluetooth::print(current_pose().theta);
    // }

    rot_output = CLAMP(rot_output, -MAX_WHEEL_VELOCITY, MAX_WHEEL_VELOCITY);
    set_wheel_velocity(-rot_output, rot_output);

    /*
    float curr_vel = range * cubic_step_vel(cmd.duration_, millis() - cmd.start_time_ms_);
    float left_vel, right_vel;
    RobotModel::inverse_diff_kinematics(0, curr_vel, left_vel, right_vel);
    set_wheel_velocity(left_vel, right_vel);
    */

    auto execution_time = millis() - cmd.start_time_ms_;
    if (execution_time > 1.2 * cmd.duration_) {
        if (fabs(NORMALIZE_THETA(curr_yaw - cmd.position_[2])) < ROTATION_TOLERANCE) {
            return true;
        }
    }

    set_last_tick(millis());
    
    return false;
}

void TrajectoryController::RotationExecutor::preempt() {
    rot_pid_.reset();
}

/**********************************/
/******** Forward Executor ********/
/**********************************/

TrajectoryController::ForwardExecutor::ForwardExecutor(TrajectoryController& controller, unsigned int tick_period) : 
    TrajectoryController::TrajectoryExecutor{controller, tick_period},
    forward_pid_{KP_FORWARD, KI_FORWARD, KD_FORWARD, tick_period},
    drift_pid_{KP_DRIFT_LIDAR, KI_DRIFT_LIDAR, KD_DRIFT_LIDAR, tick_period}
    {
        // Nop
    }

bool TrajectoryController::ForwardExecutor::tick(TrajectoryCommand& cmd) {
    if (cmd.type_ != TrajectoryCommand::Type::FORWARD) return false;
    if (!ready_to_tick()) return false;

    if (cmd.first_tick_) {
        cmd.start_time_ms_ = millis();
        cmd.first_tick_ = false;

        hardware::bluetooth::print_line(F("first tick on new FORWARD cmd"));
    }

    const float x_dist = cmd.position_[0] - cmd.start_position_[0];
    const float y_dist = cmd.position_[1] - cmd.start_position_[1];


    Pose curr_pose = current_pose();

    const float x_err = cmd.position_[0] - curr_pose.x;
    const float y_err = cmd.position_[1] - curr_pose.y;


    //float target_angle = atan2f(y_err, x_err);
    float traj_angle = cmd.position_[2];

    float angle_err = NORMALIZE_THETA(traj_angle - current_pose().theta);

    float dist_left = sqrtf(sq(x_err) + sq(y_err));

    float curr_target_dist, curr_dist_err;
    calculate_forward_goal(cmd, curr_target_dist, curr_dist_err);

    /*
     * If rotated too much, point at the goal!
     * Dont point if too close to the goal -> its entirely noise!
     */
    if (fabs(angle_err) > PI/4 && dist_left > 20) {
        float duration = (fabs(angle_err) / PI) * 1100; // 2 seconds per 180 degrees
        while (!hardware::imu::update());
        // Take 1 second
        // Adjust target for starting yaw
        float yaw = angle_err;
        yaw += hardware::imu::yaw();
        yaw = NORMALIZE_THETA(yaw);
        TrajectoryCommand correction{TrajectoryCommand::Type::ROTATE, curr_pose.x, curr_pose.y, yaw, 
            curr_pose.x, curr_pose.y, curr_pose.theta, duration};

        cmd.start_position_[0] = NORMALIZE_THETA(hardware::imu::yaw());

        push_front_command(correction);

        /*
         * Fix up the duration of the current command!
         */
        cmd.duration_ += duration;
    } else {
        // Decide to move forwards!
        float forward_output, rot_output;
        forward_pid_.compute(curr_target_dist, curr_dist_err, forward_output);
        /*
         * Calculate drift response
         */
        float drift = calculate_drift();
        drift_pid_.compute(0, drift, rot_output);

        // Clamp forward output so drift has priority
        forward_output = CLAMP<float>(forward_output, -MAX_WHEEL_VELOCITY + fabsf(rot_output), MAX_WHEEL_VELOCITY - fabsf(rot_output));

        /*
        if (hardware::bluetooth::output_buffer_space() > 32) {
            hardware::bluetooth::print(F("forward "));
            hardware::bluetooth::print(forward_output);
            hardware::bluetooth::print(F(" drift "));
            hardware::bluetooth::print_line(rot_output);
        }
        */

        set_wheel_velocity(forward_output - rot_output, forward_output + rot_output);
    }

    /*
    if (hardware::bluetooth::output_buffer_space() > 32) {
        hardware::bluetooth::print("r");
        hardware::bluetooth::print_line(curr_target);
        hardware::bluetooth::print("dist_l ");
        hardware::bluetooth::print_line(dist_left);
    }*/

    if (in_goal_state(cmd)) {
        // Point back at the same angle as start
        float duration = (fabs(angle_err) / PI) * 1100; // 2 seconds per 180 degrees
        while (!hardware::imu::update());
        float target_yaw = cmd.position_[2];

        target_yaw = NORMALIZE_THETA(target_yaw);

        const Pose& pose = current_pose();

        TrajectoryCommand correction{TrajectoryCommand::Type::ROTATE, cmd.position_[0], cmd.position_[1], target_yaw, 
            pose.x, pose.y, pose.theta, duration};
        //push_front_command(correction);

        hardware::bluetooth::print_line(F("Set rotation currection!"));

        return true;
    }


    return false;
}

float TrajectoryController::ForwardExecutor::calculate_drift() {
    float left_dist = hardware::left_lidar::distance().count();
    float right_dist = hardware::right_lidar::distance().count();

    bool left_present = left_dist < LIDAR_WALL_DIST_LIMIT;
    bool right_present = right_dist < LIDAR_WALL_DIST_LIMIT;

    if (left_present && right_present) {
        return left_dist - right_dist;
    } else if (left_present) {
        return left_dist - LIDAR_TARGET_DIST;
    } else if (right_present) {
        return LIDAR_TARGET_DIST - right_dist;
    } else {
        return 0;
    }
}

bool TrajectoryController::ForwardExecutor::in_goal_state(const TrajectoryCommand& cmd) {
    /*
     * Two conditions for stopping
     * We essentially accept an arc around the start as the goal state
     * 1: Displacement from start is within some tolerance
     * 2: Angle from start is within some tolerance
     * OR
     * Several continuous ticks with wall in front
     */

    /*
     * Detect wall in front
     * This takes priority over all other measurements
     */
    const int front_lidar_dist = hardware::front_lidar::distance().count();
    if (front_lidar_dist < LIDAR_WALL_DIST_LIMIT) {
        ++num_frames_wall_infront_;
    } else {
        num_frames_wall_infront_ = 0;
    }

    if (num_frames_wall_infront_ > FRONT_LIDAR_STOP_TICKS) {
        if (abs(front_lidar_dist - FRONT_LIDAR_STOP_DIST) <= FRONT_LIDAR_STOP_TOLERANCE) {
            /*
             * We accept this as a good update to our last pose (as we updated with the wall!)
             */
            Pose idealised_curr = current_pose();
            idealised_curr.theta = cmd.position_[2];
            set_last_goal(idealised_curr);
            return true;
        } else {
            return false;
        }
    }

    const Pose& pose = current_pose();
    const float dist_from_start = line_dist(cmd.start_position_[0], cmd.start_position_[1], 
            pose.x, pose.y);

    const float traj_dist = line_dist(cmd.start_position_[0], cmd.start_position_[1], 
            cmd.position_[0], cmd.position_[1]);

    const float dist_left = traj_dist - dist_from_start;

    const float traj_angle = atan2f(cmd.position_[1] - cmd.start_position_[1], cmd.position_[0] - cmd.start_position_[0]);

    const float angle_from_start = atan2f(pose.y - cmd.start_position_[1], pose.x - cmd.start_position_[0]);

    const float angle_diff = NORMALIZE_THETA(traj_angle - angle_from_start);

    if (dist_left < GOAL_CIRCLE_TOLERANCE && angle_diff < GOAL_ARC_ANGLE_TOLERANCE) {
        return true;
    }


    return false;
}

void TrajectoryController::ForwardExecutor::calculate_forward_goal(const TrajectoryCommand& cmd, 
        float& goal_dist, float& dist_err) {

    /*
     * We follow cubic trajectories!
     * goal dist is current distance goal (via cubic traj)
     * dist_err is distance from the robot to this point
     * negative if we have overshot (relative to a ring around start)
     */

    const float range = line_dist(cmd.position_, cmd.start_position_);

    const float cubic_val = cubic_step(cmd.duration_, millis() - cmd.start_time_ms_);

    const float curr_goal_dist = range * cubic_val;

    const float x_dist = cmd.position_[0] - cmd.start_position_[0];
    const float y_dist = cmd.position_[1] - cmd.start_position_[1];
    float goal_angle = atan2f(y_dist, x_dist);
    float curr_target_x = curr_goal_dist * cosf(goal_angle) + cmd.start_position_[0];
    float curr_target_y = curr_goal_dist * sinf(goal_angle) + cmd.start_position_[1];

    const Pose& curr_pose = current_pose();
    const float curr_x_err = curr_target_x - curr_pose.x;
    const float curr_y_err = curr_target_y - curr_pose.y;

    float curr_dist_err = sqrtf(sq(curr_x_err) + sq(curr_y_err));
    const float curr_angle_err = cmd.position_[2] - curr_pose.theta;

    const float curr_angle_err_base_link = NORMALIZE_THETA(curr_angle_err + curr_pose.theta);

    // If the distance is behind us and we have reached the end of the trajectory reverse
    if (line_dist(cmd.start_position_[0], cmd.start_position_[1], curr_pose.x, curr_pose.y) > range) {
        if (PI / 2.0 < curr_angle_err_base_link && curr_angle_err_base_link < -PI / 2.0) {
            curr_dist_err = -curr_dist_err;
        }
    }

    // Write out results
    goal_dist = curr_goal_dist;
    dist_err = curr_dist_err;
}

void TrajectoryController::ForwardExecutor::preempt() {
    forward_pid_.reset();
    drift_pid_.reset();
    num_frames_wall_infront_ = 0;
}


} // namespace drive

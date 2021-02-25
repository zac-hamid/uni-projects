#include <Arduino.h>
#include <pins_arduino.h>
#include "hardware.h"
#include "units.h"
#include "encoded_wheel.h"
#include "robot_constants.h"
#include "trajectory_controller.h"
#include "math_utils.h"
#include "queue.h"
#include "plan.h"
#include "explore.h"

Explore explore = Explore(drive::traj_controller);
hardware::maze_layout maze;
hardware::maze_layout_message msg;
Queue<Move> moves;

/*
 * Setup function
 */ 
// Setup function
void init_hardware() { 
	// hardware::serial::enable(115200);
	hardware::i2c::enable();
	hardware::imu::enable();
	hardware::left_lidar::enable();
	hardware::right_lidar::enable();
	hardware::front_lidar::enable();
	hardware::err_led::config_io_mode(hardware::io_mode::output);
	hardware::green_led::config_io_mode(hardware::io_mode::output);

	while(!hardware::imu::update());	// Wait until we get our first reading
	hardware::imu::stabilize();		// After this wait for IMU stability
	drive::traj_controller.reset();
}

// Setup function
void receive_maze() {
	hardware::bluetooth::print_line(F("===================="));
	hardware::bluetooth::print_line(F("=     MTRN4110     ="));
	hardware::bluetooth::print_line(F("=    EncodeThis    ="));
	hardware::bluetooth::print_line(F("=       ....       ="));
	hardware::bluetooth::print_line(F("=    Vision run    ="));
	hardware::bluetooth::print_line(F("====================\n"));
	hardware::bluetooth::print_line(
            F("... waiting for vision received ..."));
	int i = 0; 
	bool maze_received = false;
	while (!maze_received) {
		hardware::bluetooth::print_line(F("Trying to connect ..."));
		if (hardware::bluetooth::input_byte_in_buffer()) {
			msg = hardware::receive_maze_layout();
			hardware::bluetooth::print_line(F("... maze received ..."));
			if (msg.foundMaze()) {
				maze = hardware::parse_maze_layout(msg); 
				print_maze_serial(maze);
				Plan p{maze};
				moves = p.solve(); 
				hardware::bluetooth::print_line(F("... maze received ..."));
				maze_received = true; 
			} else {
				hardware::bluetooth::print_line(F("Bad read maze :( "));
			}
		}
		delay(100);
	}

    hardware::bluetooth::print(F("Move QUeue length: "));
    hardware::bluetooth::print_line(moves.size());
	//hardware::print_maze_serial(maze);
	delay(2000);
}

/*
 * Explore
 */ 



/*
 * Loop function
 */
void solve_maze() {
	Move curr_move; 
	while (1) {
		if (!moves.is_empty() && drive::traj_controller.is_complete()) {
			curr_move = moves.pop(); 
			switch (curr_move) {
				case FORWARD:
					hardware::bluetooth::print_line(F("Forward"));
					drive::traj_controller.set_forwards_goal(units::millimeters{250});
					hardware::bluetooth::print_line(F("done forward"));
					break;

				case LEFT:
					hardware::bluetooth::print_line(F("left"));
					drive::traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::LEFT, PI / 2.0);
					hardware::bluetooth::print(F("left done"));
					break;

				case RIGHT:
					hardware::bluetooth::print_line(F("right"));
					drive::traj_controller.set_rotate_goal(drive::TrajectoryController::Direction::RIGHT, PI / 2.0);
					hardware::bluetooth::print(F("right done"));
					break;
				
				default:
					hardware::bluetooth::print_line(F("Unknown move whoops :("));
			}
		}
		drive::traj_controller.tick();

		if (moves.is_empty()) {
			hardware::bluetooth::print_line(F("Done!!"));
			return; 
		}

		drive::traj_controller.tick();
	}
}

/*
 * Arduino stuffz
 */ 
void setup() {
	hardware::bluetooth::enable(9600);
	hardware::bluetooth::print_line(F("Finish setup"));
   init_hardware();
   explore_run();
   maze = explore.parseMaze();
   print_maze_serial(maze);
}

void loop() {

}

// turn LEDs on in the centre s


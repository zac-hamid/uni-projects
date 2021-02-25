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

#include "free_mem.h"

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
	hardware::green_led::write(hardware::logic_level::low);
	hardware::err_led::write(hardware::logic_level::low);
   while(!hardware::imu::update());	// Wait until we get our first reading
	hardware::imu::stabilize();		// After this wait for IMU stability
   
   drive::traj_controller.reset();
}

// Setup function
void receive_maze() {
	int i = 0; 
	bool maze_received = false;
	drive::traj_controller.tick();
	while (!maze_received) {
		hardware::bluetooth::print_line(F("Trying to connect ..."));
		if (hardware::bluetooth::input_byte_in_buffer()) {
			msg = hardware::receive_maze_layout();
			//hardware::bluetooth::print_line(F("... maze received ..."));
			if (msg.foundMaze()) {
				maze = hardware::parse_maze_layout(msg); 
				drive::traj_controller.tick();
				print_maze_serial(maze);
				drive::traj_controller.tick();
				Plan p{maze};
				drive::traj_controller.tick();
				moves = p.solve(); 
				hardware::bluetooth::print_line(F("... maze received ..."));
				maze_received = true; 
			} else {
				hardware::bluetooth::print_line(F("Bad read maze :( "));
			}
		}
		drive::traj_controller.tick();
	}

	hardware::bluetooth::print(F("Move Queue length: "));
	hardware::bluetooth::print_line(moves.size());
	//hardware::print_maze_serial(maze);
}

/*
 * Explore
 */
void explore_run() {
   while (!explore.complete()) {
      drive::traj_controller.tick();

      if (!explore.complete()) {
         explore.tick();
      }
   }
}

/*
 * Loop function
 */
void solve_maze() {
	Move curr_move; 
	while (1) {
		if (!moves.is_empty() && drive::traj_controller.is_complete()) {
			curr_move = moves.pop(); 
			drive::traj_controller.tick();
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

		if (moves.is_empty()) {
			hardware::bluetooth::print_line(F("Done!!"));
			return; 
		}

      drive::traj_controller.tick();
	}
}

void wait_for_start_plan() { 
   hardware::bluetooth::print_line(freeRam());
   while(hardware::reset_button::read() != hardware::logic_level::high) {
      drive::traj_controller.tick();
   }
   hardware::bluetooth::print_line("made out");
   unsigned long first_high = millis();
   while(millis() - first_high < 100) {
      if (hardware::reset_button::read() != hardware::logic_level::high)
         first_high = millis(); 
      drive::traj_controller.tick(); //
   }
   hardware::bluetooth::print_line("made out2");
   hardware::bluetooth::print_line(freeRam());
   //drive::traj_controller.reset();
   hardware::bluetooth::print_line("Ready for plan");
}

/*
 * Arduino 
 */ 
void setup() {
   hardware::vision_dip::config_io_mode(hardware::io_mode::input);
   hardware::explore_dip::config_io_mode(hardware::io_mode::input);
   hardware::reset_button::config_io_mode(hardware::io_mode::input);

	hardware::bluetooth::enable(9600);
	hardware::serial::enable(115200);

   init_hardware();

   if (hardware::vision_dip::read() == hardware::logic_level::high) {
      bluetooth::print_line(F("!! VISION !!"));
      receive_maze(); 
		hardware::err_led::write(hardware::logic_level::low);
		hardware::green_led::write(hardware::logic_level::high);
      drive::traj_controller.tick();
      solve_maze();
		hardware::green_led::write(hardware::logic_level::low);
		hardware::err_led::write(hardware::logic_level::high);
   } else {
      bluetooth::print_line(F("!! EXPLORE !!"));
      drive::traj_controller.tick();
		hardware::err_led::write(hardware::logic_level::low);
		hardware::green_led::write(hardware::logic_level::high);
      explore_run();
		hardware::green_led::write(hardware::logic_level::low);
		hardware::err_led::write(hardware::logic_level::high);
      drive::traj_controller.tick();
      maze = explore.parseMaze();
      wait_for_start_plan(); 
		hardware::err_led::write(hardware::logic_level::low);
		hardware::green_led::write(hardware::logic_level::high);
      Plan p{maze};
      moves = p.solve(); 
      drive::traj_controller.tick();
      print_maze_serial(maze);
      drive::traj_controller.tick();
      solve_maze(); 
		hardware::green_led::write(hardware::logic_level::low);
		hardware::err_led::write(hardware::logic_level::high);
   }
}

void loop() {
   drive::traj_controller.tick();
}



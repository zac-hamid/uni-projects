#ifndef ROBOT_INTERFACE_H_
#define ROBOT_INTERFACE_H_

#include <SerialPort.h>
#include <SerialStream.h>
#include <SerialStreamBuf.h>

#include <mutex>

#include "maze.h"

class RobotInterface {
    /*
     * Thread safe robot interface
     * To send messages to robot
     */
    public:
        RobotInterface(std::string serial_port, LibSerial::SerialStreamBuf::BaudRateEnum baud_rate);
        ~RobotInterface() noexcept;
        RobotInterface(const RobotInterface&) = delete;
        RobotInterface& operator=(const RobotInterface&) = delete;

        bool send_maze(Maze maze);
        bool send_string(const std::string& str);
        void test();
        char echo();

    private:
        LibSerial::SerialStream arduino_serial_;
        std::mutex mutex_;
};

#endif

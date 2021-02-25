#include "robot_interface.h"

#include <SerialPort.h>
#include <SerialStream.h>
#include <string>
#include <cstdio>
#include <stdexcept>

#include "common_maze_rep.h"

struct Buffer {
    char data[16] = "abcdefghijklmno";
};

RobotInterface::RobotInterface(std::string serial_port, LibSerial::SerialStreamBuf::BaudRateEnum baud_rate)
{
    arduino_serial_.Open(serial_port);
    arduino_serial_.SetBaudRate(baud_rate);
    arduino_serial_.SetCharSize(LibSerial::SerialStreamBuf::CHAR_SIZE_8);
    arduino_serial_.SetParity(LibSerial::SerialStreamBuf::PARITY_NONE);
    arduino_serial_.SetNumOfStopBits(1);

    if (!arduino_serial_.good()) {
        std::stringstream msg;
        msg << "Error opening robot interface " << serial_port << " with baud " << baud_rate << " try using sudo!";
        throw std::runtime_error(msg.str());
    }

    std::cout << "Opened serial port with " << serial_port << " with baud " << baud_rate << "\n";
}

RobotInterface::~RobotInterface() noexcept {
    arduino_serial_.Close();
}

bool RobotInterface::send_maze(Maze maze) {
    std::lock_guard<std::mutex> lock(mutex_);

    //common::MazeRep maze_rep = static_cast<common::MazeRep>(maze);

    common::MazeRep maze_rep{2, 5};

    maze_rep.hor_[1][2] = 1;

    char msg[1000];
    maze_rep.to_string(msg, 1000);
    std::cout << "Sending maze\n" << msg << "\n";

    uint8_t send_buf[common::BUF_SIZE];

    maze_rep.fill_buff(send_buf);

    for (int i = 0; i < common::BUF_SIZE; i++) {
        printf("Bytes %d 0x%x\n", i, send_buf[i]);
    }

    /*
    char *buf_ptr = (char *)&send_buf[0];
    for (int i = 0; i < common::BUF_SIZE; i++) {
        arduino_serial_.write((char *)send_buf, 1);
        buf_ptr++ ;
        usleep(1);
        std::cout << (int)*buf_ptr << std::hex;
    }
    */

    arduino_serial_.write((char *)send_buf, common::BUF_SIZE);

    arduino_serial_.flush();

    Buffer b;
    arduino_serial_.write((char *)&b, sizeof(Buffer));


    std::cout << "done\n";

    return true;
}

bool RobotInterface::send_string(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex_);

    //std::cout << "sending " << str << "\n";

    arduino_serial_ << str;
    arduino_serial_.flush();

    return true;
}

char RobotInterface::echo() {
    std::lock_guard<std::mutex> lock(mutex_);

    char ret[2];
    arduino_serial_.read(ret, 1);
    return ret[0];
}

void RobotInterface::test() {
    std::lock_guard<std::mutex> lock(mutex_);

    /*
    char buf[5];
    buf[0] = common::TRAPA;
    arduino_serial_.write(buf, 1);
    arduino_serial_ << "hello\n" << std::flush;
    std::string ret;
    arduino_serial_ >> ret;
    std::cout << ret;
    arduino_serial_ >> ret;
    std::cout << ret;
    arduino_serial_ >> ret;
    std::cout << ret;
    */
}

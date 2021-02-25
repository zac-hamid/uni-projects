#include <iostream>

#include "robot_interface.h"
#include "maze.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: ./" << argv[0] << " <serial port>\n";
    }

    /*
    Maze maze{3,4};
    maze.set_horiz_line(1, 1, 0);
    maze.set_vert_line(1, 1, 0);

    std::cout << maze << std::endl;
    */

    std::string port = argv[1];

    RobotInterface robot{port, LibSerial::SerialStreamBuf::BAUD_115200};

    //robot.send_maze(Maze{3, 2});
    robot.send_string("Hello there]\n");

    while(1) {
        //std::cout << robot.echo() << std::flush;
    }
}

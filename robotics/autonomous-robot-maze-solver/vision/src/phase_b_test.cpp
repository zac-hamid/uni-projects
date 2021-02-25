#include <iostream>
#include <thread>
#include <chrono>

#include <opencv2/core/utility.hpp>

#include "maze_view.h"

static const std::string keys = 
    "{help h usage ?    |              | print this message   }"
    "{s serial          |/dev/rfcomm0  | set the serial device}"
    "{i image           | <none>       | set an image to process}"
    "{v video           | <none>       | set a video to process}"
    "{d disable_serial  | <none>       | disable serial comms}";

int main(int argc, char **argv) {
    cv::CommandLineParser cmd_parser(argc, argv, keys);


    if (cmd_parser.has("help")) {
        cmd_parser.printMessage();
        return 0;
    }

    if (!cmd_parser.check()) {
        cmd_parser.printErrors();
        cmd_parser.printMessage();
        return 0;
    }

    std::string serial_port = cmd_parser.get<std::string>("serial");
    std::cout << "Setting serial port to " << serial_port << "\n";

    MazeView maze_view{serial_port};

    if (cmd_parser.has("image")) {
        // Read image from a file
        std::string im_path = cmd_parser.get<std::string>("image");
        std::cout << "Setting input image to " << im_path << "\n";
        maze_view.set_input_image(im_path);
    }

    if (cmd_parser.has("video")) {
        // Read video from a file
        std::string vid_path = cmd_parser.get<std::string>("video");
        std::cout << "Setting input video to " << vid_path << "\n";
        maze_view.set_input_video(vid_path);

    }

    if (cmd_parser.has("disable_serial")) {
        std::cout << "Disabling serial\n";
        maze_view.set_robot_connected(false);
    }

    /*
     * First we send the maze to the arduino
     */
    maze_view.send_maze();

    /*
     * Keep updating trajectory in a loop
     */
    auto update_traj = [&]() {
        maze_view.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    };

    std::thread echo_thread([&](){
            while (1) {
                maze_view.echo_to_robot();
            }
            });
    while (1) {
        update_traj();
    }
}

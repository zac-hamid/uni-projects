#ifndef MAZE_VIEW_H_
#define MAZE_VIEW_H_

#include <memory>
#include <string>
#include <mutex>

#include "aruko_localiser.h"
#include "aruko_generator.h"
#include "line_extractor.h"
#include "robot_interface.h"

const auto BAUD_RATE = LibSerial::SerialStreamBuf::BAUD_9600;
const std::string DEFAULT_BLUETOOTH_DEVICE = "/dev/rfcomm0";
const int CAMERA_ID = 0;

/*
 * Top level interface to interact with robot + maze
 */
class MazeView {
    private:
        const cv::Ptr<cv::aruco::Board> aruco_board_;
        MazeTemplate maze_template_;
        localisation::ArukoLocaliser localiser_;
        RobotInterface robot_interface_;
        LineExtractor line_extractor_;
        std::vector<cv::Point2i> trajectory_;
        std::string trajectory_window_name_ = "trajectory";
        std::string walls_window_name_ = "maze walls";

        bool single_image_mode_ = false;
        std::string input_image_ = "";

        bool robot_connected_ = true;
        int pose_arrow_len_ = 150;
        
        cv::VideoCapture input_video_;

        void set_camera_settings();
        cv::Mat get_frame();
        
    public:
        MazeView();
        MazeView(std::string serial_device);
        ~MazeView() = default;
        MazeView(const MazeView&) = delete;
        MazeView(MazeView&&) = delete;

        bool send_maze();
        void tick();
        void set_input_image(const std::string& image);
        void set_input_video(const std::string& video);
        void set_robot_connected(bool status) {robot_connected_ = status;};

        void echo_to_robot(void);
};
#endif

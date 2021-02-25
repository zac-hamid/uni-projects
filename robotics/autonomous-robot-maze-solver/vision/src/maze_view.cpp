#include "maze_view.h"

#include "maze.h"
#include "camera_parameters.h"

#include <iostream>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

MazeView::MazeView() :
    MazeView{DEFAULT_BLUETOOTH_DEVICE}
{
    // nop
}

MazeView::MazeView(std::string serial_device) :
    aruco_board_{localisation::gen_board()},
    localiser_{aruco_board_},
    robot_interface_{serial_device, BAUD_RATE},
    maze_template_{},
    line_extractor_{maze_template_},
    input_video_{CAMERA_ID},
    single_image_mode_{false}
{
    set_camera_settings();
    if (!input_video_.isOpened()) {
        std::cerr << "Input video is not opened!\n";
    }

    cv::namedWindow(trajectory_window_name_, cv::WINDOW_NORMAL);
    cv::namedWindow(walls_window_name_, cv::WINDOW_NORMAL);
}

void MazeView::set_input_image(const std::string& image) {
    single_image_mode_ = true;
    input_image_ = image;
}

void MazeView::set_input_video(const std::string& image) {
    input_video_ = cv::VideoCapture{image};
}

void MazeView::set_camera_settings() {
    input_video_.set(cv::CAP_PROP_FRAME_WIDTH, 
            camera_constants::camera_width);
    input_video_.set(cv::CAP_PROP_FRAME_HEIGHT, 
            camera_constants::camera_height);
}

bool MazeView::send_maze() {
    cv::Mat image = get_frame();

    cv::Mat lines_image;
    Maze maze = line_extractor_.extract_lines(image, lines_image);

    std::vector<cv::Mat> channels;
    cv::split(image, channels);


    localisation::PoseStamped image_pose;
    if (localiser_.get_pixel_pose(image, image_pose)) {
        std::cout << "Detected robot at " << image_pose << "\n";
        localisation::PoseStamped grid_pose = maze_template_.transform_image_2_grid(image_pose);

        std::cout << "Robot in grid frame " << grid_pose << "\n";
        maze.set_robot_pose(grid_pose);
        /*
         * Remove any wall infront of the robot
         */
        int markers[] = {0, 1, 2, 3};
        auto oriention = maze.get_robot_marker(markers);
        switch(oriention) {
            case 0: 
                maze.set_horiz_line(grid_pose.x + 1, grid_pose.y, false);                break;
            case 1:
                maze.set_vert_line(grid_pose.x, grid_pose.y + 1, false);
                break;
            case 2:
                maze.set_horiz_line(grid_pose.x, grid_pose.y, false);
                break;
            case 3:
                maze.set_vert_line(grid_pose.x, grid_pose.y, false);
                break;
        }
        // Add walls in as red
        channels.at(2) |= maze.draw(image.size());
    }

    cv::Mat processed_image;
    cv::merge(channels, processed_image);
    cv::imshow(walls_window_name_, processed_image);
    std::cout << maze << "\n";
    std::cout << "Encoding: ";
    maze.encode(std::cout);

    cv::waitKey(5);

    if (robot_connected_) {
        /*
        auto send_fun = [&, maze_msg_str] () {robot_interface_.send_string(maze_msg_str);};
        std::thread send_thread{send_fun};
        send_thread.detach();
        */
        std::stringstream encoded_maze;
        maze.encode(encoded_maze);
        return robot_interface_.send_string(encoded_maze.str());
    } else {
        std::cout << "MazeView:send_maze(): robot not connected... not sending message\n";
        return true;
    }
    return true;
}

void MazeView::tick() {
    cv::Mat image = get_frame();
    
    localisation::PoseStamped curr_pose;
    if (localiser_.get_pixel_pose(image, curr_pose)) {

        // Note for cv::Point's we need to have <y, x> because fuckuinbg coord systems
        trajectory_.push_back(cv::Point2f(curr_pose.y, curr_pose.x));

        std::vector<std::vector<cv::Point2i>> poly_lines;
        poly_lines.push_back(trajectory_);

        cv::Mat traj_image;
        image.copyTo(traj_image);
        cv::polylines(traj_image, poly_lines, false, cv::Scalar(0, 255, 0));

        cv::Point2d robot_arrow_end{curr_pose.y + pose_arrow_len_ * sin(curr_pose.yaw), 
            curr_pose.x + pose_arrow_len_ * cos(curr_pose.yaw)};

        cv::arrowedLine(traj_image, cv::Point2i(curr_pose.y, curr_pose.x), robot_arrow_end,
                cv::Scalar(255, 255, 0), 5);

        cv::imshow(trajectory_window_name_, traj_image);
        cv::waitKey(1);
    }
}

cv::Mat MazeView::get_frame() {

    if (single_image_mode_) {
        std::cout << "Reading image from file!\n";
        return cv::imread(input_image_);
    }

    if (!input_video_.isOpened()) {
        std::cerr << "Input video is not opened!\n";
    }

    if (!input_video_.grab()) {
        throw std::runtime_error{"MazeView::get_frame(): no frame available!"};
    }

    cv::Mat image;
    input_video_.retrieve(image);
    return image;
}


void MazeView::echo_to_robot(void) {
    std::string str;
    std::cin >> str;
    robot_interface_.send_string(str);
}

#include "line_extractor.h"
#include "maze_template.h"
#include "camera_parameters.h"
#include "maze.h"

#include <opencv2/opencv.hpp>

#include <iostream>

void process_frame(cv::Mat image) {
    MazeTemplate maze_template;
    LineExtractor line_extracter{maze_template};
    //MazeGenerator maze_extractor;

    cv::Mat undistort_image;

    cv::undistort(image, undistort_image, camera_constants::camera_matrix, camera_constants::distortion_coeffs);
    cv::Mat lines_image;
    Maze maze = line_extracter.extract_lines(image, lines_image, true);

    std::cout << maze << "\n";

    cv::Mat maze_img = maze.draw(image.size());
    cv::imshow("Final maze!", maze_img);
    cv::waitKey(5);


    cv::imshow("input frame", image);
    cv::waitKey(5);
    cv::imshow("undistort frame", undistort_image);
    cv::waitKey(5);
}

int main(int argc, char *argv[]) {

    cv::VideoCapture input_video; // open default video
    //input_video.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 4096);
    //input_video.set(cv::CAP_PROP_FRAME_WIDTH, 2160);
    //input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    input_video.set(cv::CAP_PROP_FRAME_WIDTH, 
            camera_constants::camera_width);
    input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 
            camera_constants::camera_height);

    if (argc == 1) {
        if (!input_video.isOpened()) {
            std::cerr << "Input video is not opened!\n";
            return 1;
        }


        while(input_video.grab()) {
            cv::Mat image;
            input_video.retrieve(image);

            process_frame(image);
        }
    } else if (argc == 2) {
        std::string image_path = argv[1];

        auto image = cv::imread(image_path);
        process_frame(image);

        cv::waitKey(0);
    } else {
        std::string video_path = argv[1];
        input_video = cv::VideoCapture{video_path};

        while(input_video.grab()) {
            cv::Mat image;
            input_video.retrieve(image);

            process_frame(image);
        }
    }
}

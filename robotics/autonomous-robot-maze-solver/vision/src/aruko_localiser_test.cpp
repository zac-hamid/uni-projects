#include "aruko_localiser.h"
#include "aruko_generator.h"
#include "camera_parameters.h"

#include <opencv2/opencv.hpp>

#include <iostream>

void process_frame(cv::Mat image) {
    const auto board = localisation::gen_board();
    auto localiser = localisation::ArukoLocaliser(board);

    cv::imshow("input frame", image);
    cv::waitKey(5);

    localisation::PoseStamped curr_pose;
    localiser.get_pixel_pose(image, curr_pose, true);
    std::cout << curr_pose << "\n";
    cv::waitKey(5);
}

int main(int argc, char **argv) {
    cv::VideoCapture input_video(0); // open default video
    input_video.set(cv::CAP_PROP_FRAME_WIDTH, 
            camera_constants::camera_width);
    input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 
            camera_constants::camera_height);
    //input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 1920);
    //input_video.set(cv::CAP_PROP_FRAME_WIDTH, 1080);
    //input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    //input_video.set(cv::CAP_PROP_FRAME_WIDTH, 1920);

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
    } else {
        std::string image_path = argv[1];

        auto image = cv::imread(image_path);
        process_frame(image);

        cv::waitKey(0);
    }
}

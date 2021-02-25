#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <iostream>

#include "camera_parameters.h"

int main(int argc, char *argv[]) {
    std::string file_name = "capture.jpg";
    if (argc != 1) {
        file_name = argv[1];
    }

    cv::VideoCapture input_video(0); // open default video
    //input_video.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    //input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    input_video.set(cv::CAP_PROP_FRAME_WIDTH, 
            camera_constants::camera_width);
    input_video.set(cv::CAP_PROP_FRAME_HEIGHT, 
            camera_constants::camera_height);

    input_video.grab();
    cv::Mat image;
    input_video.retrieve(image);

    cv::imwrite(file_name, image);
}

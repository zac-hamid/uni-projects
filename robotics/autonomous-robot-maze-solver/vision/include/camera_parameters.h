#ifndef CAMERA_CALIB_H_
#define CAMERA_CALIB_H_

#include <opencv2/opencv.hpp>

#include <iostream>

class CameraCalibration {
  private:
      cv::Mat camera_matrix_;
      cv::Mat dist_matrix_;
  public:
    CameraCalibration(std::string camera_params_file) {
        cv::FileStorage fs{camera_params_file, cv::FileStorage::READ};
        
        fs["cameraMatrix"] >> camera_matrix_;
        fs["dist_coeffs"] >> dist_matrix_;

        std::cout << "/***********************/\n" <<
                     "/*   Init Camera with  */\n" <<
                     "/*   camera matrix     */\n" <<
                     "/* " << camera_matrix_ << " */\n" <<
                     "/*   dist matrix     */\n" <<
                     "/* " << dist_matrix_ << " */\n" <<
                     "/***********************/\n";
    }

    CameraCalibration() :
        CameraCalibration{"camera_params.xml"}
    {
        // nop
    }

    const cv::Mat cameraMatrix() const {
        return camera_matrix_;
    }

    const cv::Mat distMatrix() const {
        return dist_matrix_;
    }

};

namespace camera_constants {
    const cv::Mat camera_matrix = CameraCalibration{}.cameraMatrix();

    const cv::Mat distortion_coeffs = CameraCalibration{}.distMatrix();

    const unsigned int camera_height = 1080;
    const unsigned int camera_width = 1920;
};



#endif

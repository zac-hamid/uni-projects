#ifndef CAMERA_PARAMETTERS_H_
#define CAMERA_PARAMETTERS_H_

#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>

namespace camera_constants {

// To find these values run the interactive calibration tool
// see bin/calibrate*.sh
const double fx = 1529.47;
const double cx = 674.877;
const double fy = 1529.47;
const double cy = 529.731;

const double k1 = 5.8097e-2;
const double k2 = 3.354786e00;
const double k3 = 3.354786e00;
const double p1 = 1;
const double p2 = 1;

const cv::Mat camera_matrix = (cv::Mat_<double>(3,3) <<
        1.5589146113997888e+03, 0., 6.4748582613553151e+02, 0.,
        1.5589146113997888e+03, 4.8297914643832900e+02, 0., 0., 1. );
const cv::Mat distortion_coeffs = (cv::Mat_<double>(1,5) << 
        5.8097577457563637e-02, 1.1448042873886233e+00, 
        3.0320926386776660e-03, -6.4338371561213558e-03, 
        -5.5319331378837093e+00);
};
#endif

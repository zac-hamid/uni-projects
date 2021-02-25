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
    7.5647100902668637e+02, 0., 3.0792525999206339e+02, 0.,
    7.5647100902668637e+02, 2.5369307634515658e+02, 0., 0., 1.);

const cv::Mat distortion_coeffs = (cv::Mat_<double>(1,5) << 
   1.5619567991392685e-01, -2.0390226526886698e-01, 0., 0.,
    -8.0180174396241111e-01);
};
#endif

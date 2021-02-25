#ifndef ARUKO_LOCALISER_H_
#define ARUKO_LOCALISER_H_

#include <string>
#include <iostream>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "pose_stamped.h"

namespace localisation {


class ArukoLocaliser {
  private:
    cv::Ptr<cv::aruco::Board> board_;
    const cv::Mat camera_matrix_;
    const cv::Mat dist_coeffs_;

    localisation::PoseStamped world_2_pixel_coord(cv::Vec3d rvec, cv::Vec3d tvec, const cv::Mat& image);

  public:
    ArukoLocaliser(cv::Ptr<cv::aruco::Board> board);
    bool get_pixel_pose(const cv::Mat& image, PoseStamped& vision_pose, bool visualise = false);
};
}; // namespace localisation

#endif

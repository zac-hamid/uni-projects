#ifndef MAZE_TEMPLATE_H_
#define MAZE_TEMPLATE_H_

#include <vector>
#include <exception>
#include <stdexcept>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "pose_stamped.h"

class MazeTemplate {
  private:
    // walls in y-> direction
    //const std::vector<int> horiz_walls_ = {25, 163, 302, 437, 575, 708};
    const std::vector<int> horiz_walls_ = {38, 239, 450, 656, 859, 1066};

    // walls in x direction
    //const std::vector<int> vert_walls_ = {15, 148, 288, 426, 564, 701, 835, 971, 1116, 1249};
    const std::vector<int> vert_walls_ = {20, 218, 429, 636, 845, 1048, 1257, 1463, 1678, 1875};

  public:
    cv::Mat generate_template(cv::Size size, int thickness = 10) const;
    // Generate a mask for the wall at [row][col]
    cv::Mat generate_vert_mask(int row, int col, cv::Size img_size, int thickness = 10, int trim_dist = 10) const; // ticknes = 10
    cv::Mat generate_horiz_mask(int row, int col, cv::Size img_size, int thickness = 10, int trim_dist = 10) const;
    int num_rows() const;
    int num_cols() const;

    localisation::PoseStamped transform_image_2_grid(const localisation::PoseStamped& image_pose);

};
#endif

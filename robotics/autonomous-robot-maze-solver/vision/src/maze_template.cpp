#include "maze_template.h"

#include <vector>
#include <algorithm>
#include <exception>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

cv::Mat MazeTemplate::generate_template(cv::Size size, int thickness) const {
    int min_y = *std::min_element(vert_walls_.cbegin(), vert_walls_.cend());
    int max_y = *std::max_element(vert_walls_.cbegin(), vert_walls_.cend());
    int min_x = *std::min_element(horiz_walls_.cbegin(), horiz_walls_.cend());
    int max_x = *std::max_element(horiz_walls_.cbegin(), horiz_walls_.cend());

    if (size.width < max_y) {
        std::stringstream msg;
        msg << "MazeTemplate: Image too small to fit all vertical lines!" <<
            "image size: " << size << " max y of template " << max_y;
        throw std::runtime_error{msg.str()};
    }
    if (size.height < max_x) {
        std::stringstream msg;
        msg << "MazeTemplate: Image too small to fit all horizontal lines!" <<
            "image size: " << size << " max x of template " << max_x;
        throw std::runtime_error{msg.str()};
    }

    cv::Mat ret = cv::Mat::zeros(size, CV_8U);

    // create a grid

    for (auto line_x : horiz_walls_) {
        cv::line(ret, cv::Point{min_y, line_x}, cv::Point{max_y, line_x}, 255, thickness);
    }

    for (auto line_y : vert_walls_) {
        cv::line(ret, cv::Point{line_y, min_x}, cv::Point{line_y, max_x}, 255, thickness);
    }
    return ret;
}

cv::Mat MazeTemplate::generate_vert_mask(int row, int col, cv::Size img_size, int thickness, int trim_dist) const {
    cv::Mat mask = cv::Mat::zeros(img_size, CV_8U);

    auto min_x = horiz_walls_.at(row) + trim_dist;
    auto max_x = horiz_walls_.at(row + 1) - trim_dist;
    auto line_y = vert_walls_.at(col);

    cv::line(mask, cv::Point{line_y, min_x}, cv::Point{line_y, max_x}, 255, thickness);
    
    return mask;

}

cv::Mat MazeTemplate::generate_horiz_mask(int row, int col, cv::Size img_size, int thickness, int trim_dist) const {
    cv::Mat mask = cv::Mat::zeros(img_size, CV_8U);

    auto min_y = vert_walls_.at(col) + trim_dist;
    auto max_y = vert_walls_.at(col + 1) - trim_dist;
    auto line_x = horiz_walls_.at(row);

    cv::line(mask, cv::Point{min_y, line_x}, cv::Point{max_y, line_x}, 255, thickness);
    
    return mask;
}

int MazeTemplate::num_rows() const {
    return horiz_walls_.size();
}

int MazeTemplate::num_cols() const {
    return vert_walls_.size();
}

localisation::PoseStamped MazeTemplate::transform_image_2_grid(const localisation::PoseStamped& image_pose) {
    if (image_pose.frame_id != "camera_link") {
        throw std::runtime_error("MazeTemplate::transform_image_2_grid: Unable to transform from " + image_pose.frame_id + " Require camera_link");
    }

    // Note need x goes with vert walls, y with horiz because fucking 2 coord systems in opencv

    localisation::PoseStamped grid_pose;
    grid_pose.frame_id = "grid_frame";

    for (auto vert_it = vert_walls_.begin();
            vert_it != vert_walls_.end() - 1;
            vert_it++) {

        if (*vert_it <= image_pose.y && 
                image_pose.y < *(vert_it + 1)) {
            // found the grid
            grid_pose.y = std::distance(vert_walls_.begin(), vert_it);

            std::cout << "Robot y " << image_pose.y << " Between (" << *vert_it << ", " << *(vert_it + 1) << ")\n";
        }
    }

    // Repeat for the vertical
    for (auto horiz_it = horiz_walls_.begin();
            horiz_it != horiz_walls_.end() - 1;
            horiz_it++) {
        std::cout << "checking horiz wall at " << *horiz_it << "\n";

        if (*horiz_it <= image_pose.x && 
                image_pose.x < *(horiz_it + 1)) {
            // found the grid
            grid_pose.x = std::distance(horiz_walls_.begin(), horiz_it);
            std::cout << "Robot x " << image_pose.x << " Between (" << *horiz_it << ", " << *(horiz_it + 1) << ")\n";
        }
    }

    // No transform for yaw
    grid_pose.yaw = image_pose.yaw;
    grid_pose.z = 0;

    return grid_pose;
}

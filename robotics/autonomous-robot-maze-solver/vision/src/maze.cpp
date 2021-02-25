#include "maze.h"

#include "math_utils.h"

std::ostream& operator<<(std::ostream& os, const Maze& maze) {
    os << "Maze " << maze.rows() << "x" << maze.cols() << "\n";
    for (auto i = 0; i < maze.rows(); i++) {
        for (auto j = 0; j < maze.cols(); j++) {
            if (j == maze.cols() - 1) {
                // Avoid overhaning chars
                os <<  (maze.horiz_walls.at(i).at(j) ? "-" : " ");
            } else {
                os <<  (maze.horiz_walls.at(i).at(j) ? "*--" : "*  ");
            }
        }
        os << "\n";

        // Avoid trailiung line
        if (i != maze.rows() - 1) {
            for (auto j = 0; j < maze.cols(); j++) {

                char middle_marker = ' ';
                if (maze.robot_present_ && maze.robot_pose_.x == i && maze.robot_pose_.y == j) {
                    middle_marker = maze.get_robot_marker();
                }
                os << (maze.vert_walls.at(i).at(j) ? "|" : " ") << middle_marker << " ";
            }

            os << "\n";
        }
    }
    return os;
}

std::ostream& Maze::encode(std::ostream& os) const {
    int height = vert_walls.size() - 1;
    int width = vert_walls[0].size() - 1;
    int markers[4] = {1, 0, 3, 2};
    int robot_orientation = get_robot_marker(markers);
    os << "#";
    os << width;
    os << ".";
    os << height;
    os << ".";
    os << robot_pose_.x * width + robot_pose_.y;
    os << ".";
    os << height / 2 * width + width / 2; // goal is centre
    os << ".";
    os << robot_orientation;
    os << ".";
    os << "H";
    // We strip out the outer walls
    for (auto i = 1; i < rows() - 1; i++) {
        for (auto j = 0; j < cols() - 1; j++) {
            os << (horiz_walls.at(i).at(j) ? "2": "0");
        }
    }

    os << ".V";
    for (auto i = 0; i < rows() - 1; i++) {
        for (auto j = 1; j < cols() - 1; j++) {
            os << (vert_walls.at(i).at(j) ? "2": "0");
        }
    }

    os << "@\n";

    return os;
}

cv::Mat Maze::draw(cv::Size size) {
    cv::Mat maze = cv::Mat::zeros(size, CV_8U);
    MazeTemplate maze_template;
    auto n_rows = maze_template.num_rows();
    auto n_cols = maze_template.num_cols();

    for (int row = 0; row < n_rows - 1; row++) {
        for (int col = 0; col < n_cols; col++) {
            if (vert_walls.at(row).at(col)) {
                cv::Mat line_mask = maze_template.generate_vert_mask(row, col, size);
                maze |= line_mask;
            }
        }
    }

    // Find the vertical lines
    for (int row = 0; row < n_rows; row++) {
        for (int col = 0; col < n_cols - 1; col++) {
            if (horiz_walls.at(row).at(col)) {
                cv::Mat line_mask = maze_template.generate_horiz_mask(row, col, size);
                maze |= line_mask;
            }
        }
    }

    return maze;
}

void Maze::set_robot_pose(localisation::PoseStamped pose) {
    if (pose.frame_id != "grid_frame") {
        throw std::runtime_error("Pose in " + pose.frame_id + " must be in grid_frame");
    }

    robot_present_ = true;
    robot_pose_ = pose;
}

char Maze::get_robot_marker() const {
    char markers[4] = {'V', '>', '^', '<'};
    return get_robot_marker(markers);
}

/*
 * Returns the marker corresponding to the current orientation
 * Where 
 * 0 degrees -> marker[0]
 * 90 degrees -> marker[1]
 * etc.
 */
template <typename T>
T Maze::get_robot_marker(T markers[4]) const {
    double robot_yaw = utils::normalise_angle(robot_pose_.yaw);
    std::vector<double> orientations = {0, M_PI/2.0, M_PI, -M_PI, -M_PI/2.0};

    std::vector<double> errors = orientations;
    std::for_each(errors.begin(), errors.end(), [&] (double& error) {
        error = fabs(error - robot_yaw);
            });

    auto min_error_it = std::min_element(errors.begin(), errors.end());

    auto min_error_index = std::distance(errors.begin(), min_error_it);

    switch(min_error_index) {
        case 0: return markers[0];
        case 1: return markers[1];
        case 2: return markers[2];
        case 3: return markers[2];
        case 4: return markers[3];
    }

    throw std::runtime_error("Failed to find min error (somehow!");

}

/*
bool MazeGenerator::gen(cv::Mat image, Maze& maze) {
    cv::Mat lines_image;
    line_extractor_.extract_lines(image, lines_image);

    cv::Mat lines_image_8UC1;
    lines_image.convertTo(lines_image_8UC1, CV_8UC1);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(lines_image_8UC1, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat contours_image;
    image.copyTo(contours_image);

    cv::drawContours(contours_image, contours, -1, cv::Scalar(255, 0, 0), 5, 8, hierarchy);
    cv::imshow("contours", contours_image);

    return true;
}*/

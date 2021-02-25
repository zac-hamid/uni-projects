#ifndef ARUKO_GENERATOR_H_
#define ARUKO_GENERATOR_H_

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco/dictionary.hpp>

namespace localisation {
static const int num_boards_x = 2;
static const int num_boards_y = 2;
// For old charuco
//static const double square_len = 5/100.0*2;
//static const double marker_len = 5/100.0;
//
//For new GridBoard
static const double marker_len = 5/100.0;
static const double square_seperation = 1;
static const double square_len = marker_len + square_seperation;

static const cv::Vec3d board_dimensions{num_boards_x * square_len, num_boards_y * square_len, 0};

static const auto dict = cv::aruco::DICT_4X4_50;

const cv::Ptr<cv::aruco::Board> gen_board(void);
void save_aruko_board(std::string name);
}; // namespace localisation

#endif

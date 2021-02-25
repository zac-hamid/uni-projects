#include "aruko_generator.h"

#include <string>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace localisation {

const cv::Ptr<cv::aruco::Board> gen_board() {
    const auto dict_ptr = cv::aruco::getPredefinedDictionary(dict);
    //const auto board = cv::aruco::CharucoBoard::create(num_boards_x, num_boards_y, square_len, marker_len, dict_ptr);
    const auto board = cv::aruco::GridBoard::create(num_boards_x, num_boards_y, square_seperation, marker_len, dict_ptr);

    return board;
}

void save_aruko_board(std::string name) {

    const auto dict_ptr = cv::aruco::getPredefinedDictionary(dict);
    //const auto board = cv::aruco::CharucoBoard::create(num_boards_x, num_boards_y, square_len, marker_len, dict_ptr);
    const auto board = cv::aruco::GridBoard::create(num_boards_x, num_boards_y, square_seperation, marker_len, dict_ptr);
    cv::Mat board_image;
    board->draw(cv::Size(600, 500), board_image);

    cv::imwrite(name, board_image);

    /*
     * const auto board_2 = cv::aruco::GridBoard::create(num_boards_x, num_boards_y, marker_len, 0.005, dict_ptr);

    board_2->draw(cv::Size(600, 500), board_image);

    cv::imwrite("grid_board_" + name, board_image);
    */
}

}; //namespace localisation


#ifndef LINE_EXTRACTOR_H_
#define LINE_EXTRACTOR_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "maze.h"


class LineExtractor {
  private:
    const double canny_thresh_1_ = 50;
    const double canny_thresh_2_ = 200;

    // Params for hough transform
    //
    // Control the resolution of the lines position/angle
    const double hough_rho_  = 1;
    const double hough_theta_ = 0.5*CV_PI / 180.0; 

    // Min number of votes for a line
    const int hough_threshold_ = 30; // 65
    const double hough_min_len_ = 60; //30
    // Largest gap in one line before splitting into 2
    const double hough_max_line_gap_ = 20;
    const int hough_line_image_line_width_ = 10;
    const int line_template_width_ = 10;
    const int line_template_trim_len_ = 2*line_template_width_;

    cv::Vec3d rot_adjust_ = {0,0,0}; // Transform from camera rotation to global/maze rotation

    // The acceptable deviation from horizontal/vertical to still be considered a line
    const double angle_tolerance_ = 5.0 * CV_PI / 180.0; // 10 deg

    // Min proportion of a wall black to be considered a wall
    double min_wall_black_proportion_ = 0.4; // 0.4 was working well
    const double min_proportion_line_ = 0.8;

    int wall_pixel_width_ = 8;

    MazeTemplate maze_template_;

    void draw_lines(cv::Mat& image, std::vector<cv::Vec4i> lines, int thickness, cv::Scalar color);

    Maze lines_image_to_maze(const cv::Mat& lines) const;

    bool above_line_threshold(const cv::Mat& lines, const cv::Mat& line_mask, bool vertical_line) const;

    double non_zero_proportion(const cv::Mat& image, const cv::Mat& mast) const;

    double proportion_continuous_line(const cv::Mat& image, const cv::Mat& line_mask, bool vertical_line) const;

    std::string show_wall_proportions(const cv::Mat& lines) const;
    std::string show_wall_len_proportions(const cv::Mat& lines) const;

  public:
    LineExtractor(const MazeTemplate& maze_template) :
        maze_template_{maze_template}
    {
    }

    Maze extract_lines(cv::InputArray image, cv::Mat& lines_image, bool visualise = false);

    // Euler angles
    // Only adjusts by ek (yaw)
    void set_rot_adjust(cv::Vec3d rot);

};

#endif

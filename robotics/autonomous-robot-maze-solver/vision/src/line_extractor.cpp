#include "line_extractor.h"

#include <cassert>
#include <limits>

#include "maze_template.h"

void LineExtractor::set_rot_adjust(cv::Vec3d rot) {
    rot_adjust_ = rot;
}

void LineExtractor::draw_lines(cv::Mat& image, std::vector<cv::Vec4i> lines, int thickness, cv::Scalar color) {
    std::for_each(lines.begin(), lines.end(), [&](cv::Vec4i line) {
            cv::line(image, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, thickness, cv::LINE_8);
        });

}

Maze LineExtractor::extract_lines(cv::InputArray image, cv::Mat& lines_image, bool visualise) {
    // Convert to greyscale
    auto line_template = MazeTemplate{}.generate_template(image.size());
    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);

    
    cv::Mat thresh_image;
    cv::adaptiveThreshold(gray_image, thresh_image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 21, 4); // 11, 2


    
    cv::Mat clean_thresh_image;
    auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(6,6));
    cv::morphologyEx(thresh_image, clean_thresh_image, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 2);
    //cv::erode(thresh_image, clean_thresh_image, cv::getStructuringElement(cv::MORPH_RECT, cv::Size{3,3}), cv::Point(-1, -1), 5); 
    /*
    cv::morphologyEx(thresh_image, clean_thresh_image, cv::MORPH_OPEN, 
            cv::getStructuringElement(cv::MORPH_OPEN, cv::Size(6,6)));
    cv::imshow("clean thresholded image", clean_thresh_image);
    cv::waitKey(1);
    */

    // Run the image through a canny edge detector, then do a Hough line transform
    // Should get decent results with a little tweaking
     cv::Mat edges;
     cv::Canny(gray_image, edges, canny_thresh_1_, canny_thresh_2_);

    // Vector of lines
    // (x0, y0) -> (x1, y1)
    // [0], [1] -> (x0, y0)
    // [2], [3] -> (x1, y1)
    std::vector<cv::Vec4i> lines;

    cv::HoughLinesP(edges,
            lines, hough_rho_, 
            hough_theta_, hough_threshold_, 
            hough_min_len_, hough_max_line_gap_);

    auto raw_lines = lines;
    
    /*
    cv::Mat rot_matrix;
    cv::Rodrigues(rot_adjust_, rot_matrix);
    // Apply rotation adjustment to lines
    std::for_each(lines.begin(), lines.end(), [&](cv::Vec4i& line) {
            cv::Vec3i x1 = {line[0], line[1], 0};
            cv::Vec3i x2 = {line[2], line[3], 0};

            cv::Mat x1_adjusted = rot_matrix * x1;
            cv::Mat x2_adjusted = rot_matrix * x2;

            cv::vconcat(x1_adjusted, x2_adjusted, line);
            });
    */

    // Filter lines to keep only vert/horizontal lines
    lines.erase(std::remove_if(lines.begin(), lines.end(), 
        [&] (const cv::Vec4i& line) {
            double m;
            if (line[2] == line[0]) {
                m = NAN;
                return false;
            } else {
                m = (line[3]-line[1]) / static_cast<double>(line[2] - line[0]);
            }

            double angle = atan(m) + rot_adjust_[2];
            
            bool in_bounds = false;
            for (double base : {0.0, CV_PI/2, -CV_PI/2}) {
                in_bounds |= ((-angle_tolerance_ + base) <= angle) && (angle <= (angle_tolerance_ + base));
            }

            return !in_bounds;
        }), lines.end());

    lines_image = cv::Mat::zeros(image.rows(), image.cols(), CV_8UC1);

    draw_lines(lines_image, lines,  hough_line_image_line_width_, cv::Scalar{255});

    std::cout << "lines image size:" << lines_image.size();

    lines_image &= line_template;

    Maze ret_maze = lines_image_to_maze(lines_image);
    //std::cout << "Using threshold image for lines NOT HOUGH!!!\n";
    //Maze ret_maze = lines_image_to_maze(clean_thresh_image);

    if (visualise) {
        cv::Mat image_copy;
        image.copyTo(image_copy);
        draw_lines(image_copy, lines, wall_pixel_width_, cv::Scalar(0, 0, 255));

        cv::Mat raw_lines_image;
        image.copyTo(raw_lines_image);
        draw_lines(raw_lines_image, raw_lines, wall_pixel_width_, cv::Scalar(0, 0, 255));

        cv::imshow("thresholded image", thresh_image);
        cv::waitKey(1);
        cv::imshow("clean thresholded image", clean_thresh_image);
        cv::waitKey();
        cv::imshow("cleaned up thresh", clean_thresh_image & line_template);

        cv::imshow("Canny edge detection", edges);
        cv::waitKey();
        cv::imshow("line template", line_template);
        cv::waitKey();
        cv::imshow("Raw Hough Lines", raw_lines_image);
        cv::waitKey();
        cv::imshow("Vert + Horiz Hough Lines", image_copy);
        cv::waitKey();
        cv::imshow("Hough lines filtered by template", lines_image & line_template);
        cv::waitKey();
        image.copyTo(image_copy, ~line_template);
        cv::imshow("image filtered by template",  image_copy);
        cv::waitKey();
        cv::imshow("image filtered by template - grey",  gray_image & ~line_template);
        cv::waitKey();
    }

    return ret_maze;
    //return Maze{3,3};
}

Maze LineExtractor::lines_image_to_maze(const cv::Mat& lines) const {

    MazeTemplate maze_template;
    auto n_rows = maze_template.num_rows();
    auto n_cols = maze_template.num_cols();

    Maze maze{n_rows, n_cols};

    int num_detected = 0;

    std::cout << show_wall_proportions(lines) << "\n";
    std::cout << show_wall_len_proportions(lines) << "\n";
    
    // Find the vertical lines
    for (int row = 0; row < n_rows - 1; row++) {
        for (int col = 1; col < n_cols - 1; col++) {
            cv::Mat line_mask = maze_template.generate_vert_mask(row, col, lines.size(), 
                    line_template_width_, line_template_trim_len_);

            bool wall_present = above_line_threshold(lines, line_mask, true);
            maze.set_vert_line(row, col, wall_present);

            num_detected += wall_present;
        }

        std::cout << "\n";
    }
    std::cout << "\n";

    // Find the horiz lines
    for (int row = 1; row < n_rows - 1; row++) {
        for (int col = 0; col < n_cols - 1; col++) {
            cv::Mat line_mask = maze_template.generate_horiz_mask(row, col, lines.size(), 
                    line_template_width_, line_template_trim_len_);

            bool wall_present = above_line_threshold(lines, line_mask, false);
            maze.set_horiz_line(row, col, wall_present);

            num_detected += wall_present;
        }
    }

    std::cout << "Detected " << num_detected << " walls\n";
    return maze;
}

double LineExtractor::proportion_continuous_line(const cv::Mat& image, const cv::Mat& line_mask, bool vertical_line) const {
    int dimension = vertical_line ? 0 : 1;

    cv::Mat num_pixels_row;
    cv::Mat num_pixels_row_template;
    cv::reduce((image & line_mask) != 0, num_pixels_row, dimension, CV_REDUCE_SUM, CV_32S);
    cv::reduce(line_mask != 0, num_pixels_row_template, dimension, CV_REDUCE_SUM, CV_32S);

    double max_pixels_row = *std::max_element(num_pixels_row.begin<double>(), num_pixels_row.end<double>());
    double template_max_pixels_row = *std::max_element(num_pixels_row_template.begin<double>(), num_pixels_row_template.end<double>());

    return max_pixels_row / template_max_pixels_row;
}

bool LineExtractor::above_line_threshold(const cv::Mat& lines, const cv::Mat& line_mask, bool vertical_line) const {

    double proportion_filled = non_zero_proportion(lines, line_mask);
    double line_len_prop = proportion_continuous_line(lines, line_mask, vertical_line);
    return line_len_prop > min_proportion_line_ || proportion_filled > min_wall_black_proportion_;

    //double proportion_filled = non_zero_proportion(lines, line_mask);
    //return  proportion_filled > min_wall_black_proportion_;
}

double LineExtractor::non_zero_proportion(const cv::Mat& image, const cv::Mat& mask) const {
    int num_pixels_filled = cv::countNonZero(image & mask);
    int total_pixels_in_line = cv::countNonZero(mask);
    double proportion_filled = num_pixels_filled / static_cast<double>(total_pixels_in_line);
    return proportion_filled;
}

std::string LineExtractor::show_wall_proportions(const cv::Mat& lines) const {
    int n_cols = maze_template_.num_cols();
    int n_rows = maze_template_.num_rows();
    std::stringstream msg;
    // Find the vertical lines
    msg << "/**** Vertical lines non zero proportion ****/\n";
    for (int row = 0; row < n_rows - 1; row++) {
        for (int col = 0; col < n_cols; col++) {
            cv::Mat line_mask = maze_template_.generate_vert_mask(row, col, lines.size());
            double proportion_filled = non_zero_proportion(lines, line_mask);
            msg << std::fixed << std::setprecision(3) << proportion_filled << " ";
        }

        msg << "\n";
    }
    msg << "\n";

    msg << "/**** Horizontal lines non zero proportion ****/\n";
    // Find the horiz lines
    for (int row = 0; row < n_rows; row++) {
        for (int col = 0; col < n_cols - 1; col++) {
            cv::Mat line_mask = maze_template_.generate_horiz_mask(row, col, lines.size());
            double proportion_filled = non_zero_proportion(lines, line_mask);
            msg << std::fixed << std::setprecision(3) << proportion_filled << " ";
        }
        msg << "\n";
    }
    return msg.str();
}

std::string LineExtractor::show_wall_len_proportions(const cv::Mat& lines) const {
    int n_cols = maze_template_.num_cols();
    int n_rows = maze_template_.num_rows();
    std::stringstream msg;
    // Find the vertical lines
    msg << "/**** Vertical lines line len proportion ****/\n";
    for (int row = 0; row < n_rows - 1; row++) {
        for (int col = 0; col < n_cols; col++) {
            cv::Mat line_mask = maze_template_.generate_vert_mask(row, col, lines.size());
            double proportion = proportion_continuous_line(lines, line_mask, true);
            msg << std::fixed << std::setprecision(3) << proportion << " ";
        }

        msg << "\n";
    }
    msg << "\n";

    msg << "/**** Horizontal lines line proportion ****/\n";
    // Find the horiz lines
    for (int row = 0; row < n_rows; row++) {
        for (int col = 0; col < n_cols - 1; col++) {
            cv::Mat line_mask = maze_template_.generate_horiz_mask(row, col, lines.size());
            double proportion = proportion_continuous_line(lines, line_mask, false);
            msg << std::fixed << std::setprecision(3) << proportion << " ";
        }
        msg << "\n";
    }
    return msg.str();
}

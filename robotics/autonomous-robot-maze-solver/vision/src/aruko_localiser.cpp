#include "aruko_localiser.h"

#include <vector>
#include <iostream>

#include "aruko_generator.h"
#include "camera_parameters.h"

namespace localisation {
    ArukoLocaliser::ArukoLocaliser(cv::Ptr<cv::aruco::Board> board) :
        board_{board},
        camera_matrix_{camera_constants::camera_matrix},
        dist_coeffs_{camera_constants::distortion_coeffs}
    {
        // nop
    }

    
    bool ArukoLocaliser::get_pixel_pose(const cv::Mat& image, PoseStamped& vision_pose, bool visualise) {

        std::vector<std::vector<cv::Point2f>> marker_corners;
        std::vector<int> marker_ids;

        cv::aruco::detectMarkers(image, board_->dictionary, marker_corners, marker_ids);

        cv::Vec3d board_rot, board_transl;
        if (marker_ids.size() > 0) {
            int valid = cv::aruco::estimatePoseBoard(marker_corners, 
                    marker_ids,
                    board_,
                    camera_matrix_,
                    dist_coeffs_,
                    board_rot,
                    board_transl);

            if (!valid) {
                return false;
            }

            // Shift board transl to be centre of the paper
            cv::Mat rot_matrix;
            cv::Rodrigues(board_rot, rot_matrix);

            auto centre_offset = board_rot * board_dimensions;
            board_transl = board_transl + (centre_offset / 2);

            //std::cout << "centre offset" << centre_offset << "\n";
            //std::cout << "board rot: " << board_rot << "\n";
            //std::cout << "board transl: " << board_transl << "\n";

            vision_pose = world_2_pixel_coord(board_rot, board_transl, image);
             /*
            std::tie(vision_pose.x, vision_pose.y) = 
                std::tie(pixel_coords[0], pixel_coords[1]);

            vision_pose.z = 0;

            vision_pose.yaw = board_rot[2]; // about z axis
            */

        } else {
            return false;
        }

        if (visualise) {
            cv::Mat image_copy{image};
            cv::aruco::drawAxis(image_copy, camera_matrix_, dist_coeffs_, board_rot, board_transl, 0.1);

            cv::imshow("aruko_localiser: get_pixel_pose", image_copy);
        }
        return true;
    }

    localisation::PoseStamped ArukoLocaliser::world_2_pixel_coord(cv::Vec3d rvec, cv::Vec3d tvec, const cv::Mat& image) {
        // We project the 3d point onto the image plane
        std::vector<cv::Point3f> axis_points;
        float length = 0.1;
        axis_points.push_back(cv::Point3f(0, 0, 0));
        axis_points.push_back(cv::Point3f(length, 0, 0));
        axis_points.push_back(cv::Point3f(0, length, 0));
        axis_points.push_back(cv::Point3f(0, 0, length));


        std::vector<cv::Point2f> image_points;
        cv::projectPoints(axis_points, rvec, tvec, camera_constants::camera_matrix, camera_constants::distortion_coeffs, image_points);

        cv::Vec2f pos = image_points.at(0);

        cv::Vec2f forward_x = image_points.at(1);

        
        localisation::PoseStamped image_pose;

        double yaw = atan2(pos[0] - forward_x[0], pos[1] - forward_x[1]);
        //image_pose.yaw = rvec[2];
        image_pose.yaw = yaw;

        image_pose.frame_id = "camera_link";

        // Note we need to swap coords because of OpenCV's different coord systems!
        std::tie(image_pose.x, image_pose.y) = 
            std::tie(pos[1], pos[0]);

        image_pose.z = 0;

        /*
        cv::Mat disp;
        image.copyTo(disp);

            // draw axis lines
        line(disp, image_points[0], image_points[1], cv::Scalar(0, 0, 255), 3);
        line(disp, image_points[0], image_points[2], cv::Scalar(0, 255, 0), 3);
        line(disp, image_points[0], image_points[3], cv::Scalar(255, 0, 0), 3);
        cv::imshow("disp", disp);
        cv::waitKey(1);
        */
        
        return image_pose;
        
    }

}; // namespace localisation


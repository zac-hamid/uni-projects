#ifndef MAZE_H_
#define MAZE_H_

#include <iostream>
#include <vector>
#include <cassert>

#include "common_maze_rep.h"
#include "maze_template.h"
#include "pose_stamped.h"

class Maze {
  private:
    bool robot_present_ = false;

  public:
    std::vector<std::vector<bool>> vert_walls;
    std::vector<std::vector<bool>> horiz_walls;
    localisation::PoseStamped robot_pose_;

    Maze(const size_t& rows, const size_t& cols) :
        // Initialise all to true
        vert_walls{rows, std::vector<bool>(cols, true)},
        horiz_walls{rows, std::vector<bool>(cols, true)}
    {
        assert(rows > 0 && cols > 0);
    }

    size_t rows() const {
        return vert_walls.size();
    }

    size_t cols() const {
        return horiz_walls[0].size();
    }

    explicit operator common::MazeRep() const {
        if (rows() > common::HEIGHT || cols() > common::WIDTH) {
            throw "Maze too large to represent as common maze";
        }

        common::MazeRep maze;
        maze.width_ = cols();
        maze.width_ = rows();

        for (int i = 0; i < horiz_walls.size(); i++) {
            for (int j = 0; j < horiz_walls[i].size(); j++) {
                maze.hor_[i][j] = horiz_walls[i][j];
            }
        }

        for (int i = 0; i < vert_walls.size(); i++) {
            for (int j = 0; j < vert_walls[i].size(); j++) {
                maze.ver_[i][j] = vert_walls[i][j];
            }
        }
        return maze;
    }

    void set_vert_line(int row, int col, bool value) {
        vert_walls.at(row).at(col) = value;
    }

    void set_horiz_line(int row, int col, bool value) {
        horiz_walls.at(row).at(col) = value;
    }

    void set_robot_pose(localisation::PoseStamped pose);

    cv::Mat draw(cv::Size size);

    friend std::ostream& operator<<(std::ostream& os, const Maze& maze);

    std::ostream& encode(std::ostream& os) const;

    char get_robot_marker() const;
    /*
     * Returns the marker corresponding to the current orientation
     * Where 
     * 0 degrees -> marker[0]
     * 90 degrees -> marker[1]
     * etc.
     */
    template <typename T>
     T get_robot_marker(T markers[4]) const;
};

/*
class MazeGenerator {
  private:
    LineExtractor line_extractor_;

  public:
    bool gen(cv::Mat image, Maze& maze);
};*/
#endif

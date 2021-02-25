#ifndef MAZE_LAYOUT_H_
#define MAZE_LAYOUT_H_

#include "hardware.h"

#define WIDTH       9
#define HEIGHT      5
#define MAX_DIM     9

#define NO_WALL     0x00
#define WALL        0x01
#define UNKNOWN     0x02

enum Direction {
    N, // 
    E, 
    S, 
    W,
    NO_DIR
}; 

enum Move {
    FORWARD,
    LEFT, 
    RIGHT,
    NO_MOVE
};

namespace hardware {

class maze_layout_message {
    private:
			uint8_t size_; 
      bool maze_found_; 
			char msg_[255];

    public:
      maze_layout_message() = default; 
      maze_layout_message(uint8_t size, bool maze_found, char msg[]) : 
        size_{size}, maze_found_{maze_found} {
				for (int i = 0; i < size; ++i) 
					msg_[i] = msg[i];
			}	

      maze_layout_message& operator=(const maze_layout_message& other) {
        size_ = other.size_;
        maze_found_ = other.maze_found_;
        for (int i = 0 ; i != size_; ++i)
          msg_[i] = other.msg_[i];
        return *this;
      }

      maze_layout_message(uint8_t size) : size_{size} {}
			char * getMsg() { return msg_; }
      bool foundMaze() { return maze_found_; }
};

class cell_location {
  public:
    cell_location() : cell_location{0, 0, NO_DIR} {}
    cell_location(uint8_t x, uint8_t y, Direction dir=N) : x_{x}, y_{y}, dir_{dir} {}
    cell_location(const cell_location& cell) {
      x_ = cell.x_; 
      y_ = cell.y_; 
      dir_ = cell.dir_; 
    }

    uint8_t getX() { return x_; }
    void setX(uint8_t x) { x_ = x; }
    uint8_t getY() { return y_; }
    void setY(uint8_t y) { y_ = y; }
    Direction getDir() { return dir_; }
    void setDir(Direction dir) { dir_ = dir; }

    friend bool operator==(const cell_location& c1, const cell_location& c2) {
      return (c1.x_ == c2.x_ && c1.y_ == c2.y_ && c1.dir_ == c2.dir_);
    }

    friend bool operator!=(const cell_location& c1, const cell_location& c2) {
      return !(c1 == c2);
    }

    friend cell_location operator+(const cell_location& cell, Move move);

  private:
    uint8_t x_, y_; 
    Direction dir_; 
};

class maze_layout { 
  public:
    // Default constructor
    maze_layout() = default;

    // Proper constructor 
    maze_layout(uint8_t width, uint8_t height, uint8_t robot, uint8_t dest, Direction dir=NO_DIR) : 
      width_{width}, height_{height}, 
      robot_{static_cast<uint8_t>(floor(robot/width)), 
            static_cast<uint8_t>(robot%width), 
            dir},
      start_{static_cast<uint8_t>(floor(robot/width)), 
            static_cast<uint8_t>(robot%width), 
            dir}, 
      dest_{static_cast<uint8_t>(floor(dest/width)),
            static_cast<uint8_t>(floor(dest%width))},
      hor_{}, ver_{}, cell_{} 
    {
      
      for (uint8_t x = 0; x <= height; ++x) {
        for (uint8_t y = 0; y <= width; ++y) {
          if (y != width_) {
            if (x == 0 || x == height_) {
              hor_[x][y] = WALL;
            } else {
              hor_[x][y] = NO_WALL;
            }
          }

          if (x != height_)  {
            if (y == 0 || y == width_) {
              ver_[x][y] = WALL;
            } else {
              ver_[x][y] = NO_WALL;
            }
          }
          
          if (x != height_ || y != width_) 
            cell_[x][y] = 0;
        }
      } 
    }

    // Copy constructor
    maze_layout(const maze_layout& other) {
      width_ = other.width_;
      height_ = other.height_;
      robot_ = other.robot_;
      start_ = other.start_;
      dest_ = other.dest_;

      for (uint8_t x = 0; x <= height_; ++x) {
        for (uint8_t y = 0; y <= width_; ++y) {
          if (y != width_)
            hor_[x][y] = other.hor_[x][y];
          if (x != height_)
            ver_[x][y] = other.ver_[x][y]; 
          if (x != height_ || y != width_)
            cell_[x][y] = other.cell_[x][y];
        }
      } 
    }

    maze_layout& operator=(const maze_layout& other) {
      width_ = other.width_;
      height_ = other.height_;
      robot_ = other.robot_;
      start_ = other.start_;
      dest_ = other.dest_;

      for (uint8_t x = 0; x <= height_; ++x) {
        for (uint8_t y = 0; y <= width_; ++y) {
          if (y != width_)
            hor_[x][y] = other.hor_[x][y];
          if (x != height_)
            ver_[x][y] = other.ver_[x][y]; 
          if (x != height_ || y != width_)
            cell_[x][y] = other.cell_[x][y];
        }
      } 
      return *this; 
    }

    uint8_t getWidth() { return width_; }
    uint8_t getHeight() { return height_; }

    void setHor(uint8_t x, uint8_t y, uint8_t val) { hor_[x][y] = val; }
    uint8_t getHor(uint8_t x, uint8_t y) { return hor_[x][y]; }

    void setVer(uint8_t x, uint8_t y, uint8_t val) { ver_[x][y] = val; }
    uint8_t getVer(uint8_t x, uint8_t y) { return ver_[x][y]; }

    cell_location getRob() const { return robot_; }
    void setRob(cell_location& robot) { robot_ = robot; }

    cell_location getStart() const { return start_; }
    void setStart(cell_location& start) { start_ = start; }

    cell_location getDest() const { return dest_; }
    void setDest(cell_location& dest) { dest_ = dest; }

    int getRobX() { return robot_.getX(); }
    void setRobX(int x) { robot_.setX(x); }

    int getRobY() { return robot_.getY(); }
    void setRobY(int y) { robot_.setY(y); }

    Direction getRobDir() { return robot_.getDir(); }
    void setRobDir(Direction dir) { robot_.setDir(dir); }

    uint8_t getPos(int x, int y) { return cell_[x][y]; } 
    void setPos(int x, int y, uint8_t val) { cell_[x][y] = val; }

    bool canMove(Move move);
  
    bool canMoveU(Move move);

    bool isDest(cell_location cell) {
      return (cell.getX() == dest_.getX() && cell.getY() == dest_.getY());
    }

    bool isStart(cell_location cell) {
      return (cell.getY() == start_.getY() && cell.getX() == start_.getX()) ? true : false;
    }

    Direction getStartDir() { return start_.getDir(); }


  private:
    uint8_t width_;
    uint8_t height_; 
    cell_location robot_;
    cell_location start_;
    cell_location dest_;
    uint8_t hor_[HEIGHT+1][WIDTH];
    uint8_t ver_[HEIGHT][WIDTH+1]; 
    uint8_t cell_[HEIGHT][WIDTH];
};

} // namespace hardware
#endif

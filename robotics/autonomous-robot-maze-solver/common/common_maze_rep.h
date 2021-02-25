#ifndef COMMON_MAZE_REP_H_
#define COMMON_MAZE_REP_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#include <string>

namespace common {

static const int WIDTH = 9;
static const int HEIGHT = 9;
static const uint8_t TRAPA = 0x02;
static const uint8_t TRAPB = 0x84;

static const uint8_t EMPTY = 0;
static const uint8_t FULL = 1;

enum RobotOrientation {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

class StringBuilder {
    private:
        char *str_;
        int max_len_;
    public:
        StringBuilder(char *str, int max_len) :
            str_{str},
            max_len_{max_len}
        {
            memset(str, 0, max_len * sizeof(char));
        }

        char *str() {return str_;};

        friend StringBuilder& operator<<(StringBuilder& sb, const char *str) {
            strncat(sb.str_, str, sb.max_len_);
            return sb;
        }

        friend StringBuilder& operator<<(StringBuilder& sb, const char c) {
                char str[2] = {c, 0};
                strncat(sb.str_, str, sb.max_len_);
                return sb;
        }

        
        friend StringBuilder& operator<<(StringBuilder& sb, const int i) {
            /*
                char str[100];
                //itoa(str, i, 10);
                
                //strncat(sb.str_, std::to_string(i).c_str(), sb.max_len_);
                snprintf(str, 100, "%d", i);
                strncat(sb.str_, str, sb.max_len_);
                return sb;
                */

            return sb << static_cast<char>(i + '0');
        }
};

#pragma pack(push, 1)
struct MazeRep {
    // NOTE ALL 1 Byte so we dont need to worry about endiness!!!
    uint8_t width_;
    uint8_t height_; 

    // Indicates there is a hor. wall at [row][col]
    uint8_t hor_[HEIGHT + 1][WIDTH + 1];
    uint8_t ver_[HEIGHT + 1][WIDTH + 1]; 
    
    // x is ver, y width, top left (0, 0)
    uint8_t robot_x_;
    uint8_t robot_y_;

    uint8_t chk_;

    // in degrees
    RobotOrientation robot_orientation_;

    MazeRep() :
        width_{0},
        height_{0},
        robot_x_(0),
        robot_y_(0)
    
    {
        memset(hor_, 1, sizeof(hor_));
        memset(ver_, 1, sizeof(ver_));
    }

    MazeRep(uint8_t rows, uint8_t cols) :
        width_{cols},
        height_{rows},
        robot_x_(0),
        robot_y_(0)
    {
        memset(hor_, 0, sizeof(hor_));
        memset(ver_, 0, sizeof(ver_));
        for (auto i = 0; i <= height_; ++i) {  
            for (auto j = 0; j <= width_; ++j) {  
                hor_[i][j] = (i == 0 || i == height_) ? FULL : EMPTY;  
                ver_[i][j] = (j == 0 || j == width_) ? FULL :EMPTY;  
            }  
        }  
    }

    MazeRep(uint8_t *buf, bool& succ) {
        // Note we are not worrying about endiness
        if (*(buf++) != TRAPA || *(buf++) != TRAPB) {
            //throw "MazeRep traps did not match!";
            return;
        }
        memcpy(this, buf, sizeof(MazeRep));
        
        succ = (chk_ == calc_chk());
    }

    void fill_buff(uint8_t *buf) {
        *(buf++) = TRAPA;
        *(buf++) = TRAPB;

        chk_ = calc_chk();

        memcpy(buf, this, sizeof(MazeRep));
    }

    void to_string(char *str, int len) {
        memset(str, 0, len);

        StringBuilder msg(str, len);
        char robot_orientation_marker;
        
        switch(robot_orientation_) {
            case RobotOrientation::DOWN: robot_orientation_marker = 'V'; break;
            case RobotOrientation::RIGHT: robot_orientation_marker = '>'; break;
            case RobotOrientation::UP: robot_orientation_marker = '^'; break;
            case RobotOrientation::LEFT: robot_orientation_marker = '<'; break;
        }
        
        msg << "Maze " << (int)height_ << "x" << (int)width_ << "\n";
        for (auto i = 0; i <= height_; i++) {
            for (auto j = 0; j < width_; j++) {
                msg << (hor_[i][j] == FULL ? "---" : "   ");
            }
            msg << "\n";
            for (auto j = 0; j <= width_; j++) {
                char middle_marker = (robot_x_ == i && robot_y_ == j) ? robot_orientation_marker : ' ';

                msg << (ver_[i][j] == FULL ? "|" : " ") << middle_marker << " ";
            }

            msg << "\n";
        }

        /*
        int height_count = 0;
        int width_count = 0;
        int count = 0; 
        while (count <= 2*getHeight()) {
            if (count % 2 == 0) {
                for (int i = 0; i < getWidth(); ++i) {
                    msg << " ";
                    if (getHor(i, width_count) == 1)
                        msg << "---";
                    else 
                        msg << "   ";
                }
                width_count++;
            } else {
                for (int i = 0; i <= getWidth(); ++i){
                    if (getVer(i, height_count) == 1)
                        msg << "|";
                    else 
                        msg << " ";

                    if (i != getWidth())
                        msg << "   ";

                }
                height_count++;
            }
            msg << "";
            count++; 
        }
        */
    }

    uint8_t calc_chk() {
        uint8_t sum = 0;
        for (auto i = 0; i < sizeof(MazeRep) -1; i++) {
            sum += *((uint8_t *)this + i);
        }
        return sum;
    }
};
#pragma pack(pop)

static const int BUF_SIZE = sizeof(MazeRep) + 2;

} // namespace common
#endif

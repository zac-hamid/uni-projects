#ifndef DRIVE_POSE_H_
#define DRIVE_POSE_H_

#include <avr/pgmspace.h>

namespace drive {

class Pose {
  public:
    double x;
    double y;
    double theta;

    Pose() = default;

    Pose(const double& _x, const double& _y, const double& _theta) :
        x{_x},
        y{_y},
        theta{_theta}
    {
        // nop
    }

    template <typename serial>
    void print() {
        serial::print(F("Pose {"));
        serial::print(x);
        serial::print(F(" ,"));
        serial::print(y);
        serial::print(F(" ,"));
        serial::print_line('}');
    }

};

} // namespace drive

#endif

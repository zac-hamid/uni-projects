#ifndef POSE_STAMPED_H_
#define POSE_STAMPED_H_

#include <string>
#include <iostream>

namespace localisation {
class PoseStamped {
  public:
    std::string frame_id;
    int x = 0;
    int y = 0;
    int z = 0;
    double yaw = 0; // about z axis

    PoseStamped() = default;

    friend std::ostream& operator<<(std::ostream& os, const PoseStamped& ps) {
        os << "(x: " << ps.x << ", y: " << ps.y << ", z: " << ps.z << ", yaw: " << ps.yaw << ")";
        return os;
    }
};

} // namespace localisation
#endif

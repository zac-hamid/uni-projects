#include "math_utils.h"

#include <cmath>

namespace utils {
// Normalise to -pi -> pi
double normalise_angle(double angle) {
    while (angle < M_PI) angle += 2*M_PI;
    while (angle > M_PI) angle -= 2*M_PI;

    return angle;
}

} // namespace utils

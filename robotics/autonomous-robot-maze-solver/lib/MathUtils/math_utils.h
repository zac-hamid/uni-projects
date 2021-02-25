#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

#include <Arduino.h>

constexpr float DEG2RAD(float x) {
    return (x * (PI/180.0));
}

constexpr float RAD2DEG(float x) {
    return (x * (180.0/PI));
}

constexpr float RPM_TO_RADS_S(float rpm) {
    return (rpm * 2 * PI) / 60.0;
}

constexpr float RADS_S_TO_RPM(float rads_s) {
    return (rads_s * 60) / (2 * PI);
}

constexpr float NORMALIZE_THETA(float theta) {
    while (theta > PI) theta -= 2*PI;
    while (theta < -PI) theta += 2*PI;
    return theta;
}

template<typename T>
constexpr T CLAMP(const T& x, const T& min, const T& max) {
    if (x > max) return max;
    else if (x < min) return min;
    else return x;
}

template <typename T>
inline constexpr T SQUARE(const T& x) {
    return x * x;
}

template <typename T>
inline constexpr T REMAP_RANGE(const T& val, const T& input_low, const T& input_high, 
        const T& output_low, const T& output_high) {
    return output_low + (val - input_low) * (output_high - output_low) / (float)(input_high - input_low);
}

#endif

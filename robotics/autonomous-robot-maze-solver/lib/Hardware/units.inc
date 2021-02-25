#include <Arduino.h>
#include "units.h"

// Jesus christ why are you using #undef...
// Because of name clashes with Arduino.h
// units.h MUST be #included AFTER Arduino.h
// This is only an issue because of the typecast operators


namespace units {
/***********************************/
/*********** base_unit *************/
/***********************************/

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator+ (unit_type rhs) const -> unit_type {
    return value_ + rhs.value_;

}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator- (unit_type rhs) const -> unit_type {
    return  value_ - rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator* (value_type rhs) const -> unit_type {
    return  value_ * rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator/ (value_type rhs) const -> unit_type {
    return  value_ / rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator< (unit_type rhs) const -> bool {
    return value_ < rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator<= (unit_type rhs) const -> bool {
    return value_ <= rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator> (unit_type rhs) const -> bool {
    return value_ > rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator>= (unit_type rhs) const -> bool {
    return value_ >= rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator== (unit_type rhs) const -> bool {
    return value_ == rhs.value_;
}

template <typename unit_type, typename value_t>
auto base_unit<unit_type, value_t>::operator!= (unit_type rhs) const -> bool {
    return value_ != rhs.value_;
}
} // namespace units

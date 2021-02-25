#include <Arduino.h>
#include "units.h"

namespace units {
/***********************************/
/********** microseconds ***********/
/***********************************/

auto microseconds::now () noexcept -> microseconds {
    unsigned long time = micros();
    return microseconds{time};
}


/***********************************/
/********** milliseconds **********/
/***********************************/

auto milliseconds::now () noexcept -> milliseconds {
    unsigned long time = millis();
    return milliseconds{time};
}

/***********************************/
/************** sleep **************/
/***********************************/

auto sleep_for (milliseconds period) -> void {
    delay(period.count());
}

auto sleep_for (microseconds period) -> void {
    delayMicroseconds(period.count());
}

auto sleep_until (milliseconds wake_time) -> void {
    while (milliseconds::now() < wake_time) {
        // nop
    }
}

auto sleep_until (microseconds wake_time) -> void {
    while (microseconds::now() < wake_time) {
        // nop
    }
}

degrees::operator radians () const {
    units::radians ret{count() * DEG_TO_RAD};
    return ret;
}

radians::operator degrees () const {
    units::degrees ret{count() * RAD_TO_DEG};
    return ret;
}

} // namespace units

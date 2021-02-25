#include <stdint.h>

#include "hardware.h"

namespace hardware {
// Convert a analog_reference to an arduino mode.
// Why this is not a method of an class enum is... odd
// Note we check if these consts are defined so it will compile for a nano etc.
uint8_t locomotion::to_mode(analog_reference ref) {
    uint8_t mode;
    switch(ref) {
        case analog_reference::vcc_default: mode = DEFAULT; break;
#ifdef INTERNAL
        case analog_reference::internal: mode = INTERNAL; break;
#else 
        #warning "INTERNAL not defined!"
#endif
        // Not all arduinos have all of these defined...
#ifdef INTERNAL1V1 
        case analog_reference::internal_1v1: mode = INTERNAL1V1; break;
#else
        #warning "INTERNAL1V1 not defined! only defined on Mega!"
#endif

#ifdef INTERNAL2V56
        case analog_reference::internal_2v56: mode = INTERNAL2V56; break;
#else
        #warning "INTERNAL2V56 not defined! only defined on Mega!"
#endif
        case analog_reference::external: mode = EXTERNAL; break;
        default: mode = DEFAULT; break;
    }
    return mode;
}

double locomotion::to_range(analog_reference ref) {
    double range;
    switch(ref) {
        case analog_reference::vcc_default: range = locomotion::BOARD_VOLTAGE; break;
#ifdef INTERNAL
        case analog_reference::internal: range = locomotion::INTERNAL_VOLTAGE; break;
#else 
        #warning "INTERNAL not defined!"
#endif
        // Not all arduinos have all of these defined...
#ifdef INTERNAL1V1 
        case analog_reference::internal_1v1: range = 1.1; break;
#else
        #warning "INTERNAL1V1 not defined! only defined on Mega!"
#endif

#ifdef INTERNAL2V56
        case analog_reference::internal_2v56: range = 2.56; break;
#else
        #warning "INTERNAL2V56 not defined! only defined on Mega!"
#endif
        // TODO: Check if there is another way of doing this
        case analog_reference::external: range = 5.0; break;
        default: range = BOARD_VOLTAGE; break;
    }
    return range; 

}
}

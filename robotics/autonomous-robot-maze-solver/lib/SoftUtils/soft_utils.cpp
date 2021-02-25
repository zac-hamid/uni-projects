#include "soft_utils.h"

#include <Arduino.h>

/*
 * Returns true if set
 * False else
 */
bool test_and_set(bool& b) {
    if (b) return false;
    noInterrupts();
    bool b_copy = b;
    b = true;
    interrupts();

    return !b_copy;
}


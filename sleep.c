//
// main.c
//
// Copyright (C) 2016 by Eric Dey. All rights reserved.
//
#include "sleep.h"


// This implements a millisecond delay function that takes an unsigned long
// input. This function is designed to be good enough for general purposes
// that include human-perception scales. It is not designed for high accuracy.
// When the remaining time is less than 0.125ms the delay can be ended early.
void delay_ms(unsigned long ms) {
    unsigned long cycles;   // total count of remaining cycles to wait
    unsigned long cutoff;   // cutoff limit below which no more delay is done

    cycles = (ms)*(_XTAL_FREQ/4000);
    cutoff = (_XTAL_FREQ/4000) / 8;    // 0.125ms lower limit
    
    // Handle 10,000's of cycles  -- avoid calling _delay(n) with n > 10,000
    while (cycles >= 20000) {
        cycles -= 20000;
        _delay(10000);
        _delay(10000);
    }
    if (cycles >= 10000) {
        cycles -= 10000;
        _delay(10000);
    }

    if (cycles <= cutoff) return;  // Exit as soon as we drop below the cutoff

    // Handle 1000's of cycles
    while (cycles >= 2000) {
        cycles -= 2000;
        _delay(2000);
    }
    if (cycles >= 1000) {
        cycles -= 1000;
        _delay(1000);
    }

    if (cycles <= cutoff) return;  // Exit as soon as we drop below the cutoff

    // Handle 100's of cycles
    while (cycles >= 200) {
        cycles -= 200;
        _delay(200);
    }
    if (cycles >= 100) {
        cycles -= 100;
        _delay(100);
    }

    if (cycles <= cutoff) return;  // Exit as soon as we drop below the cutoff

    // Handle 10's of cycles
    while (cycles >= 20) {
        cycles -= 20;
        _delay(20);
    }
    if (cycles >= 10) {
        cycles -= 10;
        _delay(10);
    }

    if (cycles <= cutoff) return;  // Exit as soon as we drop below the cutoff

    // At this point, there are less than 10 cycles remaining and, unless
    // the oscillator is really slow, execution will already have been ended.
    // This is code is followed through to the end mostly out of a need for
    // completion.

    // Handle remaining cycles of 3
    while (cycles >= 2) {
        cycles -= 3;
        _delay3(1);
    }
    
    if (cycles == 0) return;

    // Handle last 1 or 2 cycles -- accuracy is highly questionable by now
    _delay(1);
    if (cycles == 2) _delay(1);
}


// This implements a Python-style sleep that takes a floating point number
// of seconds. This goes down to 1ms resolution.
void sleep(float seconds) {
    unsigned long min, ms;

    // Do this 60,000ms (1 min) at a time to avoid overflowing size of long
    min = (unsigned long) (seconds / 60.0);
    for (; min > 0; min--) {
        delay_ms(60000);
        seconds -= 60.0;
    }

    // Remainder of time is less than seconds; handle in one request
    ms = (unsigned long) (1000.0 * seconds);
    delay_ms(ms);
}

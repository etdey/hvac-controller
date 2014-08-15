//
// main.c
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//

#include "sleep.h"


// This implements a delay function using the built-in plib functions. It
// is more flexible because it takes a long int for its input.
void delay_ms(unsigned long ms) {
    unsigned long cycles;
    unsigned char n;

    cycles = (ms)*(_XTAL_FREQ/4000);

    // Handle 10,000's of cycles
    while (cycles >= 10000) {
        if (cycles >= 2550000) {
            Delay10KTCYx(255);
            cycles -= 2550000;
        } else {
            n = cycles / 10000;
            cycles -= ((unsigned long)n * 10000);
            Delay10KTCYx(n);
        }
    }

    // Handle 1000's of cycles
    n = cycles / 1000;
    if (n > 0) {
        cycles -= ((unsigned long)n * 1000);
        Delay1KTCYx(n);
    }

    // Handle 100's of cycles
    n = cycles / 100;
    if (n > 0) {
        cycles -= ((unsigned long)n * 100);
        Delay100TCYx(n);
    }

    // Handle 10's of cycles
    n = cycles / 10;
    if (n > 0) {
        cycles -= ((unsigned long)n * 10);
        Delay10TCYx(n);
    }

    // Handle remaining cycles
    n = cycles;
    if (n > 0) {
        Delay1TCYx(n);
    }
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

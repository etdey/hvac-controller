//
// timevalues.h
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// Time transition values
//

#ifndef TIMEVALUES_H
#define	TIMEVALUES_H

#ifdef	__cplusplus
extern "C" {
#endif

// This must be less than the WDT or the MCU will reset. This must also
// be longer than the TMR0 ISR period.
#define SECONDS_BETWEEN_SAMPLES     0.250

// This is derived from the TMR0 period, callback rate, and corrected
// for the observed oscillator error factor.
// Oscillator error: too slow < 1.0 > too fast)
// 
// Computing initial value:
// OscillatorError = {ActualElapsedTime} / {DeviceElapsedTime}
// NSEC_BETWEEN_TIMER_CALLBACKS = 100000000ns / {OscillatorError}
//
// Computing adjustment to prior value:
// ErrorSeconds = deltaClockReported - deltaClockComputed
// newValue = int((1 - ({ErrorSeconds}/{ElapsedSeconds})) * {PreviousValue})
//
#define NSEC_BETWEEN_TIMER_CALLBACKS_UP    81765896
#define NSEC_BETWEEN_TIMER_CALLBACKS_DOWN  81766911

// Times requirements for state transitions. Values are in seconds.
#define COOL_ON_TO_COOL_OFF         1200   /*  20 minutes */
#define COOL_OFF_TO_COOL_ON         1800   /*  30 minutes */
#define COOL_OFF_TO_HEAT_ON         3600   /*  60 minutes */
#define HEAT_ON_TO_HEAT_OFF         1800   /*  30 minutes */
#define HEAT_OFF_TO_HEAT_ON         1200   /*  20 mintues */
#define HEAT_OFF_TO_COOL_ON         3600   /*  60 minutes */
#define FAN_BEFORE_COOL_ON          900    /*  15 minutes */
#define FAN_BEFORE_HEAT_ON          900    /*  15 minutes */
#define FAN_AFTER_COOL_OFF          300    /*   5 minutes */
#define FAN_AFTER_HEAT_OFF          120    /*   2 minutes */
#define FAN_ON_TO_FAN_OFF           300    /*   5 minutes */
#define FAN_OFF_TO_FAN_ON           600    /*  10 minutes */
#define MAX_COOL_ON                 7200   /* 120 minutes */
#define MAX_HEAT_ON                 10800  /* 180 minutes */
#define MAX_ALL_OFF_TO_FAN_ON       14400  /*   4 hours */
#define MIN_COOL_OFF_TO_INSTANT_ON  86400  /*  24 hours */
#define MIN_HEAT_OFF_TO_INSTANT_ON  43200  /*  12 hours */

#ifdef	__cplusplus
}
#endif

#endif


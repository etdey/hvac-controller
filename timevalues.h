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

// This must be less than the WDT or the MCU will reset.
#define SECONDS_BETWEEN_SAMPLES     1.0

// This is derived from the TMR0 period, callback rate, and corrected
// for the observed oscillator error factor.
// Oscillator error: too slow < 1.0 > too fast)
// 
// Computing initial value:
// OscillatorError = {ActualElapsedTime} / {RecordedElapsedTime}
// MSEC_BETWEEN_TIMER_CALLBACKS = 100ms / {OscillatorError}
//
// Computing adjustment to prior value:
// (1 - ({ErrorSeconds}/{ElapsedSeconds})) * {PreviousValue}
//
#define MSEC_BETWEEN_TIMER_CALLBACKS  81.76752686

// Times requirements for state transitions. Values are in seconds.
#define COOL_ON_TO_COOL_OFF         1200   /*  20 minutes */
#define COOL_OFF_TO_COOL_ON         1800   /*  30 minutes */
#define COOL_OFF_TO_HEAT_ON         3600   /*  60 minutes */
#define HEAT_ON_TO_HEAT_OFF         1200   /*  20 minutes */
#define HEAT_OFF_TO_HEAT_ON         1200   /*  20 mintues */
#define HEAT_OFF_TO_COOL_ON         3600   /*  60 minutes */
#define FAN_BEFORE_COOL_ON          900    /*  15 minutes */
#define FAN_BEFORE_HEAT_ON          900    /*  15 minutes */
#define FAN_AFTER_COOL_OFF          420    /*   7 minutes */
#define FAN_AFTER_HEAT_OFF          420    /*   7 minutes */
#define MAX_COOL_ON                 7200   /* 120 minutes */
#define MAX_HEAT_ON                 10800  /* 180 minutes */

#ifdef	__cplusplus
}
#endif

#endif


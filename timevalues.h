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
// Oscillator error = 1.2231 (> 1.0 is too fast)
// 100ms / 1.2231 = 81.76ms  ~= 82ms
#define MSEC_BETWEEN_TIMER_CALLBACKS  82

// Times requirements for state transitions. Values are in seconds.
#define COOL_ON_TO_COOL_OFF         30 * 60
#define COOL_OFF_TO_COOL_ON         20 * 60
#define COOL_OFF_TO_HEAT_ON         60 * 60
#define HEAT_ON_TO_HEAT_OFF         20 * 60
#define HEAT_OFF_TO_HEAT_ON         20 * 60
#define HEAT_OFF_TO_COOL_ON         60 * 60
#define FAN_BEFORE_COOL_ON          15 * 60
#define FAN_BEFORE_HEAT_ON          15 * 60
#define FAN_AFTER_COOL_OFF          15 * 60
#define FAN_AFTER_HEAT_OFF          15 * 60
#define MAX_COOL_ON                 120 * 60
#define MAX_HEAT_ON                 180 * 60

#ifdef	__cplusplus
}
#endif

#endif


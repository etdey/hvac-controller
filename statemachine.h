//
// statemachine.c
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// This module defines the state machine and the transition rules
// for the A/C controller module.
//

#ifndef STATEMACHINE_H
#define	STATEMACHINE_H

#include "datatypes.h"

#ifdef	__cplusplus
extern "C" {
#endif


typedef void *ACStateMachine;


typedef enum {
    INITIAL,
    ALL_OFF,
    FAN_ON,
    COOL_ON,
    HEAT_ON,
    COOL_OFF_FAN_ON,
    HEAT_OFF_FAN_ON,
    ILLEGAL
} ControlStateDescription;


typedef enum {
    UNKNOWN = 0,
    COOLING = 1,
    HEATING = 2
} ConditioningFunction;


ACStateMachine fsm_init(void);
ControlStateDescription fsm_evaluate(ACStateMachine, ControlStateDescription, ControlStateDescription, ACControlTimers *);


#ifdef	__cplusplus
}
#endif

#endif

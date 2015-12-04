//
// statemachine.c
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// This module defines the state machine and the transition rules
// for the A/C controller module.
//

#include "statemachine.h"
#include "datatypes.h"
#include "timevalues.h"


typedef struct {
    ControlStateDescription current;
    ControlStateDescription target;
    ControlStateDescription (*computeNext)(ACStateMachine, ACControlTimers *);
} ACStateTableEntry ;


struct  ACStateMachine_struct {
    int numEdges;
    ACStateTableEntry *edges;
    ControlStateDescription currentState;
    ConditioningFunction lastCondioningFunct;
};


ControlStateDescription trans_all_off_to_fan_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != ALL_OFF) return(ILLEGAL);

    // Ensure that enough time has passed since fan was last on.
    if (timers->fanOff < FAN_OFF_TO_FAN_ON) return (ALL_OFF);

    return(FAN_ON);
}

ControlStateDescription trans_fan_on_to_all_off(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != FAN_ON) return(ILLEGAL);

    // Ensure that fan has been on long enough before turning it off
    if (timers->fanOn < FAN_ON_TO_FAN_OFF) return(FAN_ON);

    return(ALL_OFF);
}

ControlStateDescription trans_fan_on_to_cool_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != FAN_ON) return(ILLEGAL);

    // Run the fan prior to turning on the cooling function unless sufficient
    // time has passed since last cooling or heating use. Following this test,
    // the time since heating was last on must still be checked to ensure a
    // minimum even if fan has run long enough.
    if ((timers->coolOff < MIN_COOL_OFF_TO_INSTANT_ON ||
            timers->heatOff < MIN_COOL_OFF_TO_INSTANT_ON) &&
            timers->fanOn < FAN_BEFORE_COOL_ON
            ) return (FAN_ON);

    // Unless there has not been a previous conditioning function, ensure that
    // enough time has pasted since previous conditioning function.
    if (f->lastCondioningFunct != UNKNOWN) {
        if (timers->coolOff < COOL_OFF_TO_COOL_ON) return(FAN_ON);
        if (timers->heatOff < HEAT_OFF_TO_COOL_ON) return(FAN_ON);
    }

    f->lastCondioningFunct = COOLING;

    return(COOL_ON);
}

ControlStateDescription trans_fan_on_to_heat_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != FAN_ON) return(ILLEGAL);

    // Run the fan prior to turning on the heating function unless sufficient
    // time has passed since last heating or cooling use. Following this test,
    // the time since cooling was last on must still be checked to ensure a
    // minimum even if fan has run long enough.
    if ((timers->heatOff < MIN_HEAT_OFF_TO_INSTANT_ON ||
            timers->coolOff < MIN_HEAT_OFF_TO_INSTANT_ON) &&
            timers->fanOn < FAN_BEFORE_HEAT_ON) return (FAN_ON);

    // Unless there has not been a previous conditioning function, ensure that
    // enough time has pasted since previous conditioning function.
    if (f->lastCondioningFunct != UNKNOWN) {
        if (timers->heatOff < HEAT_OFF_TO_HEAT_ON) return(FAN_ON);
        if (timers->coolOff < COOL_OFF_TO_HEAT_ON) return(FAN_ON);
    }

    f->lastCondioningFunct = HEATING;

    return(HEAT_ON);
}

ControlStateDescription trans_cool_on_to_cool_off_fan_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != COOL_ON) return(ILLEGAL);

    // Ensure that cooling has been on long enough before turning it off.
    if (timers->coolOn < COOL_ON_TO_COOL_OFF) return (COOL_ON);

    f->lastCondioningFunct = COOLING;

    return(COOL_OFF_FAN_ON);
}

ControlStateDescription trans_cool_off_fan_on_to_all_off(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != COOL_OFF_FAN_ON) return(ILLEGAL);

    // Ensure that fan runs long enough after completion of cooling function.
    // Once FAN_OFF state is entered, FAN_ON requires no minimum transition time.
    if (timers->coolOff < FAN_AFTER_COOL_OFF) return (COOL_OFF_FAN_ON);

    return(ALL_OFF);
}

ControlStateDescription trans_cool_off_fan_on_to_fan_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != COOL_OFF_FAN_ON) return(ILLEGAL);

    // Ensure that fan runs long enough after completion of cooling function.
    // Once FAN_ON state is entered, FAN_OFF requires no minimum transition time.
    if (timers->coolOff < FAN_AFTER_COOL_OFF) return (COOL_OFF_FAN_ON);

    return(FAN_ON);
}

ControlStateDescription trans_heat_on_to_heat_off_fan_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != HEAT_ON) return(ILLEGAL);

    // Ensure that heating has been on long enough before turning it off.
    if (timers->heatOn < HEAT_ON_TO_HEAT_OFF) return (HEAT_ON);

    f->lastCondioningFunct = HEATING;

    return(HEAT_OFF_FAN_ON);
}

ControlStateDescription trans_heat_off_fan_on_to_all_off(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != HEAT_OFF_FAN_ON) return(ILLEGAL);

    // Ensure that fan runs long enough after completion of heating function.
    // Once FAN_OFF state is entered, FAN_ON requires no minimum transition time.
    if (timers->heatOff < FAN_AFTER_HEAT_OFF) return (HEAT_OFF_FAN_ON);

    return(ALL_OFF);
}

ControlStateDescription trans_heat_off_fan_on_to_fan_on(ACStateMachine fsm, ACControlTimers *timers) {
    struct ACStateMachine_struct *f = fsm;
    if (f->currentState != HEAT_OFF_FAN_ON) return(ILLEGAL);

    // Ensure that fan runs long enough after completion of heating function.
    // Once FAN_ON state is entered, FAN_OFF requires no minimum transition time.
    if (timers->heatOff < FAN_AFTER_HEAT_OFF) return (HEAT_OFF_FAN_ON);

    return(FAN_ON);
}


ACStateMachine fsm_init() {
    static struct ACStateMachine_struct fsm;

    // This table contains the allowable state transitions. It must include
    // the intermediate transitions (e.g. COOL_ON to ALL_OFF) based on control
    // requests from the thermostat.
    const static ACStateTableEntry fsmEdgeTable[] = {
        {ALL_OFF, FAN_ON, &trans_all_off_to_fan_on},
        {ALL_OFF, COOL_ON, &trans_all_off_to_fan_on},
        {ALL_OFF, HEAT_ON, &trans_all_off_to_fan_on},

        {FAN_ON, ALL_OFF, &trans_fan_on_to_all_off},
        {FAN_ON, COOL_ON, &trans_fan_on_to_cool_on},
        {FAN_ON, HEAT_ON, &trans_fan_on_to_heat_on},

        {COOL_ON, ALL_OFF, &trans_cool_on_to_cool_off_fan_on},
        {COOL_ON, FAN_ON, &trans_cool_on_to_cool_off_fan_on},
        {COOL_ON, COOL_OFF_FAN_ON, &trans_cool_on_to_cool_off_fan_on},
        {COOL_ON, HEAT_ON, &trans_cool_on_to_cool_off_fan_on},

        {COOL_OFF_FAN_ON, ALL_OFF, &trans_cool_off_fan_on_to_all_off},
        {COOL_OFF_FAN_ON, FAN_ON, &trans_cool_off_fan_on_to_fan_on},
        {COOL_OFF_FAN_ON, COOL_ON, &trans_cool_off_fan_on_to_fan_on},
        {COOL_OFF_FAN_ON, HEAT_ON, &trans_cool_off_fan_on_to_fan_on},

        {HEAT_ON, ALL_OFF, &trans_heat_on_to_heat_off_fan_on},
        {HEAT_ON, FAN_ON, &trans_heat_on_to_heat_off_fan_on},
        {HEAT_ON, HEAT_OFF_FAN_ON, &trans_heat_on_to_heat_off_fan_on},
        {HEAT_ON, COOL_ON, &trans_heat_on_to_heat_off_fan_on},

        {HEAT_OFF_FAN_ON, ALL_OFF, &trans_heat_off_fan_on_to_all_off},
        {HEAT_OFF_FAN_ON, FAN_ON, &trans_heat_off_fan_on_to_fan_on},
        {HEAT_OFF_FAN_ON, HEAT_ON, &trans_heat_off_fan_on_to_fan_on},
        {HEAT_OFF_FAN_ON, COOL_ON, &trans_heat_off_fan_on_to_fan_on},
    };
    
    fsm.currentState = INITIAL;
    fsm.lastCondioningFunct = UNKNOWN;
    fsm.numEdges = sizeof(fsmEdgeTable) / sizeof(ACStateTableEntry);
    fsm.edges = (ACStateTableEntry *) &fsmEdgeTable;

    return((ACStateMachine)&fsm);
}


ControlStateDescription fsm_evaluate(ACStateMachine fsmV, ControlStateDescription current, ControlStateDescription target, ACControlTimers *timers) {
    int n = 0;
    struct ACStateMachine_struct *fsm;
    ControlStateDescription newState;

    fsm = (struct ACStateMachine_struct *) fsmV;
    fsm->currentState = current;

    while (n < fsm->numEdges) {
        if (fsm->edges[n].current == current && fsm->edges[n].target == target) {
            newState = fsm->edges[n].computeNext(fsm, timers);
            return(newState);
        }
        n++;
    }

    return(ILLEGAL);
}
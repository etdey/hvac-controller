//
// main.c
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// Air conditioner controller.
//

#include <limits.h>

#include "mcc_generated_files/mcc.h"
#include "sleep.h"
#include "timevalues.h"
#include "pinfunctions.h"
#include "datatypes.h"
#include "statemachine.h"


//
// Global variables for the module
//
ACControlLines systemLines, thermostatLines;
ControlStateDescription currentState, targetState;
ConditioningFunction lastTemperatureFunction;
ACControlTimers stateTimers;
ACStateMachine fsm;

unsigned long uptimeSeconds = 0; // whole seconds since startup
unsigned long uptimeExtraMS = 0; // fractional milliseconds added to uptime


//
// Send the system state to the control board
//
int setSystemLines(ControlStateDescription state) {
    switch (state) {
        case COOL_ON:
            SYSTEM_FAN = 1;
            SYSTEM_COOL = 1;
            SYSTEM_HEAT = 0;
            break;

        case HEAT_ON:
            SYSTEM_FAN = 1;
            SYSTEM_COOL = 0;
            SYSTEM_HEAT = 1;
            break;

        case COOL_OFF_FAN_ON:
        case HEAT_OFF_FAN_ON:
        case FAN_ON:
            SYSTEM_FAN = 1;
            SYSTEM_COOL = 0;
            SYSTEM_HEAT = 0;
            break;

        case ALL_OFF:
            SYSTEM_FAN = 0;
            SYSTEM_COOL = 0;
            SYSTEM_HEAT = 0;
            break;
    }

    return(0);
}


//
// Evaluate the given state and return enum value
//
ControlStateDescription evaluateState(ACControlLines *lines, ConditioningFunction lastCondition) {
    if (lines->cool != 0) {
        return (COOL_ON);
    }
    if (lines->heat != 0) {
        return (HEAT_ON);
    }
    if (lines->fan != 0) {
        if (lastCondition == COOLING) return (COOL_OFF_FAN_ON);
        if (lastCondition == HEATING) return (HEAT_OFF_FAN_ON);
        return (FAN_ON);
    }
    return (ALL_OFF);
}


//
// Interrupt callback to update the internal time keeping. Called from
// TMR0 ISR every 100ms.
//
void timerInterruptCallback() {
    uptimeExtraMS += MSEC_BETWEEN_TIMER_CALLBACKS;
    while (uptimeExtraMS >= 1000) {
        uptimeExtraMS -= 1000;
        uptimeSeconds++;
    }
}


//
// Update state timers based upon elapsed time measured by TMR0 ISR and
// based upon the system's current state.
//
void updateTimers(ACControlTimers *timers, ControlStateDescription currentState) {
    static unsigned long lastUptimeSeconds, lastUptimeExtraMS;
    double elapsedTime;

    if (currentState == INITIAL) {
        timers->coolOff = 0;
        timers->coolOn = 0;
        timers->fanOff = 0;
        timers->fanOn = 0;
        timers->heatOff = 0;
        timers->heatOn = 0;
        lastUptimeSeconds = uptimeSeconds;
        lastUptimeExtraMS = uptimeExtraMS;
        return;
    }

    // Compute elapsed time as a floating point value and update 'last' values.
    // The 'last' values are updated here to minimize time lossage from the
    // remaining processing in this function.
    elapsedTime = (double)(uptimeSeconds-lastUptimeSeconds) + \
            ((double)uptimeExtraMS / 1000) - (double)lastUptimeExtraMS / 1000;
    lastUptimeSeconds = uptimeSeconds;
    lastUptimeExtraMS = uptimeExtraMS;

    // Increment all counters and decide later which to clear
    timers->coolOff += elapsedTime;
    timers->coolOn += elapsedTime;
    timers->fanOff += elapsedTime;
    timers->fanOn += elapsedTime;
    timers->heatOff += elapsedTime;
    timers->heatOn += elapsedTime;
    
    // Clear counters to zero based on system state
    switch (currentState) {

        // Cool ON implies Fan ON
        case COOL_ON:
            timers->coolOff = 0;
            timers->fanOff = 0;
            timers->heatOn = 0;
            break;

        // Heat ON implies Fan ON
        case HEAT_ON:
            timers->heatOff = 0;
            timers->fanOff = 0;
            timers->coolOn = 0;
            break;

        // Cool OFF/Fan ON implies that cool was recently on
        case COOL_OFF_FAN_ON:
            timers->coolOn = 0;
            timers->fanOff = 0;
            timers->heatOn = 0;
            break;

        // Heat OFF/Fan ON implies that heat was recently on
        case HEAT_OFF_FAN_ON:
            timers->heatOn = 0;
            timers->fanOff = 0;
            timers->coolOn = 0;
            break;

        // Fan ON implies that cool/heat are off and that the previous
        // usage of cool/heat are not known.
        case FAN_ON:
            timers->coolOn = 0;
            timers->heatOn = 0;
            timers->fanOff = 0;
            break;

        // All OFF implies that cool/heat/fan are off
        case ALL_OFF:
            timers->coolOn = 0;
            timers->heatOn = 0;
            timers->fanOn = 0;
            break;
    }
}


//
// Handle I2C read requests. This is called from the I2C ISR.
//
uint8_t i2c_read(uint8_t dataAddress) {
    uint8_t retVal = 0;

    switch((I2CDataAddress)dataAddress) {
        case SYSTEM_STATE:
            retVal  = (systemLines.fan == 1) ? 1 : 0;
            retVal += (systemLines.cool == 1) ? 2 : 0;
            retVal += (systemLines.heat == 1) ? 4 : 0;
            break;

        case THERMOSTAT_STATE:
            retVal  = (thermostatLines.fan == 1) ? 1 : 0;
            retVal += (thermostatLines.cool == 1) ? 2 : 0;
            retVal += (thermostatLines.heat == 1) ? 4 : 0;
            break;

        case CURRENT_STATE:
            retVal = (uint8_t) currentState;
            break;

        case TARGET_STATE:
            retVal = (uint8_t) targetState;
            break;

        case LAST_COND_FUNCTION:
            retVal = (uint8_t) lastTemperatureFunction;
    }

    return(retVal);
}


void main(void) {

    // Initialize the PIC device
    SYSTEM_Initialize();
    //INTERRUPT_GlobalInterruptHighEnable();
    //INTERRUPT_GlobalInterruptLowEnable();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // Initialize system state
    systemLines.fan = 0;
    systemLines.cool = 0;
    systemLines.heat = 0;
    currentState = INITIAL;
    lastTemperatureFunction = UNKNOWN;
    fsm = fsm_init();

    // Enable watchdog timer: 4ms * 1024 postscaler = 4.09sec
    SWDTEN = 1;

    while (1)     {
        CLRWDT();
        updateTimers(&stateTimers, currentState);

        // Read requested control functions from thermostat
        thermostatLines.fan = (THERMOSTAT_FAN == 0) ? 0 : 1;
        thermostatLines.cool = (THERMOSTAT_COOL == 0) ? 0 : 1;
        thermostatLines.heat = (THERMOSTAT_HEAT == 0) ? 0 : 1;
        targetState = evaluateState(&thermostatLines, lastTemperatureFunction);

        // Initial starting condition
        if (currentState == INITIAL) {
            // When in initial state, set system to thermostat requested state
            systemLines.fan = thermostatLines.fan;
            systemLines.cool = thermostatLines.cool;
            systemLines.heat = thermostatLines.heat;
            currentState = targetState;

            if (currentState == COOL_ON) {
                lastTemperatureFunction = COOLING;
            } else if (currentState == HEAT_ON) {
                lastTemperatureFunction = HEATING;
            }

            setSystemLines(currentState);
            continue;
        }

        if (currentState != targetState) {
            targetState = fsm_evaluate(fsm, currentState, targetState, &stateTimers);

            // If we reach an illegal state, reset the MCU
            if (targetState == ILLEGAL) {
                RESET();
            }

            // If there is a new state, send it to the controller board
            if (currentState != targetState) {
                setSystemLines(targetState);
            }

            currentState = targetState;
        }

        CLRWDT();
        sleep(SECONDS_BETWEEN_SAMPLES);
    }
}

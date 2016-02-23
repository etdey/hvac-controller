//
// main.c
//
// Copyright (C) 2016 by Eric Dey. All rights reserved.
//
// Air conditioner controller.
//

#include <limits.h>
#include <xc.h>
#include <string.h>

#include "mcc_generated_files/mcc.h"
#include "sleep.h"
#include "timevalues.h"
#include "pinfunctions.h"
#include "datatypes.h"
#include "statemachine.h"
#include "XBeeAPIPackets.h"

// Firmware version values must be 0-255.
#define FIRMWARE_VERSION_MAJOR      3
#define FIRMWARE_VERSION_MINOR      13

// XBee Configuration
#define XBEE_DEST_MAC_HI            0x0013A200
#define XBEE_DEST_MAC_LO            0x40AF5B5D
#define XBEE_NI_MAXLEN              20
#define XBEE_COMMAND_MAXLEN         250
#define XBEE_RESPONSE_MAXLEN        250
#define XBEE_REPORT_INTERVAL_SEC    60
#define XBEE_REPORT_INTERVAL0_SEC   5

//
// Global variables for the module
//
ACControlLines systemLines, thermostatLines;
ControlStateDescription currentState, targetState;
ConditioningFunction lastTemperatureFunction;
ACControlTimers stateTimers;
ACStateMachine fsm;

volatile unsigned long uptimeSeconds = 0; // whole seconds since startup
volatile unsigned long uptimeExtraNS = 0; // nanoseconds component of uptime

unsigned char ioWatchdogSeconds = 0; // number of seconds; zero to disable
unsigned long ioWatchdogLastReset = 0; // timestamp of last I/O WD reset

char xbee_command_str[XBEE_COMMAND_MAXLEN+1];
char xbee_response_str[XBEE_COMMAND_MAXLEN+1];
XBeeStatusPayload xbee_status;
unsigned long xbee_nextReportTime;

unsigned long timerCallbackPeriodNsec;


//
// Send the system state to the control board
//
int setSystemLines(ControlStateDescription state) {
    switch (state) {
        case COOL_ON:
            SYSTEM_FAN = 1;
            SYSTEM_COOL = 1;
            SYSTEM_HEAT = 0;
            systemLines.fan = 1;
            systemLines.cool = 1;
            systemLines.heat = 0;
            break;

        case HEAT_ON:
            SYSTEM_FAN = 1;
            SYSTEM_COOL = 0;
            SYSTEM_HEAT = 1;
            systemLines.fan = 1;
            systemLines.cool = 0;
            systemLines.heat = 1;
            break;

        case COOL_OFF_FAN_ON:
        case HEAT_OFF_FAN_ON:
        case FAN_ON:
            SYSTEM_FAN = 1;
            SYSTEM_COOL = 0;
            SYSTEM_HEAT = 0;
            systemLines.fan = 1;
            systemLines.cool = 0;
            systemLines.heat = 0;
            break;

        case ALL_OFF:
            SYSTEM_FAN = 0;
            SYSTEM_COOL = 0;
            SYSTEM_HEAT = 0;
            systemLines.fan = 0;
            systemLines.cool = 0;
            systemLines.heat = 0;
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
    uptimeExtraNS += timerCallbackPeriodNsec;
    while (uptimeExtraNS >= 1000000000) {
        uptimeExtraNS -= 1000000000;
        uptimeSeconds++;

        // Heartbeat light
        CIRCUIT_HEARTBEAT = ~CIRCUIT_HEARTBEAT;
        //IO_RA4_Toggle();

        // Clear old status light
        CIRCUIT_STATUS = 0;
    }
}


//
// Update state timers based upon elapsed time measured by TMR0 ISR and
// based upon the system's current state.
//
void updateTimers(ACControlTimers *timers, ControlStateDescription currentState) {
    static unsigned long lastUptimeSeconds;
    unsigned long currUptimeSeconds, elapsedTime;

    currUptimeSeconds = uptimeSeconds; // Capture this now in case it updates
    
    if (currentState == INITIAL) {
        lastUptimeSeconds = currUptimeSeconds;

        // Reset the total time counters for each state
        timers->fanOn = 0;
        timers->fanOff = 0;
        timers->coolOn = 0;
        timers->coolOff = 0;
        timers->heatOn = 0;
        timers->heatOff = 0;

        // Set the initial time counters for reporting interval
        timers->fanOn_repInterval = 0;
        timers->fanOff_repInterval = 0;
        timers->coolOn_repInterval = 0;
        timers->coolOff_repInterval = 0;
        timers->heatOn_repInterval = 0;
        timers->heatOff_repInterval = 0;

        return;
    }

    // Compute the elapsed time since the last call to this function and
    // return if a whole second has not passed.
    elapsedTime = currUptimeSeconds - lastUptimeSeconds;
    if (elapsedTime <= 0) return;

    // Update last uptime value; do not use it again in this function
    lastUptimeSeconds = currUptimeSeconds;

    // Update counters based on state; reporting interval counters are reset
    // when a report is transmitted.
    switch (currentState) {

        // Cool ON implies Fan ON
        case COOL_ON:
            timers->fanOff = 0;
            timers->coolOff = 0;
            timers->heatOn = 0;

            timers->fanOn += elapsedTime;
            timers->coolOn += elapsedTime;
            timers->heatOff += elapsedTime;

            timers->fanOn_repInterval += elapsedTime;
            timers->coolOn_repInterval += elapsedTime;
            timers->heatOff_repInterval += elapsedTime;

            break;

        // Heat ON implies Fan ON
        case HEAT_ON:
            timers->fanOff = 0;
            timers->coolOn = 0;
            timers->heatOff = 0;

            timers->fanOn += elapsedTime;
            timers->coolOff += elapsedTime;
            timers->heatOn += elapsedTime;

            timers->fanOn_repInterval += elapsedTime;
            timers->coolOff_repInterval += elapsedTime;
            timers->heatOn_repInterval += elapsedTime;

            break;

        // Cool OFF/Fan ON implies that cool was recently on
        case COOL_OFF_FAN_ON:
            timers->fanOff = 0;
            timers->coolOn = 0;
            timers->heatOn = 0;

            timers->fanOn += elapsedTime;
            timers->coolOff += elapsedTime;
            timers->heatOff += elapsedTime;

            timers->fanOn_repInterval += elapsedTime;
            timers->coolOff_repInterval += elapsedTime;
            timers->heatOff_repInterval += elapsedTime;

            break;

        // Heat OFF/Fan ON implies that heat was recently on
        case HEAT_OFF_FAN_ON:
            timers->fanOff = 0;
            timers->coolOn = 0;
            timers->heatOn = 0;

            timers->fanOn += elapsedTime;
            timers->coolOff += elapsedTime;
            timers->heatOff += elapsedTime;

            timers->fanOn_repInterval += elapsedTime;
            timers->coolOff_repInterval += elapsedTime;
            timers->heatOff_repInterval += elapsedTime;

            break;

        // Fan ON implies that cool/heat are off and that the previous
        // usage of cool/heat are not known.
        case FAN_ON:
            timers->fanOff = 0;
            timers->coolOn = 0;
            timers->heatOn = 0;

            timers->fanOn += elapsedTime;
            timers->coolOff += elapsedTime;
            timers->heatOff += elapsedTime;

            timers->fanOn_repInterval += elapsedTime;
            timers->coolOff_repInterval += elapsedTime;
            timers->heatOff_repInterval += elapsedTime;

            break;

        // All OFF implies that cool/heat/fan are off
        case ALL_OFF:
            timers->fanOn = 0;
            timers->coolOn = 0;
            timers->heatOn = 0;

            timers->fanOff += elapsedTime;
            timers->coolOff += elapsedTime;
            timers->heatOff += elapsedTime;

            timers->fanOff_repInterval += elapsedTime;
            timers->coolOff_repInterval += elapsedTime;
            timers->heatOff_repInterval += elapsedTime;

            break;
    }

    // I/O watchdog timer handling; if ioWatchdogSeconds is zero
    // the timer is disabled.
    if (ioWatchdogSeconds != 0) {
        if ((currUptimeSeconds - ioWatchdogLastReset) > ioWatchdogSeconds) {
            // Ideally, the current state information would be saved now
            // before doing a reset.
            RESET();
        }
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
            break;

        case UPTIME_SEC0:
            retVal = (uint8_t) (uptimeSeconds & 0x000000ff);
            break;

        case UPTIME_SEC1:
            retVal = (uint8_t) ((uptimeSeconds & 0x0000ff00) >> 8);
            break;

        case UPTIME_SEC2:
            retVal = (uint8_t) ((uptimeSeconds & 0x00ff0000) >> 16);
            break;

        case IO_WD_SECONDS:
            retVal = (uint8_t) ioWatchdogSeconds;

        case FW_VER_MAJOR:
            retVal = (uint8_t) FIRMWARE_VERSION_MAJOR;
            break;

        case FW_VER_MINOR:
            retVal = (uint8_t) FIRMWARE_VERSION_MINOR;
            break;

        case REG_OSCCON:
            retVal = (uint8_t) OSCCON;
            break;

        default:
            retVal = 0;  // undefined address
    }

    return(retVal);
}


//
// Handle I2C write requests. This is called from the I2C ISR.
//
uint8_t i2c_write(uint8_t dataAddress, uint8_t dataByte) {
    uint8_t retVal = 0;

    switch((I2CDataAddress)dataAddress) {
        case IO_WD_SECONDS:
            ioWatchdogSeconds = dataByte;
            ioWatchdogLastReset = uptimeSeconds;
            break;
    }

    return(retVal);
}


void sendXbeeStatusReport(ACControlTimers *timers) {
    uint8_t state;
    static uint8_t sequenceNumber = 0;  // Report sequence counter
    static unsigned long lastUptimeSeconds = 0;
    unsigned long currUptimeSeconds, deltaUptimeSeconds;

    currUptimeSeconds = uptimeSeconds;  // Capture this now in case it updates
    if (lastUptimeSeconds == 0) lastUptimeSeconds = currUptimeSeconds;
    deltaUptimeSeconds = currUptimeSeconds - lastUptimeSeconds;

    xbee_status.sequenceNumber = sequenceNumber++;
    xbee_status.uptime = currUptimeSeconds;

    state = (systemLines.fan == 1) ? 1 : 0;
    state += (systemLines.cool == 1) ? 2 : 0;
    state += (systemLines.heat == 1) ? 4 : 0;
    xbee_status.systemState = state;

    state = (thermostatLines.fan == 1) ? 1 : 0;
    state += (thermostatLines.cool == 1) ? 2 : 0;
    state += (thermostatLines.heat == 1) ? 4 : 0;
    xbee_status.thermostatState = state;

    xbee_status.currentState = (uint8_t) currentState;
    xbee_status.targetState = (uint8_t) targetState;

    xbee_status.timer_fanOn = timers->fanOn;
    xbee_status.timer_fanOff = timers->fanOff;
    xbee_status.timer_coolOn = timers->coolOn;
    xbee_status.timer_coolOff = timers->coolOff;
    xbee_status.timer_heatOn = timers->heatOn;
    xbee_status.timer_heatOff = timers->heatOff;

    xbee_status.accum_fanOn = (timers->fanOn >= deltaUptimeSeconds) ? deltaUptimeSeconds : timers->fanOn_repInterval;
    xbee_status.accum_fanOff = (timers->fanOff >= deltaUptimeSeconds) ? deltaUptimeSeconds : timers->fanOff_repInterval;
    xbee_status.accum_coolOn = (timers->coolOn >= deltaUptimeSeconds) ? deltaUptimeSeconds : timers->coolOn_repInterval;
    xbee_status.accum_coolOff = (timers->coolOff >= deltaUptimeSeconds) ? deltaUptimeSeconds : timers->coolOff_repInterval;
    xbee_status.accum_heatOn = (timers->heatOn >= deltaUptimeSeconds) ? deltaUptimeSeconds : timers->heatOn_repInterval;
    xbee_status.accum_heatOff = (timers->heatOff >= deltaUptimeSeconds) ? deltaUptimeSeconds : timers->heatOff_repInterval;

    XBeePacket_APISendData(XBEE_DEST_MAC_HI, XBEE_DEST_MAC_LO, 0xfffe, sizeof(xbee_status), (uint8_t *)&xbee_status);

    // Reset the reporting interval counters
    timers->fanOn_repInterval = 0;
    timers->fanOff_repInterval = 0;
    timers->coolOn_repInterval = 0;
    timers->coolOff_repInterval = 0;
    timers->heatOn_repInterval = 0;
    timers->heatOff_repInterval = 0;

    lastUptimeSeconds = currUptimeSeconds;
}


void main(void) {
    ControlStateDescription newState;
    unsigned long currUptimeSeconds;

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

    // Get our node identifier from the XBee module
    strcpy(xbee_command_str, "NI");
    xbee_status.nodeName[0] = 0x00;
    if (XBeePacket_APISendATCommand(xbee_command_str, xbee_response_str) == 0) {
        // first char of response is a space; skip it
        strncpy(xbee_status.nodeName, xbee_response_str+1, XBEE_NI_MAXLEN);
        xbee_status.nodeName[XBEE_NI_MAXLEN] = 0x00;
    }

    // Set the oscillator correction factor based on the unit name
    if (strcmp("ACDOWN",xbee_status.nodeName) == 0) {
        timerCallbackPeriodNsec = NSEC_BETWEEN_TIMER_CALLBACKS_DOWN;
    } else {
        timerCallbackPeriodNsec = NSEC_BETWEEN_TIMER_CALLBACKS_UP;
    }

    // Set time for first status report
    xbee_nextReportTime = uptimeSeconds + XBEE_REPORT_INTERVAL0_SEC;
    // Populate static parts of status report
    xbee_status.packetType = 1;     // A/C controller packet type
    xbee_status.versionMajor = FIRMWARE_VERSION_MAJOR;
    xbee_status.versionMinor = FIRMWARE_VERSION_MINOR;

    while (1)     {
        CLRWDT();  // Clear the watchdog timer

        currUptimeSeconds = uptimeSeconds;  // Capture this now in case it updates
        updateTimers(&stateTimers, currentState); // Update all state counters

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

        // Enforce maximum run times and maximum all-off time
        if (currentState == targetState) {
            switch (currentState) {
                case COOL_ON:
                    if (stateTimers.coolOn > MAX_COOL_ON) {
                        targetState = COOL_OFF_FAN_ON;
                    }
                    break;

                case HEAT_ON:
                    if (stateTimers.heatOn > MAX_HEAT_ON) {
                        targetState == HEAT_OFF_FAN_ON;
                    }
                    break;

                case ALL_OFF:
                    if (stateTimers.fanOff > MAX_ALL_OFF_TO_FAN_ON) {
                        targetState == FAN_ON;
                    }
            }
        }

        // Evaluate state change from current to target state
        if (currentState != targetState) {
            newState = fsm_evaluate(fsm, currentState, targetState, &stateTimers);

            // If we reach an illegal state, reset the MCU
            if (newState == ILLEGAL) {
                RESET();
            }

            // If there is a new state, send it to the controller board
            if (currentState != newState) {
                setSystemLines(newState);
                currentState = newState;
            }
        }

        // Report status via XBee radio module
        if (currUptimeSeconds >= xbee_nextReportTime) {
            xbee_nextReportTime += XBEE_REPORT_INTERVAL_SEC;
            sendXbeeStatusReport(&stateTimers);
        }

        CLRWDT();  // Clear the watchdog timer
        sleep(SECONDS_BETWEEN_SAMPLES);
    }
}

//
// datatypes.h
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// Data types used by multiple modules.
//

#ifndef DATATYPES_H
#define	DATATYPES_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct {
    int fan;
    int cool;
    int heat;
} ACControlLines;


//
// Structure for tracking number of seconds since control state became true
// and the time stamp (uptime value) when that occurred.
//
typedef struct {
    unsigned long fanOn;
    unsigned long fanOff;
    unsigned long coolOn;
    unsigned long coolOff;
    unsigned long heatOn;
    unsigned long heatOff;
    unsigned long fanOn_repInterval;
    unsigned long fanOff_repInterval;
    unsigned long coolOn_repInterval;
    unsigned long coolOff_repInterval;
    unsigned long heatOn_repInterval;
    unsigned long heatOff_repInterval;
} ACControlTimers;


typedef enum {
    SYSTEM_STATE = 0,
    THERMOSTAT_STATE = 1,
    CURRENT_STATE = 2,
    TARGET_STATE = 3,
    LAST_COND_FUNCTION = 4,
    UPTIME_SEC0 = 5,
    UPTIME_SEC1 = 6,
    UPTIME_SEC2 = 7,
    REG_OSCCON = 8,
    IO_WD_SECONDS = 9,
    DATA_ADDR_END = 10,
    FW_VER_MAJOR = 254,
    FW_VER_MINOR = 255
} I2CDataAddress;


//
// Status package structure:
// XBee data payload structure (RF Data for frame type 0x10)
//
typedef struct {
    uint8_t packetType;
    uint8_t versionMajor;
    uint8_t versionMinor;
    uint8_t sequenceNumber;
    unsigned long uptime;
    uint8_t systemState;
    uint8_t thermostatState;
    uint8_t currentState;
    uint8_t targetState;
    unsigned long timer_fanOn;
    unsigned long timer_fanOff;
    unsigned long timer_coolOn;
    unsigned long timer_coolOff;
    unsigned long timer_heatOn;
    unsigned long timer_heatOff;
    unsigned long accum_fanOn;
    unsigned long accum_fanOff;
    unsigned long accum_coolOn;
    unsigned long accum_coolOff;
    unsigned long accum_heatOn;
    unsigned long accum_heatOff;
    char nodeName[21];
} XBeeStatusPayload;


#ifdef	__cplusplus
}
#endif

#endif

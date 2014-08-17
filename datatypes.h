//
// datatypes.h
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// Data types used by multiple modules.
//

#ifndef DATATYPES_H
#define	DATATYPES_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct {
    int fan;
    int cool;
    int heat;
} ACControlLines;


typedef struct {
    double fanOn;
    double fanOff;
    double coolOn;
    double coolOff;
    double heatOn;
    double heatOff;
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
    DATA_ADDR_END = 8,
    FW_VER_MAJOR = 254,
    FW_VER_MINOR = 255
} I2CDataAddress;


#ifdef	__cplusplus
}
#endif

#endif

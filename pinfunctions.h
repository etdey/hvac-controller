//
// pinfunctions.h
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// Defines the pin out/in functions
//

#ifndef PINFUNCTIONS_H
#define	PINFUNCTIONS_H

#include "mcc_generated_files/mcc.h"

#ifdef	__cplusplus
extern "C" {
#endif


#define THERMOSTAT_FAN      PORTCbits.RC7
#define THERMOSTAT_COOL     PORTCbits.RC6
#define THERMOSTAT_HEAT     PORTCbits.RC5

#define SYSTEM_FAN          LATBbits.LATB2
#define SYSTEM_COOL         LATBbits.LATB1
#define SYSTEM_HEAT         LATBbits.LATB0


#ifdef	__cplusplus
}
#endif

#endif


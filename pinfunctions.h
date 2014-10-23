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


/* Input lines for version 1.x boards
#define THERMOSTAT_FAN      PORTCbits.RC7
#define THERMOSTAT_COOL     PORTCbits.RC6
#define THERMOSTAT_HEAT     PORTCbits.RC5
*/
#define THERMOSTAT_FAN      PORTAbits.RA0
#define THERMOSTAT_COOL     PORTAbits.RA1
#define THERMOSTAT_HEAT     PORTAbits.RA2

#define SYSTEM_FAN          LATBbits.LATB2
#define SYSTEM_COOL         LATBbits.LATB1
#define SYSTEM_HEAT         LATBbits.LATB0

#define EUART_DTR           PORTCbits.RC5
#define EUART_CTS           LATCbits.LATC2

#define CIRCUIT_HEARTBEAT   LATAbits.LATA3
#define CIRCUIT_STATUS      LATAbits.LATA4

#ifdef	__cplusplus
}
#endif

#endif


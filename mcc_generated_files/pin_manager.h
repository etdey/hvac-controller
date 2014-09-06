/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using MPLAB® Code Configurator

  @Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.0.1
        Device            :  PIC18F25K20
        Version           :  1.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 v1.31
        MPLAB             :  MPLAB X 2.10
*/

/*
Copyright (c) 2013 - 2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_RA0 aliases
#define IO_RA0_TRIS               TRISA0
#define IO_RA0_LAT                LATA0
#define IO_RA0_PORT               PORTAbits.RA0
#define IO_RA0_ANS                ANS0
#define IO_RA0_SetHigh()    do { LATA0 = 1; } while(0)
#define IO_RA0_SetLow()   do { LATA0 = 0; } while(0)
#define IO_RA0_Toggle()   do { LATA0 = ~LATA0; } while(0)
#define IO_RA0_GetValue()         PORTAbits.RA0
#define IO_RA0_SetDigitalInput()    do { TRISA0 = 1; } while(0)
#define IO_RA0_SetDigitalOutput()   do { TRISA0 = 0; } while(0)

#define IO_RA0_SetAnalogMode()   do { ANS0 = 1; } while(0)
#define IO_RA0_SetDigitalMode()   do { ANS0 = 0; } while(0)
// get/set IO_RA1 aliases
#define IO_RA1_TRIS               TRISA1
#define IO_RA1_LAT                LATA1
#define IO_RA1_PORT               PORTAbits.RA1
#define IO_RA1_ANS                ANS1
#define IO_RA1_SetHigh()    do { LATA1 = 1; } while(0)
#define IO_RA1_SetLow()   do { LATA1 = 0; } while(0)
#define IO_RA1_Toggle()   do { LATA1 = ~LATA1; } while(0)
#define IO_RA1_GetValue()         PORTAbits.RA1
#define IO_RA1_SetDigitalInput()    do { TRISA1 = 1; } while(0)
#define IO_RA1_SetDigitalOutput()   do { TRISA1 = 0; } while(0)

#define IO_RA1_SetAnalogMode()   do { ANS1 = 1; } while(0)
#define IO_RA1_SetDigitalMode()   do { ANS1 = 0; } while(0)
// get/set IO_RA2 aliases
#define IO_RA2_TRIS               TRISA2
#define IO_RA2_LAT                LATA2
#define IO_RA2_PORT               PORTAbits.RA2
#define IO_RA2_ANS                ANS2
#define IO_RA2_SetHigh()    do { LATA2 = 1; } while(0)
#define IO_RA2_SetLow()   do { LATA2 = 0; } while(0)
#define IO_RA2_Toggle()   do { LATA2 = ~LATA2; } while(0)
#define IO_RA2_GetValue()         PORTAbits.RA2
#define IO_RA2_SetDigitalInput()    do { TRISA2 = 1; } while(0)
#define IO_RA2_SetDigitalOutput()   do { TRISA2 = 0; } while(0)

#define IO_RA2_SetAnalogMode()   do { ANS2 = 1; } while(0)
#define IO_RA2_SetDigitalMode()   do { ANS2 = 0; } while(0)
// get/set RA3 procedures
#define RA3_SetHigh()    do { LATA3 = 1; } while(0)
#define RA3_SetLow()   do { LATA3 = 0; } while(0)
#define RA3_Toggle()   do { LATA3 = ~LATA3; } while(0)
#define RA3_GetValue()         PORTAbits.RA3
#define RA3_SetDigitalInput()    do { TRISA3 = 1; } while(0)
#define RA3_SetDigitalOutput()   do { TRISA3 = 0; } while(0)
#define RA3_SetAnalogMode()   do { ANS3 = 1; } while(0)
#define RA3_SetDigitalMode()   do { ANS3 = 0; } while(0)
// get/set RA4 procedures
#define RA4_SetHigh()    do { LATA4 = 1; } while(0)
#define RA4_SetLow()   do { LATA4 = 0; } while(0)
#define RA4_Toggle()   do { LATA4 = ~LATA4; } while(0)
#define RA4_GetValue()         PORTAbits.RA4
#define RA4_SetDigitalInput()    do { TRISA4 = 1; } while(0)
#define RA4_SetDigitalOutput()   do { TRISA4 = 0; } while(0)
// get/set RA5 procedures
#define RA5_SetHigh()    do { LATA5 = 1; } while(0)
#define RA5_SetLow()   do { LATA5 = 0; } while(0)
#define RA5_Toggle()   do { LATA5 = ~LATA5; } while(0)
#define RA5_GetValue()         PORTAbits.RA5
#define RA5_SetDigitalInput()    do { TRISA5 = 1; } while(0)
#define RA5_SetDigitalOutput()   do { TRISA5 = 0; } while(0)
#define RA5_SetAnalogMode()   do { ANS4 = 1; } while(0)
#define RA5_SetDigitalMode()   do { ANS4 = 0; } while(0)
// get/set OSC2 aliases
#define OSC2_TRIS               TRISA6
#define OSC2_LAT                LATA6
#define OSC2_PORT               PORTAbits.RA6
#define OSC2_SetHigh()    do { LATA6 = 1; } while(0)
#define OSC2_SetLow()   do { LATA6 = 0; } while(0)
#define OSC2_Toggle()   do { LATA6 = ~LATA6; } while(0)
#define OSC2_GetValue()         PORTAbits.RA6
#define OSC2_SetDigitalInput()    do { TRISA6 = 1; } while(0)
#define OSC2_SetDigitalOutput()   do { TRISA6 = 0; } while(0)

// get/set OSC1 aliases
#define OSC1_TRIS               TRISA7
#define OSC1_LAT                LATA7
#define OSC1_PORT               PORTAbits.RA7
#define OSC1_SetHigh()    do { LATA7 = 1; } while(0)
#define OSC1_SetLow()   do { LATA7 = 0; } while(0)
#define OSC1_Toggle()   do { LATA7 = ~LATA7; } while(0)
#define OSC1_GetValue()         PORTAbits.RA7
#define OSC1_SetDigitalInput()    do { TRISA7 = 1; } while(0)
#define OSC1_SetDigitalOutput()   do { TRISA7 = 0; } while(0)

// get/set IO_RB0 aliases
#define IO_RB0_TRIS               TRISB0
#define IO_RB0_LAT                LATB0
#define IO_RB0_PORT               PORTBbits.RB0
#define IO_RB0_WPU                WPUB0
#define IO_RB0_ANS                ANS12
#define IO_RB0_SetHigh()    do { LATB0 = 1; } while(0)
#define IO_RB0_SetLow()   do { LATB0 = 0; } while(0)
#define IO_RB0_Toggle()   do { LATB0 = ~LATB0; } while(0)
#define IO_RB0_GetValue()         PORTBbits.RB0
#define IO_RB0_SetDigitalInput()    do { TRISB0 = 1; } while(0)
#define IO_RB0_SetDigitalOutput()   do { TRISB0 = 0; } while(0)

#define IO_RB0_SetPullup()    do { WPUB0 = 1; } while(0)
#define IO_RB0_ResetPullup()   do { WPUB0 = 0; } while(0)
#define IO_RB0_SetAnalogMode()   do { ANS12 = 1; } while(0)
#define IO_RB0_SetDigitalMode()   do { ANS12 = 0; } while(0)
// get/set IO_RB1 aliases
#define IO_RB1_TRIS               TRISB1
#define IO_RB1_LAT                LATB1
#define IO_RB1_PORT               PORTBbits.RB1
#define IO_RB1_WPU                WPUB1
#define IO_RB1_ANS                ANS10
#define IO_RB1_SetHigh()    do { LATB1 = 1; } while(0)
#define IO_RB1_SetLow()   do { LATB1 = 0; } while(0)
#define IO_RB1_Toggle()   do { LATB1 = ~LATB1; } while(0)
#define IO_RB1_GetValue()         PORTBbits.RB1
#define IO_RB1_SetDigitalInput()    do { TRISB1 = 1; } while(0)
#define IO_RB1_SetDigitalOutput()   do { TRISB1 = 0; } while(0)

#define IO_RB1_SetPullup()    do { WPUB1 = 1; } while(0)
#define IO_RB1_ResetPullup()   do { WPUB1 = 0; } while(0)
#define IO_RB1_SetAnalogMode()   do { ANS10 = 1; } while(0)
#define IO_RB1_SetDigitalMode()   do { ANS10 = 0; } while(0)
// get/set IO_RB2 aliases
#define IO_RB2_TRIS               TRISB2
#define IO_RB2_LAT                LATB2
#define IO_RB2_PORT               PORTBbits.RB2
#define IO_RB2_WPU                WPUB2
#define IO_RB2_ANS                ANS8
#define IO_RB2_SetHigh()    do { LATB2 = 1; } while(0)
#define IO_RB2_SetLow()   do { LATB2 = 0; } while(0)
#define IO_RB2_Toggle()   do { LATB2 = ~LATB2; } while(0)
#define IO_RB2_GetValue()         PORTBbits.RB2
#define IO_RB2_SetDigitalInput()    do { TRISB2 = 1; } while(0)
#define IO_RB2_SetDigitalOutput()   do { TRISB2 = 0; } while(0)

#define IO_RB2_SetPullup()    do { WPUB2 = 1; } while(0)
#define IO_RB2_ResetPullup()   do { WPUB2 = 0; } while(0)
#define IO_RB2_SetAnalogMode()   do { ANS8 = 1; } while(0)
#define IO_RB2_SetDigitalMode()   do { ANS8 = 0; } while(0)
// get/set RB3 procedures
#define RB3_SetHigh()    do { LATB3 = 1; } while(0)
#define RB3_SetLow()   do { LATB3 = 0; } while(0)
#define RB3_Toggle()   do { LATB3 = ~LATB3; } while(0)
#define RB3_GetValue()         PORTBbits.RB3
#define RB3_SetDigitalInput()    do { TRISB3 = 1; } while(0)
#define RB3_SetDigitalOutput()   do { TRISB3 = 0; } while(0)
#define RB3_SetPullup()    do { WPUB3 = 1; } while(0)
#define RB3_ResetPullup()   do { WPUB3 = 0; } while(0)
#define RB3_SetAnalogMode()   do { ANS9 = 1; } while(0)
#define RB3_SetDigitalMode()   do { ANS9 = 0; } while(0)
// get/set RB4 procedures
#define RB4_SetHigh()    do { LATB4 = 1; } while(0)
#define RB4_SetLow()   do { LATB4 = 0; } while(0)
#define RB4_Toggle()   do { LATB4 = ~LATB4; } while(0)
#define RB4_GetValue()         PORTBbits.RB4
#define RB4_SetDigitalInput()    do { TRISB4 = 1; } while(0)
#define RB4_SetDigitalOutput()   do { TRISB4 = 0; } while(0)
#define RB4_SetPullup()    do { WPUB4 = 1; } while(0)
#define RB4_ResetPullup()   do { WPUB4 = 0; } while(0)
#define RB4_SetAnalogMode()   do { ANS11 = 1; } while(0)
#define RB4_SetDigitalMode()   do { ANS11 = 0; } while(0)
// get/set RB5 procedures
#define RB5_SetHigh()    do { LATB5 = 1; } while(0)
#define RB5_SetLow()   do { LATB5 = 0; } while(0)
#define RB5_Toggle()   do { LATB5 = ~LATB5; } while(0)
#define RB5_GetValue()         PORTBbits.RB5
#define RB5_SetDigitalInput()    do { TRISB5 = 1; } while(0)
#define RB5_SetDigitalOutput()   do { TRISB5 = 0; } while(0)
#define RB5_SetPullup()    do { WPUB5 = 1; } while(0)
#define RB5_ResetPullup()   do { WPUB5 = 0; } while(0)
// get/set RB6 procedures
#define RB6_SetHigh()    do { LATB6 = 1; } while(0)
#define RB6_SetLow()   do { LATB6 = 0; } while(0)
#define RB6_Toggle()   do { LATB6 = ~LATB6; } while(0)
#define RB6_GetValue()         PORTBbits.RB6
#define RB6_SetDigitalInput()    do { TRISB6 = 1; } while(0)
#define RB6_SetDigitalOutput()   do { TRISB6 = 0; } while(0)
#define RB6_SetPullup()    do { WPUB6 = 1; } while(0)
#define RB6_ResetPullup()   do { WPUB6 = 0; } while(0)
// get/set RB7 procedures
#define RB7_SetHigh()    do { LATB7 = 1; } while(0)
#define RB7_SetLow()   do { LATB7 = 0; } while(0)
#define RB7_Toggle()   do { LATB7 = ~LATB7; } while(0)
#define RB7_GetValue()         PORTBbits.RB7
#define RB7_SetDigitalInput()    do { TRISB7 = 1; } while(0)
#define RB7_SetDigitalOutput()   do { TRISB7 = 0; } while(0)
#define RB7_SetPullup()    do { WPUB7 = 1; } while(0)
#define RB7_ResetPullup()   do { WPUB7 = 0; } while(0)
// get/set RC0 procedures
#define RC0_SetHigh()    do { LATC0 = 1; } while(0)
#define RC0_SetLow()   do { LATC0 = 0; } while(0)
#define RC0_Toggle()   do { LATC0 = ~LATC0; } while(0)
#define RC0_GetValue()         PORTCbits.RC0
#define RC0_SetDigitalInput()    do { TRISC0 = 1; } while(0)
#define RC0_SetDigitalOutput()   do { TRISC0 = 0; } while(0)
// get/set RC1 procedures
#define RC1_SetHigh()    do { LATC1 = 1; } while(0)
#define RC1_SetLow()   do { LATC1 = 0; } while(0)
#define RC1_Toggle()   do { LATC1 = ~LATC1; } while(0)
#define RC1_GetValue()         PORTCbits.RC1
#define RC1_SetDigitalInput()    do { TRISC1 = 1; } while(0)
#define RC1_SetDigitalOutput()   do { TRISC1 = 0; } while(0)
// get/set RC2 procedures
#define RC2_SetHigh()    do { LATC2 = 1; } while(0)
#define RC2_SetLow()   do { LATC2 = 0; } while(0)
#define RC2_Toggle()   do { LATC2 = ~LATC2; } while(0)
#define RC2_GetValue()         PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISC2 = 0; } while(0)
// get/set SCL aliases
#define SCL_TRIS               TRISC3
#define SCL_LAT                LATC3
#define SCL_PORT               PORTCbits.RC3
#define SCL_SetHigh()    do { LATC3 = 1; } while(0)
#define SCL_SetLow()   do { LATC3 = 0; } while(0)
#define SCL_Toggle()   do { LATC3 = ~LATC3; } while(0)
#define SCL_GetValue()         PORTCbits.RC3
#define SCL_SetDigitalInput()    do { TRISC3 = 1; } while(0)
#define SCL_SetDigitalOutput()   do { TRISC3 = 0; } while(0)

// get/set SDA aliases
#define SDA_TRIS               TRISC4
#define SDA_LAT                LATC4
#define SDA_PORT               PORTCbits.RC4
#define SDA_SetHigh()    do { LATC4 = 1; } while(0)
#define SDA_SetLow()   do { LATC4 = 0; } while(0)
#define SDA_Toggle()   do { LATC4 = ~LATC4; } while(0)
#define SDA_GetValue()         PORTCbits.RC4
#define SDA_SetDigitalInput()    do { TRISC4 = 1; } while(0)
#define SDA_SetDigitalOutput()   do { TRISC4 = 0; } while(0)

// get/set IO_RC5 aliases
#define IO_RC5_TRIS               TRISC5
#define IO_RC5_LAT                LATC5
#define IO_RC5_PORT               PORTCbits.RC5
#define IO_RC5_SetHigh()    do { LATC5 = 1; } while(0)
#define IO_RC5_SetLow()   do { LATC5 = 0; } while(0)
#define IO_RC5_Toggle()   do { LATC5 = ~LATC5; } while(0)
#define IO_RC5_GetValue()         PORTCbits.RC5
#define IO_RC5_SetDigitalInput()    do { TRISC5 = 1; } while(0)
#define IO_RC5_SetDigitalOutput()   do { TRISC5 = 0; } while(0)

// get/set IO_RC6 aliases
#define IO_RC6_TRIS               TRISC6
#define IO_RC6_LAT                LATC6
#define IO_RC6_PORT               PORTCbits.RC6
#define IO_RC6_SetHigh()    do { LATC6 = 1; } while(0)
#define IO_RC6_SetLow()   do { LATC6 = 0; } while(0)
#define IO_RC6_Toggle()   do { LATC6 = ~LATC6; } while(0)
#define IO_RC6_GetValue()         PORTCbits.RC6
#define IO_RC6_SetDigitalInput()    do { TRISC6 = 1; } while(0)
#define IO_RC6_SetDigitalOutput()   do { TRISC6 = 0; } while(0)

// get/set IO_RC7 aliases
#define IO_RC7_TRIS               TRISC7
#define IO_RC7_LAT                LATC7
#define IO_RC7_PORT               PORTCbits.RC7
#define IO_RC7_SetHigh()    do { LATC7 = 1; } while(0)
#define IO_RC7_SetLow()   do { LATC7 = 0; } while(0)
#define IO_RC7_Toggle()   do { LATC7 = ~LATC7; } while(0)
#define IO_RC7_GetValue()         PORTCbits.RC7
#define IO_RC7_SetDigitalInput()    do { TRISC7 = 1; } while(0)
#define IO_RC7_SetDigitalOutput()   do { TRISC7 = 0; } while(0)

// get/set MCLR aliases
#define MCLR_PORT               PORTEbits.RE3
#define MCLR_GetValue()         PORTEbits.RE3


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    GPIO and peripheral I/O initialization
 * @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);

#endif // PIN_MANAGER_H
/**
 End of File
*/
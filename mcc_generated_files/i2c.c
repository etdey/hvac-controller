//
// i2c.c
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
//


#include "mcc.h"

#define I2C_SLAVE_ADDRESS 24
#define I2C_SLAVE_MASK    127


// Prototypes from main module
uint8_t i2c_read(uint8_t);
uint8_t i2c_write(uint8_t, uint8_t);


typedef enum
{
    SLAVE_NORMAL_DATA,
    SLAVE_DATA_ADDRESS,
} SLAVE_WRITE_DATA_TYPE;

/**
 Section: Global Variables
*/

volatile uint8_t    I2C_slaveWriteData      = 0x55;

/**
 Section: Local Functions
*/
void I2C_StatusCallback(I2C_SLAVE_DRIVER_STATUS i2c_bus_state);



/**
  Prototype:        void I2C_Initialize(void)
  Input:            none
  Output:           none
  Description:      I2C_Initialize is an
                    initialization routine that takes inputs from the GUI.
  Comment:          
  Usage:            I2C_Initialize();

*/
void I2C_Initialize(void)
{
    // initialize the hardware
    // BF RCinprocess_TXcomplete; UA dontupdate; SMP Standard Speed; P stopbit_notdetected; S startbit_notdetected; R_nW write_noTX; CKE Idle to Active; D_nA lastbyte_address; 
    SSPSTAT = 0x80;
    // SSPEN enabled; WCOL no_collision; SSPOV no_overflow; CKP Idle:Low, Active:High; SSPM 7 Bit; 
    SSPCON1 = 0x26;
    // ACKSTAT received; RCEN disabled; RSEN disabled; ACKEN disabled; SEN disabled; ACKDT acknowledge; GCEN disabled; PEN disabled; 
    SSPCON2 = 0x00;
    // MSK0 127; 
    SSPMSK = (I2C_SLAVE_MASK << 1);  // adjust UI mask for R/nW bit            
    // SSPADD 24; 
    SSPADD = (I2C_SLAVE_ADDRESS << 1);  // adjust UI address for R/nW bit

    // clear the slave interrupt flag
    PIR1bits.SSPIF = 0;
    // enable the master interrupt
    PIE1bits.SSPIE = 1;

}

void I2C_ISR ( void )
{
    uint8_t     i2c_data                = 0x55;


    // NOTE: The slave driver will always acknowledge
    //       any address match.

    PIR1bits.SSPIF = 0;        // clear the slave interrupt flag
    i2c_data        = SSPBUF;  // read SSPBUF to clear BF
    if(1 == SSPSTATbits.R_nW)
    {
        if((1 == SSPSTATbits.D_nA) && (1 == PORTCbits.RC4))
        {
            // callback routine can perform any post-read processing
            I2C_StatusCallback(I2C_SLAVE_READ_COMPLETED);
        }
        else
        {
            // callback routine should write data into SSPBUF
            I2C_StatusCallback(I2C_SLAVE_READ_REQUEST);
        }
    }
    else if(0 == SSPSTATbits.D_nA)
    {
        // this is an I2C address

        // callback routine should prepare to receive data from the master
        I2C_StatusCallback(I2C_SLAVE_WRITE_REQUEST);
    }
    else
    {
        I2C_slaveWriteData   = i2c_data;

        // callback routine should process I2C_slaveWriteData from the master
        I2C_StatusCallback(I2C_SLAVE_WRITE_COMPLETED);
    }

    SSPCON1bits.CKP    = 1;    // release SCL

} // end I2C_ISR()



void I2C_StatusCallback(I2C_SLAVE_DRIVER_STATUS i2c_bus_state)
{
    static uint8_t dataAddress    = 0;
    static uint8_t slaveWriteType   = SLAVE_NORMAL_DATA;


    switch (i2c_bus_state)
    {
        case I2C_SLAVE_WRITE_REQUEST:
            // the master will be sending the eeprom address next
            slaveWriteType  = SLAVE_DATA_ADDRESS;
            break;


        case I2C_SLAVE_WRITE_COMPLETED:

            switch(slaveWriteType)
            {
                case SLAVE_DATA_ADDRESS:
                    dataAddress   = I2C_slaveWriteData;
                    break;

                case SLAVE_NORMAL_DATA:
                    // Process the data write request in the main module
                    SSPBUF = i2c_write(dataAddress++, I2C_slaveWriteData);
                    break;

                default:
                    break;

            } // end switch(slaveWriteType)

            slaveWriteType  = SLAVE_NORMAL_DATA;
            break;

        case I2C_SLAVE_READ_REQUEST:
            // Process the data read request in the main module
            SSPBUF = i2c_read(dataAddress++);
            if (dataAddress >= 255) dataAddress = 0;
            break;

        case I2C_SLAVE_READ_COMPLETED:
        default:
            break;

    } // end switch(i2c_bus_state)

}


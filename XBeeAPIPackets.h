//
// XBeeAPIPackets.h
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
//

#ifndef XBEEAPIPACKETS_H
#define	XBEEAPIPACKETS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define XBEEAPIPACKETS_SEND_QUEUE_DEPTH         2       // Must be >= 2
#define XBEEAPIPACKETS_RECV_QUEUE_DEPTH         2       // Must be >= 2
#define XBEEAPIPACKETS_DATA_BUFFER_MAX_SIZE     256


//typedef enum {
//    AT_COMMAND_REQUEST = 0x08,
//    TRANSMIT_DATA_PACKET = 0x10,
//    TRANSMIT_DATA_PACKET_EXPLICIT = 0x11,
//    AT_COMMAND_RESPONSE = 0x88,
//    MODEM_STATUS = 0x8a,
//    TRANSMIT_STATUS = 0x8b,
//    RECEIVE_DATA_PACKET = 0x90,
//    RECEIVE_DATA_PACKET_EXPLICIT = 0x91,
//    REMOTE_COMMAND_RESPONSE = 0x97
//} XBeeFrameType;


typedef struct {
    int length;
    uint8_t data[XBEEAPIPACKETS_DATA_BUFFER_MAX_SIZE+1];
} XBeeAPIPacket;


int XBeePacket_APISendData(unsigned long dest64hi, unsigned long dest64lo, unsigned short dest16, unsigned int dataLength, uint8_t *data);
int XBeePacket_APISendATCommand(char *inputStr, char *outputStr);


#ifdef	__cplusplus
}
#endif

#endif	/* XBEEAPIPACKETS_H */


//
// XBeeAPIPackets.c
//
// Copyright (C) 2014 by Eric Dey. All rights reserved.
//
// Implements encoding/decoding of the XBee API mode packets.
//
#include <stdlib.h>
#include <string.h>

#include "mcc_generated_files/eusart.h"
#include "XBeeAPIPackets.h"


// Current packet buffer of one. If sending and receiving every become
// asynchronous with buffers, this needs to be changed.
XBeeAPIPacket XBeePacket_current_packet;
uint8_t XBeePacket_current_id = 1;


int XBeePacket_SendFrame(XBeeAPIPacket *packet) {
    int n, frameLength;
    uint8_t checksum=0;

    EUSART_Write(0x7e);  // start character

    frameLength = packet->length;
    EUSART_Write((frameLength & 0xff00) >> 8);  // MSB length of frame
    EUSART_Write(frameLength & 0xff);           // LSB length of frame

    // Write the frame-specific data bytes
    for (n=0; n < frameLength; n++) {
        EUSART_Write(packet->data[n]);
        checksum += packet->data[n];
    }

    EUSART_Write(0xff - (checksum & 0xff));     // frame checksum

    return(0);
}


int XBeePacket_ReceiveFrame(XBeeAPIPacket *packet) {
    unsigned int n, frameLength;
    uint8_t d=0, checksum=0;

    // Read bytes until the frame start byte is found
    while (d != 0x7e) {
        d = EUSART_Read();
    }

    frameLength = ((int) EUSART_Read() << 8) & 0xff00;   // MSB length of data
    frameLength += (int) EUSART_Read() & 0xff;           // LSB length of data
    packet->length = frameLength;

    // Read the frame-specific data bytes
    for (n=0; n < frameLength; n++) {
        d = EUSART_Read();
        packet->data[n] = d;
        checksum += d;
    }

    checksum = 0xff - checksum;
    d = EUSART_Read();
    if (checksum != d) {
        return(1);       // Invalid frame because checksum mismatch
    }

    return(0);
}


// Send API Transmit Request (0x10)
int XBeePacket_APISendData(unsigned long dest64hi, unsigned long dest64lo, unsigned short dest16, unsigned int dataLength, uint8_t *data) {
    XBeeAPIPacket *packet = &XBeePacket_current_packet;
    unsigned int apiFrameLength;
    uint8_t *apiFrame;

    apiFrameLength = 14 + dataLength;  // 1+1+8+2+1+1+dataLength
    if (apiFrameLength > XBEEAPIPACKETS_DATA_BUFFER_MAX_SIZE) return(1);
    packet->length = apiFrameLength;

    // Build the frame-specific data
    apiFrame = packet->data;
    apiFrame[0]  = 0x10;  // Frame Type
    apiFrame[1]  = 0x00;  // Frame ID
    apiFrame[2]  = (dest64hi & 0xff000000) >> 24;  // 64-bit destination MSB
    apiFrame[3]  = (dest64hi & 0x00ff0000) >> 16;
    apiFrame[4]  = (dest64hi & 0x0000ff00) >> 8;
    apiFrame[5]  = (dest64hi & 0x000000ff);
    apiFrame[6]  = (dest64lo & 0xff000000) >> 24;
    apiFrame[7]  = (dest64lo & 0x00ff0000) >> 16;
    apiFrame[8]  = (dest64lo & 0x0000ff00) >> 8;
    apiFrame[9]  = (dest64lo & 0x000000ff);        // 64-bit destination LSB
    apiFrame[10]  = (dest16 & 0xff00) >> 8;         // 16-bit destination MSB
    apiFrame[11] = (dest16 & 0x00ff);              // 16-bit destination LSB
    apiFrame[12] = 0x00;  // Broadcast radius
    apiFrame[13] = 0x00;  // Options
    memcpy(apiFrame+14, data, dataLength);  // RF Data

    return(XBeePacket_SendFrame(packet));
}


// Send API AT Command (0x08) and receive response
// @inputStr the text of the AT command following the "AT" characters
// @outputStr set to NULL for no response, otherwise allocate space for response
//
// NOTE: This is not yet safe for multi-frame responses.
//
int XBeePacket_APISendATCommand(char *inputStr, char *outputStr) {
    XBeeAPIPacket *packet = &XBeePacket_current_packet;
    unsigned int apiFrameLength;
    uint8_t *apiFrame;
    uint8_t frameId=0;
    int retVal;

    if (outputStr != NULL) {
        frameId = XBeePacket_current_id++;
        if (XBeePacket_current_id == 0) XBeePacket_current_id++;
    }

    apiFrameLength = 2 + strlen(inputStr);  // 1+1+inputLength
    if (apiFrameLength > XBEEAPIPACKETS_DATA_BUFFER_MAX_SIZE) return(1);
    packet->length = apiFrameLength;

    // Build the frame-specific data
    apiFrame = packet->data;
    apiFrame[0]  = 0x08;  // Frame Type
    apiFrame[1]  = frameId;  // Frame ID
    memcpy(apiFrame+2, inputStr, strlen(inputStr));  // AT command string

    retVal = XBeePacket_SendFrame(packet);
    if (frameId == 0) return(retVal);    // Return now if no response requested

    // Receive the response packet
    while(1) {
        retVal = XBeePacket_ReceiveFrame(packet);
        if (retVal != 0) return(retVal);

        apiFrame = packet->data;
        apiFrameLength = packet->length;

        if (apiFrame[0] != 0x88) continue;  // Not the response frame
        if (apiFrame[1] != frameId) {
            if (apiFrame[1] > frameId) return(1);  // Received future frame
            if (apiFrame[1] < (frameId - 128)) {
                // Consume older frame IDs unless they are too far back.
                continue;
            } else {
                // Too far back means it wrapped. This is really a future frame.
                return(1);
            }
        }
        break;  // received expected frame, break out of receive loop
    }

    // There are 5 bytes prior to the start of the optional command data.
    // If the packet length is greater than 5, then copy the extra data.
    if (packet->length > 5) {
        memcpy(outputStr, apiFrame+5, packet->length - 5);
    }
    outputStr[packet->length - 5] = 0x00;    // Null terminate the string
    retVal = (int) apiFrame[4];

    return(retVal);
}

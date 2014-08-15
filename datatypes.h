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


#ifdef	__cplusplus
}
#endif

#endif

#!/usr/bin/env python
#
# data_loader.py
#
"""
A/C Controller Data Loader

Copyright (C) 2014 by Eric Dey. All rights reserved.


"""

import sys, datetime, csv, json
#import influxdb


ControlStates = [
    "INITIAL",
    "ALL_OFF",
    "FAN_ON",
    "COOL_ON",
    "HEAT_ON",
    "COOL_OFF_FAN_ON",
    "HEAT_OFF_FAN_ON",
]

def str2bool(s):
    if [s == "1"]:
        return True
    else:
        return False

sys.stdout.write("[")
points = []

with sys.stdin as inputFile:
    reader = csv.reader(inputFile, delimiter=' ', quoting=csv.QUOTE_NONE)
    for row in reader:
        if len(row) < 14:
            continue
        
        (strDate, strTime, sbit2, sbit1, sbit0, tbit2, tbit1, tbit0, csbit2, csbit1, csbit0, tsbit2, tsbit1, tsbit0) = row[:14]
        if len(row) > 14:
            strUptime = row[14]
        else:
            strUptime = '0'
        
        # WARNING: This is not DST safe
        sampleTime = datetime.datetime.strptime("%s %s" % (strDate, strTime), '%m/%d/%Y %H:%M:%S')
        timestamp = int(sampleTime.strftime("%s")) * 1000      # microsecond epoch
        
        sysfan = str2bool(sbit0)
        syscool = str2bool(sbit1)
        sysheat = str2bool(sbit2)
        
        tstatfan = str2bool(tsbit0)
        tstatcool = str2bool(tsbit1)
        tstatheat = str2bool(tsbit2)
        
        currentState = ControlStates[int("%s%s%s" % (csbit2, csbit1, csbit0),2)]
        targetState = ControlStates[int("%s%s%s" % (tsbit2, tsbit1, tsbit0),2)]
        
        uptime = int(strUptime, 16)
        
        """
        rec = {
            "name" : "downstairs",
            "columns" : ["time", "sysheat", "syscool", "sysfan", "tstatheat", "tstatcool", "tstatfan", "uptime"],
            "points" : [[timestamp, sbit2, sbit1, sbit0, tsbit2, tsbit1, tsbit0, uptime]],
        }
        """
        """
        rec = {
            "name" : "unit0",
            "columns" : ["time", "sysheat", "syscool", "sysfan", "tstatheat", "tstatcool", "tstatfan", "uptime"],
            "points" : [[timestamp, sysheat, syscool, sysfan, tstatheat, tstatcool, tstatfan, uptime]],
        }
        """
        points.append([timestamp, int(sbit2)*60, int(sbit1)*60, int(sbit0)*60, int(tsbit2)*60, int(tsbit1)*60, int(tsbit0)*60, uptime])
        
rec = {
    "name" : "downstairs",
    "columns" : ["time", "sysheat", "syscool", "sysfan", "tstatheat", "tstatcool", "tstatfan", "uptime"],
    "points" : points,
}
json.dump(rec, sys.stdout)
sys.stdout.write("]\n")
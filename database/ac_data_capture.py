#!/usr/bin/env python
#
# ac_data_capture.py
#
"""
A/C Controller Data Capture

Copyright (C) 2014 by Eric Dey. All rights reserved.

This module reads data from I2C and uploads it to an InfluxDB server.

"""

import datetime, time, json, smbus
import influxdb


SAMPLE_PERIOD_SEC = 60.0     # Time in seconds between samples
SAMPLE_PERIOD_SEC_FIRST = 5.0

DBHOST = 'influxdb.tx.deys.org'
DBPORT = 8086
DBUSER = 'root'
DBPASS = 'root'
DBNAME = 'aircontrol'
DBSERIES = 'downstairs'
DBRETRIES = 5


ControlStates = [
    "INITIAL",
    "ALL_OFF",
    "FAN_ON",
    "COOL_ON",
    "HEAT_ON",
    "COOL_OFF_FAN_ON",
    "HEAT_OFF_FAN_ON",
]

I2CDataAddresses = {
    "SYSTEM_STATE" : 0,
    "THERMOSTAT_STATE" : 1,
    "CURRENT_STATE" : 2,
    "TARGET_STATE" : 3,
    "LAST_COND_FUNCTION" : 4,
    "UPTIME_SEC0" : 5,
    "UPTIME_SEC1" : 6,
    "UPTIME_SEC2" : 7,
    "REG_OSCCON" : 8,
    "DATA_ADDR_END" : 9,
    "FW_VER_MAJOR" : 254,
    "FW_VER_MINOR" : 255
}


class ACController(object):
    
    def __init__(self, busNumber=0, busAddress=0x18):
        self.busNumber = busNumber
        self.busAddress = busAddress
        
        self.device = smbus.SMBus(busNumber)
        self.lastTimestamp = None


    def _read(self, regAddress, retries=3):
        """read single byte from i2c"""
        while retries > 0:
            try:
                return(self.device.read_byte_data(self.busAddress, regAddress))
            except:
                retries -= 1
        return(None)
    
    
    def firmware_version(self):
        value = "%s.%s" % (self._read(I2CDataAddresses['FW_VER_MAJOR']), self._read(I2CDataAddresses['FW_VER_MINOR']))
        return(value)
    
    
    def uptime(self):
        value =   self._read(I2CDataAddresses['UPTIME_SEC0'])
        value += (self._read(I2CDataAddresses['UPTIME_SEC1']) << 8)
        value += (self._read(I2CDataAddresses['UPTIME_SEC2']) << 16)
        return(value)
    
    
    def _line_state(self, thermostat=False):
        fan = False
        cool = False
        heat = False
        if thermostat is not True:
            value = self._read(I2CDataAddresses['SYSTEM_STATE'])
        else:
            value = self._read(I2CDataAddresses['THERMOSTAT_STATE'])
        if (value & 0x01) != 0:  fan = True
        if (value & 0x02) != 0:  cool = True
        if (value & 0x04) != 0:  heat = True
        return([fan,cool,heat])
    
    
    def system_state(self):
        return(self._line_state())
    
    
    def thermostat_state(self):
        return(self._line_state(thermostat=True))
    
    
    def _control_state(self, target=False):
        if target is not True:
            value = self._read(I2CDataAddresses['CURRENT_STATE'])
        else:
            value = self._read(I2CDataAddresses['TARGET_STATE'])
        return(ControlStates[value])
    
    
    def current_state(self):
        return(self._control_state())
    
    
    def target_state(self):
        return(self._control_state(target=True))



class DataStore(object):
    
    def __init__(self, host=DBHOST, port=DBPORT, username=DBUSER, password=DBPASS, database=DBNAME):
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.database = database
        
        self._newconnection()
    
    
    def _newconnection(self):
        self.db = influxdb.client.InfluxDBClient(self.host, self.port, self.username, self.password, self.database)
    
    
    def write_record(self, points, series=DBSERIES, retries=DBRETRIES):
        """write a set of point to database
        @points: a list of column data points
        @series: name of data series
        """
        rec = [{
            "name" : series,
            "columns" : ["time", "sysheat", "syscool", "sysfan", "tstatheat", "tstatcool", "tstatfan", "uptime"],
            "points" : [points],
        }]
        
        while retries > 0:
            try:
                self.db.write_points(json.dumps(rec), time_precision='ms')
                break
            except influxdb.client.requests.exceptions.ConnectionError:
                self._newconnection()
                retries -= 1



if __name__ == '__main__':
    ac = ACController()
    db = DataStore()
    
    firstSample = True
    lastUptime = 0
    lastTimestamp = 0
    nextTimestamp = 0
    while True:
        currTimestamp = time.time()
        uptime = ac.uptime()
        
        currTimestampMS = int(currTimestamp * 1000)
        prettyTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')        
        
        (sysfan, syscool, sysheat) = ac.system_state()
        (tstatfan, tstatcool, tstatheat) = ac.thermostat_state()
        
        deltaTimestamp = currTimestamp-lastTimestamp
        deltaUptime = uptime-lastUptime
        lastUptime = uptime
        lastTimestamp = currTimestamp
        
        if firstSample is True:
            firstSample = False
            nextTimestamp = currTimestamp + SAMPLE_PERIOD_SEC_FIRST
            time.sleep(nextTimestamp - time.time())
            continue
        
        if sysfan is True:
            sysfanTime = deltaTimestamp
        else:
            sysfanTime = 0
        
        if syscool is True:
            syscoolTime = deltaTimestamp
        else:
            syscoolTime = 0
        
        if sysheat is True:
            sysheatTime = deltaTimestamp
        else:
            sysheatTime = 0
        
        if tstatfan is True:
            tstatfanTime = deltaTimestamp
        else:
            tstatfanTime = 0
        
        if tstatcool is True:
            tstatcoolTime = deltaTimestamp
        else:
            tstatcoolTime = 0
        
        if tstatheat is True:
            tstatheatTime = deltaTimestamp
        else:
            tstatheatTime = 0
        
        """
        print "%s uptime = %ss, elapsed time = %0.3fs, difference = %dms" % (prettyTime, uptime, deltaTimestamp,(deltaTimestamp-deltaUptime)*1000)
        print "System     fan: +%0.2f, cool: +%0.2f, heat: +%0.2f" % (sysfanTime, syscoolTime, sysheatTime)
        print "Thermostat fan: +%0.2f, cool: +%0.2f, heat: +%0.2f" % (tstatfanTime, tstatcoolTime, tstatheatTime)
        """
        
        sysmsg = ""
        if sysfan:
            sysmsg += "F"
        else:
            sysmsg += " "
        if syscool:
            sysmsg += "C"
        else:
            sysmsg += " "
        if sysheat:
            sysmsg += "H"
        else:
            sysmsg += " "
        
        tstatmsg = ""
        if tstatfan:
            tstatmsg += "F"
        else:
            tstatmsg += " "
        if tstatcool:
            tstatmsg += "C"
        else:
            tstatmsg += " "
        if tstatheat:
            tstatmsg += "H"
        else:
            tstatmsg += " "
        
        print "%s system:%s tstat:%s uptime:%ss clockdiff:%dms" % (prettyTime, sysmsg, tstatmsg, uptime, (deltaTimestamp-deltaUptime)*1000)
        
        db.write_record([currTimestampMS, sysheatTime, syscoolTime, sysfanTime, tstatheatTime, tstatcoolTime, tstatfanTime, uptime])
        
        nextTimestamp += SAMPLE_PERIOD_SEC
        time.sleep(nextTimestamp - time.time())

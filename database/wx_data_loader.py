#!/usr/bin/env python
#
# wx_data_capture.py
#
"""
Weather Data Capture

Copyright (C) 2014 by Eric Dey. All rights reserved.

This module reads data from the ADDS web site and uploads it to an InfluxDB server.

"""

import sys, datetime, time, json, math
from xml.etree import ElementTree
import influxdb

from wx_data_capture import WX_STATIONS as WX_STATIONS, WXServer as WXServer


DBHOST = 'influxdb.tx.deys.org'
DBPORT = 8086
DBUSER = 'root'
DBPASS = 'root'
DBNAME = 'aircontrol'
DBSERIES = 'weather'
DBRETRIES = 5


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
    
    
    def write_record_dewpoint(self, station, tempC, dewpC, timestamp=None, series=DBSERIES, retries=DBRETRIES):
        if timestamp is None:
            timestamp = int(time.time() * 1000)
        
        dewpF = dewpC * 1.8 + 32
        
        rh = int(100 * (math.exp((17.625*dewpC)/(243.04+dewpC))/math.exp((17.625*tempC)/(243.04+tempC))))
        
        rec = [{
            "name" : series,
            "columns" : ["time", "dewpC", "dewpF", "rh", "station"],
            "points" : [[timestamp, dewpC, dewpF, rh, station]],
        }]
        
        while retries > 0:
            try:
                self.db.write_points(json.dumps(rec), time_precision='ms')
                break
            except influxdb.client.requests.exceptions.ConnectionError:
                self._newconnection()
                retries -= 1



if __name__ == '__main__':
    db = DataStore()
    
    inputFileName = sys.argv[1]
    wx = WXServer(WX_STATIONS, inputFileName)

    metars = wx.xmlRoot.findall("./data/METAR")
    if metars is None:
        raise Exception("unable to locate METAR element in tree")
    
    for metar in metars:
        station_id = metar.find("./station_id").text
        temp_c = wx.temperature(station_id, metarElem=metar)
        dewpoint_c = wx.dewpoint(station_id, metarElem=metar)
        observation_time = wx.observationTime(station_id, metarElem=metar)
        
        # This makes the assumption time string is in GMT and local time is CDT
        timestamp = int(datetime.datetime.strptime(observation_time, '%Y-%m-%dT%H:%M:%S').strftime("%s")) - (5*3600)
        timestampMS = timestamp * 1000
        print timestamp, station_id, temp_c, dewpoint_c
        db.write_record_dewpoint(station_id, temp_c, dewpoint_c, timestampMS)

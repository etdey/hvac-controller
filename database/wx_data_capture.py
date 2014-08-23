#!/usr/bin/env python
#
# wx_data_capture.py
#
"""
Weather Data Capture

Copyright (C) 2014 by Eric Dey. All rights reserved.

This module reads data from the ADDS web site and uploads it to an InfluxDB server.

"""

import datetime, time, urllib2, json
from xml.etree import ElementTree
import influxdb


DBHOST = 'influxdb.tx.deys.org'
DBPORT = 8086
DBUSER = 'root'
DBPASS = 'root'
DBNAME = 'aircontrol'
DBSERIES = 'weather'
DBRETRIES = 5

WX_BASEURL = 'http://www.aviationweather.gov/adds/dataserver_current/httpparam'
WX_STATION = 'KATT'

HTTP_USERAGENT = r'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20100101 Firefox/31.0'


class WXServer(object):

    def __init__(self, station):
        self.station = station.upper()
        self.baseurl = WX_BASEURL
    
    
    def temperature(self, farenheit=False):
        urlArgs = ["dataSource=metars", "requestType=retrieve", "format=xml", "hoursBeforeNow=2", "mostRecent=true"]
        urlArgs.append("stationString=%s" % (self.station))
        url = self.baseurl + "?" + "&".join(urlArgs)
        
        urlRequest = urllib2.Request(url)
        urlRequest.add_header("User-Agent", HTTP_USERAGENT)
        inputFile = urllib2.urlopen(urlRequest)
        
        xmlTree = ElementTree.parse(inputFile)
        xmlRoot = xmlTree.getroot()
        if xmlRoot.tag != 'response':
            raise Exception("unexpected root XML tag: %s" % (xmlRoot.tag))
        
        metar = xmlRoot.find("./data/METAR")
        if metar is None:
            raise Exception("unable to locate METAR element in tree")
        
        station_id = metar.find("./station_id").text
        if station_id != self.station:
            raise Exception("station mismatch: %s != %s" % (station_id, self.station))
        
        temp_c = float(metar.find("./temp_c").text)
        if farenheit is True:
            return(temp_c * 1.8 + 32)
        return(temp_c)
    


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
    
    
    def write_record(self, station, tempC, timestamp=None, series=DBSERIES, retries=DBRETRIES):
        if timestamp is None:
            timestamp = int(time.time() * 1000)
        
        tempF = tempC * 1.8 + 32
        
        rec = [{
            "name" : series,
            "columns" : ["time", "tempC", "tempF", "station"],
            "points" : [[timestamp, tempC, tempF, station]],
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
    wx = WXServer(station=WX_STATION)
    
    tempC = wx.temperature()
    db.write_record(WX_STATION, tempC)

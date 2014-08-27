#!/usr/bin/env python
#
# wx_data_capture.py
#
"""
Weather Data Capture

Copyright (C) 2014 by Eric Dey. All rights reserved.

This module reads data from the ADDS web site and uploads it to an InfluxDB server.

"""

import sys, datetime, time, json
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
WX_STATIONS = ['KATT', 'KAUS', 'KHYI', 'KEDC']

HTTP_USERAGENT = r'Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20100101 Firefox/31.0'

SKYCODE_COVERAGE_PERCENT = {
    "SKC" : 0.0,
    "CLR" : 0.0,
    "CAVOK" : 0.0,
    "FEW" : 18.75,
    "SCT" : 43.75,
    "BKN" : 75.0,
    "OVC" : 100.0,
    "OVX" : 100.0,
}


class WXServer(object):

    def __init__(self, stations, xmlDataFile=None):
        self.baseurl = WX_BASEURL
        self.xmlDataFile = xmlDataFile
        self.xmlTree = None
        self.xmlRoot = None
        
        self.stations = []
        for s in stations:
            self.stations.append(s.upper())
        
        self.refresh()
    
    
    def refresh(self, retries=5):
        if self.xmlDataFile is not None:
            # Don't refresh when data was read from a file
            if self.xmlTree is not None:
                return
            inputFile = self.xmlDataFile
        else:
            urlArgs = ["dataSource=metars", "requestType=retrieve", "format=xml", "hoursBeforeNow=2", "mostRecentForEachStation=true"]
            urlArgs.append("stationString=%s" % ("%20".join(self.stations)))
            url = self.baseurl + "?" + "&".join(urlArgs)
            
            urlRequest = urllib2.Request(url)
            urlRequest.add_header("User-Agent", HTTP_USERAGENT)
            
            inputFile = None
            while retries >= 0:
                try:
                    inputFile = urllib2.urlopen(urlRequest)
                    break
                except:
                    retries -= 1
                    continue
            if inputFile is None:
                raise Exception("could not establish connection to ADDS server")
        
        self.xmlTree = ElementTree.parse(inputFile)
        self.xmlRoot = self.xmlTree.getroot()
        if self.xmlRoot.tag != 'response':
            raise Exception("unexpected root XML tag: %s" % (self.xmlRoot.tag))
    
    
    def _find_station_metar(self, station):
        station = station.upper()
        if station not in self.stations:
            raise Exception("requested information for an unknown station: %s" % (station))
        
        metars = self.xmlRoot.findall("./data/METAR")
        if metars is None:
            raise Exception("unable to locate a METAR element in tree")
        
        for metar in metars:
            station_id = metar.find("./station_id").text
            if station_id != station:
                continue
            
            return(metar)
        
        raise Exception("data does not contain station: %s" % (station))
    
    
    def temperature(self, station, farenheit=False, metarElem=None):
        if metarElem is not None:
            metar = metarElem
        else:
            metar = self._find_station_metar(station)
        
        temp_c = float(metar.find("./temp_c").text)
        if farenheit is True:
            return(temp_c * 1.8 + 32)
        return(temp_c)
    
    
    def maxSkyCoverage(self, station, rawText=False, metarElem=None):
        if metarElem is not None:
            metar = metarElem
        else:
            metar = self._find_station_metar(station)
        
        maxValueText = ''
        maxValuePercent = 0.0
        conditionElements = metar.findall("./sky_condition")
        
        for c in conditionElements:
            text = c.get('sky_cover', 0.0)
            if text not in SKYCODE_COVERAGE_PERCENT.keys():
                raise Exception("unexpected sky coverage code found in data: %s" % (text))
            percent = SKYCODE_COVERAGE_PERCENT[text]
            if percent > maxValuePercent:
                maxValueText = text
                maxValuePercent = percent
        
        if rawText is True:
            return(maxValueText)
        return(maxValuePercent)
    
    
    def observationTime(self, station, metarElem=None):
        if metarElem is not None:
            metar = metarElem
        else:
            metar = self._find_station_metar(station)
        
        return(metar.find("./observation_time").text[:-1])
    


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
    
    
    def write_record(self, station, tempC, cloudcover, timestamp=None, series=DBSERIES, retries=DBRETRIES):
        if timestamp is None:
            timestamp = int(time.time() * 1000)
        
        tempF = tempC * 1.8 + 32
        
        rec = [{
            "name" : series,
            "columns" : ["time", "tempC", "tempF", "cloudcover", "station"],
            "points" : [[timestamp, tempC, tempF, cloudcover, station]],
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
        observation_time = wx.observationTime(station_id, metarElem=metar)
        cloudcover = wx.maxSkyCoverage(station_id, metarElem=metar)
        
        # This makes the assumption time string is in GMT and local time is CDT
        timestamp = int(datetime.datetime.strptime(observation_time, '%Y-%m-%dT%H:%M:%S').strftime("%s")) - (5*3600)
        timestampMS = timestamp * 1000
        print timestamp, station_id, temp_c, cloudcover
        db.write_record(station_id, temp_c, cloudcover, timestampMS)

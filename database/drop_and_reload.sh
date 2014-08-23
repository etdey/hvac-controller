#!/bin/bash
#
# drop_and_reload.sh
#
#
echo "Deleting all records..."
curl -G 'http://influxdb:8086/db/aircontrol/series?u=root&p=root&' --data-urlencode "q=delete from downstairs"
echo ""

echo "Importing all records..."
cat /home/etdey/downstairs-ac-controller.txt | ./data_loader.py | curl -X POST -d@- 'http://influxdb:8086/db/aircontrol/series?u=root&p=root'
echo ""

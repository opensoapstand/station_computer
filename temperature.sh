#!/bin/bash

PATHS="/home/df-admin/drinkfill/library/temperature/examples/linux-usart"

cd $PATHS

maxtemp=4
temp=$(./linux-dallas-uart)
dates=$(date +%Y-%m-%d)
times=$(date +%H:%M:%S)

sql="INSERT INTO temperature (date, time, temp) VALUES ('$dates', '$times', $temp);"

sqlite3 /release/db/sqlite/temperature.db "$sql"

if (( $(echo "$temp > $maxtemp" |bc -l) )); then
	echo "High temperaure Alert!"
	echo "The temperature inside the Bentall Drinkfill unit is $temp" | mail -s "Bentall Drinkfill Temperature Alert" paddy@drinkfill.com,andy@drinkfill.com,mike@drinkfill.com
fi

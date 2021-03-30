#!/bin/bash

wget -q --spider http://google.com

if [ $? -eq 0 ]; then
	echo "Internet is connected, starting db backup"
	pgloader --on-error-stop /release/db/sqlite/drinkfill-sqlite.db pgsql://df-admin:D@@nkF1ll$\@/chinook
	if [ $? -eq 0 ]; then
		echo "pgloader win"
		heroku pg:reset -a choicessoapstand --confirm choicessoapstand
		if [ $? -eq 0 ]; then
			echo "heroku pg:reset win"
			heroku pg:push chinook DATABASE_URL -a choicessoapstand
			if [ $? -eq 0 ]; then
				echo "heroku pg:push win"
				heroku restart -a choicessoapstand
				if [ $? -eq 0 ]; then
					echo "heroku restart win"
					exit
				else
					echo "heroku restart fail"
				fi
			else
				echo "heroku pq:push fail"
			fi
		else
			echo "heroku pg:reset fail"
		fi
	else
		echo "pgloader fail"
	fi

else
	echo "Internet is not connected"
fi

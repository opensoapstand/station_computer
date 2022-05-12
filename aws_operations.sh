#!/bin/bash

#######################################################################
# created by Lode for Drinkfill.                                      #
# 2022-05-12                                                          #
# Place and run in Station. In /home/df-admin/production/admin        #
#   or                                                                #
# Place and run in Station. In /home/df-admin/drinkfill               #
#######################################################################

echo 'Drinkfill file transfer menu. CAUTION:Will impact station functionality.'

PS3='Choose option(digit + enter):'
options=( "Station info" "AWS log in" "AWS run station operations" "upload to AWS home folder" "upload to AWS station folder" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "Station info")
            db_path=/home/df-admin/production/db/drinkfill-sqlite_newlayout.db
            if [[ -f "$db_path" ]]; then
                station_id=$(sqlite3 $db_path "select machine_id from machine;")
                echo "Machine id: $station_id"
            else
                echo "Database not found at $db_path"
            fi
            ;;

        "AWS log in")
            cd /home/df-admin/Downloads
            # -tt : https://stackoverflow.com/questions/7114990/pseudo-terminal-will-not-be-allocated-because-stdin-is-not-a-terminal
            cmd=( ssh -tt -i DrinkfillAWS.pem ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com )
            "${cmd[@]}"
            ;;
        "AWS run station operations")
            cd /home/df-admin/Downloads
            # ssh -i DrinkfillAWS.pem ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com "bash stations_operations.sh"
             cmd=( ssh -tt -i DrinkfillAWS.pem ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com "bash stations_operations.sh" )
            "${cmd[@]}"
            ;;
        "upload to AWS home folder")
            read -e -p "Enter filename (like in normal terminal. e.g. use tab for completion): " file
            ls -l "$file"
            full_path="$(pwd)"/"$file"
            echo full_path
            cd /home/df-admin/Downloads
            scp -r -i DrinkfillAWS.pem "$full_path" ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu
            ;;
        "upload to AWS station folder")
            read -e -p "Enter filename (like in normal terminal. e.g. use tab for completion): " file
            ls -l "$file"
            full_path="$(pwd)"/"$file"
            echo full_path
            station_id=$(sqlite3 /home/df-admin/production/db/drinkfill-sqlite_newlayout.db "select machine_id from machine;")

            cd /home/df-admin/Downloads
            scp -r -i DrinkfillAWS.pem "$full_path" ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu/Stations/$station_id
            ;;
         
        "Quit") 
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
    break;
done



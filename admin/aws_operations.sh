#!/bin/bash

#######################################################################
# created by Lode for Drinkfill.                                      #
# 2022-05-12                                                          #
# Place and run in Station. In /home/df-admin/production/admin        #
#   or                                                                #
# Place and run in Station. In /home/df-admin/drinkfill               #
#######################################################################
echo 'Drinkfill file transfer menu. CAUTION:Will impact station functionality.'

port_in_use=$(sudo ./rtunnel_print.sh 2>/dev/null)
PS3="Choose option(digit + enter) (rtunnel port=$port_in_use) :"
options=("Quit" "Station info" "AWS log in" "AWS run station operations" "upload to AWS home folder" "upload to AWS station folder" "upload production as version in AWS SoftwareStation" "upload station_computer/admin/stationsoperations.sh to aws home")
select opt in "${options[@]}"
do
    case $opt in
        "Station info")
           ./status_services.sh
           ;;

        "upload station_computer/admin/stationsoperations.sh to aws home")

            cd /home/df-admin/Downloads
            scp -r -i DrinkfillAWS.pem "/home/df-admin/station_computer/admin/stations_operations.sh" ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu
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
            read -e -p "Enter filename (like in normal terminal but typ /home/df-admin (not~), use tab for completion): " file
            # ls -l "$file"
            # //full_path="$(pwd)"/"$file"
            full_path="$file"
            echo $full_path
            cd /home/df-admin/Downloads
            scp -r -i DrinkfillAWS.pem "$full_path" ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu
            ;;
        "upload to AWS station folder")
            read -e -p "Enter filename (like in normal terminal but typ /home/df-admin (not~), use tab for completion): " file
            full_path="$file"
            echo $full_path
            # ls -l "$file"
            station_id=$(sqlite3 /home/df-admin/production/db/configuration.db "select machine_id from machine;")

            cd /home/df-admin/Downloads
            scp -r -i DrinkfillAWS.pem "$full_path" ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu/Stations/$station_id
            ;;
        "upload production as version in AWS SoftwareStation")
            read -p "Create software version. e.g. 1.0.5 : v" dest_folder
            #  cmd=( ssh -tt -i DrinkfillAWS.pem ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com )
            # "${cmd[@]}"
            cd /home/df-admin/Downloads
            # create logging
            ssh -i DrinkfillAWS.pem ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com "mkdir SoftwareStation/v$dest_folder;mkdir SoftwareStation/v$dest_folder/bin;mkdir SoftwareStation/v$dest_folder/logging;mkdir SoftwareStation/v$dest_folder/db;mkdir SoftwareStation/v$dest_folder/logging/controller;mkdir SoftwareStation/v$dest_folder/logging/transactions;mkdir SoftwareStation/v$dest_folder/logging/ui"
            # copy folders from production
            scp -r -i DrinkfillAWS.pem /home/df-admin/production/bin ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu/SoftwareStation/v$dest_folder
            scp -r -i DrinkfillAWS.pem /home/df-admin/production/admin ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu/SoftwareStation/v$dest_folder
            scp -r -i DrinkfillAWS.pem /home/df-admin/production/references ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu/SoftwareStation/v$dest_folder
            ;;
        "Quit") 
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
    break;
done



#!/bin/bash

#######################################################################
# created by Lode for Drinkfill / Aelen.                              #
# 2023-03-23                                                          #
# Place and run in Station. In /home/df-admin/station_computer/admin  #
#######################################################################

sudo ./status_services.sh

echo 'Drinkfill developer tools. Quick shortcuts to tools.'
echo 'ui is the qt user interface program'
echo 'fsm is the pcb controller program'
echo 'soapstand_manager for more soapstand station functions'
echo 'aws_operations for more aws backend functions'


make_options () {

    if [[ $1 = "fsm" ]]; then
        cd ..
        cd fsm
        ./make_fsm.sh
    elif [[ $1 = "ui_fsm" ]]; then
        cd ..
        cd ui
        ./make_ui.sh
        cd ..
        cd fsm
        ./make_fsm.sh
    elif [[ $1 = "ui" ]]; then
        cd ..
        cd ui
        ./make_ui.sh
    elif [[ $1 = "none" ]]; then
        :
    else
      echo 'No build option chosen'
    fi
    
    if [[ $2 = "copy_binaries" ]]
    then
      echo 'Copy and start binaries to production.'
        sudo systemctl stop ui_soapstand
        sudo systemctl stop controller_soapstand

        sudo scp /home/df-admin/station_computer/ui/DF_UI /home/df-admin/production/bin/DF_UI
        sudo scp /home/df-admin/station_computer/fsm/controller /home/df-admin/production/bin/controller
        
        sudo systemctl start controller_soapstand.service
        sudo systemctl start ui_soapstand.service

    else
      echo 'Will not copy newly build binaries.'
    fi

}

port_in_use=$(sudo ./rtunnel_print.sh 2>/dev/null)
PS3="Choose option(digit + enter) (rtunnel port=$port_in_use) :"
options=("Quit" "aws_operations" "AWS log in" "AWS run station operations" "soapstand_manager" "Station info" "Stop ui and controller" "(Re)start ui and controller" "run standalone controller" "Copy program binary files from station_software git folder to production folder and run" "Create and run production data copied from station_software git folder (without db!)" "Services: Soapstand (re)load from production (ui,controller,wificheck,transactioncheck)" "Setup aws port (rtunnel)" "make ui and fsm" "make ui and fsm and deploy binaries" "make ui" "make ui and deploy binaries" "make fsm" "make fsm and deploy binaries" "Production database operations" "check temperature" )
select opt in "${options[@]}"
do
    case $opt in
        "(Re)start ui and controller")
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service
            ;;
        "Stop ui and controller")
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand
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
        "Copy program binary files from station_software git folder to production folder and run")
            echo "Copy soapstand application to production folder"
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand
            
            sudo scp /home/df-admin/station_computer/ui/DF_UI /home/df-admin/production/bin/DF_UI
            sudo scp /home/df-admin/station_computer/fsm/controller /home/df-admin/production/bin/controller
            
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service
            echo "done."
            ;;
        "Create and run production data copied from station_software git folder (without db!)")
            sudo ./create_and_run_production_data.sh
            echo "done."
            ;;
        "Station info")
           ./status_services.sh
           ;;
        "aws_operations")
           ./aws_operations.sh
           ;;
        "soapstand_manager")
            ./soapstand_manager.sh
            ;;
        "load binaries")
           make_options 'none' 'copy_binaries' 
            ;;
        "make ui and fsm")
           make_options 'ui_fsm' 
            ;;
        "make ui and fsm and deploy binaries")
           make_options 'ui_fsm' 'copy_binaries'
            ;;
        "make ui")
           make_options 'ui' 
            ;;
        "make ui and deploy binaries")
           make_options 'ui' 'copy_binaries'
            ;;
        "make fsm")
           make_options 'fsm' 
            ;;
        "make fsm and deploy binaries")
           make_options 'fsm' 'copy_binaries'
            ;;
        "run standalone controller")
            cd ../fsm
            ./controller
            ;;
        "check temperature")
            echo "use option Production database operations -> chose usage.db->temperature to see all records. "
            DATABASE="/home/df-admin/production/db/usage.db"
            temperature=$(sqlite3 $DATABASE "SELECT * FROM temperature ORDER BY ROWID DESC LIMIT 1;")
            echo "The most recent temperature record is: $temperature"

        ;;
        "Services: Soapstand (re)load from production (ui,controller,wificheck,transactioncheck)")
            # move files to service folder
            ./copy_and_enable_services.sh
            echo 'All done. (note: rtunnel.service is not copied automatically.)'
            ;;
        "Production database operations")
            sudo ./db_operations.sh  
         ;;
        "Setup aws port (rtunnel)")
            sudo ./set_aws_port.sh
            ;;
        "Quit") 
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
    break;
done



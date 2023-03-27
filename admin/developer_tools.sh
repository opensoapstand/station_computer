#!/bin/bash

#######################################################################
# created by Lode for Drinkfill.                                      #
# 2023-03-23                                                          #
# Place and run in Station. In /home/df-admin/drinkfill/admin         #
#######################################################################

./status_services.sh

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

        sudo scp /home/df-admin/drinkfill/ui/DF_UI /home/df-admin/production/bin/DF_UI
        sudo scp /home/df-admin/drinkfill/fsm/controller /home/df-admin/production/bin/controller
        
        sudo systemctl start controller_soapstand.service
        sudo systemctl start ui_soapstand.service

    else
      echo 'Will not copy newly build binaries.'
    fi

}


PS3='Choose option(digit + enter):'
options=("Quit" "aws_operations" "AWS log in" "AWS run station operations" "soapstand_manager"  "Stop ui and controller" "(Re)start ui and controller" "run standalone controller" "Station info" "Copy binaries to production and run" "Create and run production data" "make ui and fsm" "make ui and fsm and load binaries" "make ui" "make ui and load binaries" "make fsm" "make fsm and load binaries" "Setup ssh rtunnel" "(Re)load services from production")
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
        "Create and run production data")
            sudo ./create_and_run_production_data.sh
            echo "done."
            ;;

        "Setup ssh rtunnel")
            ./rtunnel_setup.sh
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
        "make ui and fsm and load binaries")
           make_options 'ui_fsm' 'copy_binaries'
            ;;
        "make ui")
           make_options 'ui' 
            ;;
        "make ui and load binaries")
           make_options 'ui' 'copy_binaries'
            ;;
        "make fsm")
           make_options 'fsm' 
            ;;
        "make fsm and load binaries")
           make_options 'fsm' 'copy_binaries'
            ;;
        "run standalone controller")
            cd ../fsm
            ./controller
            ;;
        "(Re)load services from production")
            # move files to service folder
            ./copy_and_enable_services.sh
            echo 'All done. (note: rtunnel.service is not copied automatically.)'
            ;;
        "Quit") 
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
    break;
done



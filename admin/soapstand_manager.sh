#!/bin/bash
#set -e #setting stop script on error

# echo "----------------------------------------"
# echo "  Drinkfill Soapstand service manager for:"
# echo "        - controller_soapstand.service"
# echo "        - ui_soapstand.service "
# echo "  by Lode 2022-05-03"
# echo "----------------------------------------"

./status_services.sh

PS3='Please enter your choice: '
options=("Quit" "Station info" "Status" "Start" "Stop" "Restart" "Screenshotbot execute" "Enable Autostart" "Disable Autostart" "Copy binary files to production folder" "Create and run production data" "(Re)load services from production" "Setup rtunnel" "Setup Ubuntu for drinkfill UI")
select opt in "${options[@]}"
do
    case $opt in
        "Station info")
            ./status_services.sh
            ;;
        "Status")
            ./status_services.sh
            ;;
        "Start")
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service
            ;;
        "Stop")
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand
            ;;
        "Screenshotbot execute")
            ./screenshotbot.sh
            ;;
        "Restart")
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service
            ;;
        "Enable Autostart")
            sudo systemctl enable ui_soapstand.service
            sudo systemctl enable controller_soapstand.service
            ;;
        "Disable Autostart")
            sudo systemctl disable ui_soapstand.service
            sudo systemctl disable controller_soapstand.service
            ;;
        "Setup Ubuntu for drinkfill UI")
            echo "Screen setup for Drinkfill UI: - disable window transition animations. - set window vertical"
            echo "     - disable window transition animations."
            gsettings set org.gnome.desktop.interface enable-animations false
            ;;
        "Setup rtunnel")
          
            sudo ./rtunnel_setup.sh
            echo "retunnel restarted done"
            ;;
        "Copy binary files to production folder")
            echo "Copy soapstand application to production folder" 
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand

            sudo scp /home/df-admin/drinkfill/ui/DF_UI /home/df-admin/production/bin/DF_UI
            sudo scp /home/df-admin/drinkfill/fsm/controller /home/df-admin/production/bin/controller
            
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service

            ;;
        "Create and run production data")
            sudo ./create_and_run_production_data.sh
            echo "done."
            ;;

        "(Re)load services from production")
            # move files to service folder
            ./copy_and_enable_services.sh
            echo 'All done. (note: rtunnel.service is not copied automatically.)'
            ;;
        "Quit")
            break
            ;;

        *) 
        
        PS3="" # this hides the prompt
        echo asdf | select foo in "${options[@]}"; do break; done # dummy select 
        PS3="Choose an option digit: " # this displays the common prompt
        ;;
        # echo "invalid option $REPLY";;
    esac
    break;
done
#!/bin/bash
#set -e #setting stop script on error

# echo "----------------------------------------"
# echo "  Drinkfill Soapstand service manager for:"
# echo "        - controller_soapstand.service"
# echo "        - ui_soapstand.service "
# echo "  by Lode 2022-05-03"
# echo "----------------------------------------"

# echo "Status report"
# echo "controller:"
# systemctl is-active controller_soapstand.service
# systemctl is-enabled controller_soapstand.service
# echo "ui:"
# systemctl is-active ui_soapstand.service
# systemctl is-enabled ui_soapstand.service
# echo "reversed ssh tunnel:"
# systemctl is-active rtunnel.service
# systemctl is-enabled rtunnel.service
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
            sudo nano /etc/systemd/system/rtunnel.service
            sudo systemctl daemon-reload
            sudo service rtunnel restart
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
         
            echo "stop soapstand services"
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand

            # create production data
            sudo ./create_production_data.sh

            echo "start soapstand services"

            # move services to systemd/system folder
            sudo scp /home/df-admin/production/admin/controller_soapstand.service /etc/systemd/system
            sudo scp /home/df-admin/production/admin/ui_soapstand.service /etc/systemd/system
            sudo scp /home/df-admin/production/admin/check_connectivity.service /etc/systemd/system

            # reload services
            sudo systemctl daemon-reload

            # enable at startup (disable to undo)
            sudo systemctl enable ui_soapstand.service
            sudo systemctl enable controller_soapstand.service
            sudo systemctl enable check_connectivity.service


            # start service
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service
            sudo systemctl start check_connectivity.service

             echo "start aefaefasefasef services"
            # status
            sudo ./status_services.sh

            echo "done."
            ;;

        "(Re)load services from production")
            # move files to service folder
            sudo scp /home/df-admin/production/admin/controller_soapstand.service /etc/systemd/system
            sudo scp /home/df-admin/production/admin/ui_soapstand.service /etc/systemd/system
            sudo scp /home/df-admin/production/admin/check_connectivity.service /etc/systemd/systemd

            #Update station status every 30 mins
            # crontab -l | grep -q 'stationStatus.py' && echo 'Station Status Already exists'  \
            # || crontab -l > mycron 
            # echo "*/30 * * * *  python ~/production/admin/stationStatus.py" >> mycron
        
            #missing Transactions 
            # could be done as a service: run python ~/production/admin/missingTransactions.py hourly
            crontab -l | grep -q 'missingTransactions.py' && echo 'Missing transactions handler scheduler not reloaded: already exists'  \
            || crontab -l > mycron 
            echo "0 * * * *  python ~/production/admin/missingTransactions.py" >> mycron 
            crontab mycron
            rm mycron

            # reload services
            sudo systemctl daemon-reload
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
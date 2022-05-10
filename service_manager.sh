#!/bin/bash
#set -e #setting stop script on error

echo "----------------------------------------"
echo "  Drinkfill Soapstand service manager for:"
echo "        - controller_soapstand.service"
echo "        - ui_soapstand.service "
echo "  by Lode 2022-05-03"
echo "----------------------------------------"

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
options=("Status" "Start" "Stop" "Restart" "Enable Autostart" "Disable Autostart" "Copy binary files to production folder" "Create and run production data" "(Re)load services from production" "Quit")
select opt in "${options[@]}"
do
    case $opt in
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

            # reload services
            sudo systemctl daemon-reload

            # enable at startup (disable to undo)
            sudo systemctl enable ui_soapstand.service
            sudo systemctl enable controller_soapstand.service


            # start service
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service

            # status
            sudo ./status_services.sh

            echo "done."
            ;;

        "(Re)load services from production")
            # move files to service folder
            sudo scp /home/df-admin/production/admin/controller_soapstand.service /etc/systemd/system
            sudo scp /home/df-admin/production/admin/ui_soapstand.service /etc/systemd/system
            
            #missing Transactions 
            # could be done as a service: run python ~/production/admin/missingTransactions.py every hour
            crontab -l | grep -q 'missingTransactions.py' && echo 'Missing transactions handler scheduler not reloaded: already exists'  \
            || crontab -l > mycron 
            echo "0 * * * *  python ~/production/admin/missingTransactions.py" >> mycron 
            crontab mycron
            rm mycron

            # reload services
            sudo systemctl daemon-reload
            echo 'All done'
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
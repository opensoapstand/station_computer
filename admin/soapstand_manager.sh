#!/bin/bash
#set -e #setting stop script on error

# echo "----------------------------------------"
# echo "  Drinkfill Soapstand service manager for:"
# echo "        - controller_soapstand.service"
# echo "        - ui_soapstand.service "
# echo "  by Lode 2022-05-03"
# echo "----------------------------------------"

./status_services.sh

port_in_use=$(sudo ./rtunnel_print.sh 2>/dev/null)
PS3="Choose option(digit + enter) (rtunnel port=$port_in_use) :"
options=("Quit" "Station info" "Status" "Start" "Stop" "Restart" "Screenshotbot execute" "Enable Autostart" "Disable Autostart" "Copy binary files to production folder" "Create and run production data copied from drinkfill folder (without db!)" "(Re)load services from production" "Setup aws port (rtunnel)" "Setup Ubuntu for drinkfill UI" "Deploy productionstatic.zip" "Screenshot: Take single shot" "Copy db from drinkfill to production folder OLD" "Copy configuration db from drinkfill to production folder" "Copy usage db from drinkfill to production folder")
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
        "Screenshot: Take single shot")
            
            read -p "Will save to production/screenshots Input optional name: " name
            current_date=$(date +%Y%m%d-%H%M%S)
            name_with_date="${current_date}_${name}.jpg"
            mkdir "/home/df-admin/production/screenshots"
            full_path="/home/df-admin/production/screenshots/${name_with_date}"
            echo "Screen shot, save to : ${full_path}"
            DISPLAY=:0 scrot ${full_path}
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
            echo "     - decrease reboot time."
            user_question="Continue to decrease boot time? change: GRUB_TIMEOUT=1 add: GRUB_RECORDFAIL_TIMEOUT=\$GRUB_TIMEOUT, [y] to continu, other key to skip."
            read -p "$user_question" -n 1 -r
            echo    # (optional) move to a new line
            if [[ $REPLY =~ ^[Yy]$ ]]
            then
            sudo nano /etc/default/grub
            sudo update-grub
            fi

        ;;
        "Setup aws port (rtunnel)")
            
            sudo ./set_aws_port.sh
            echo "retunnel restarted done"
        ;;
        "Copy binary files to production folder")
            echo "Copy soapstand application to production folder"
            sudo systemctl stop ui_soapstand
            sudo systemctl stop controller_soapstand
            
            sudo scp /home/df-admin/station_computer/ui/DF_UI /home/df-admin/production/bin/DF_UI
            sudo scp /home/df-admin/station_computer/fsm/controller /home/df-admin/production/bin/controller
            
            sudo systemctl start ui_soapstand.service
            sudo systemctl start controller_soapstand.service
            
        ;;
        "Create and run production data copied from drinkfill folder (without db!)")
            sudo ./create_and_run_production_data.sh
            echo "done."
        ;;

        "Copy db from drinkfill to production folder OLD")
            sudo -u df-admin scp /home/df-admin/station_computer/db/sqlite/drinkfill-sqlite_newlayout.db /home/df-admin/production/db/drinkfill-sqlite_newlayout.db 
            echo "done."
        ;;
        "Copy configuration db from drinkfill to production folder")
            sudo -u df-admin scp /home/df-admin/station_computer/db/sqlite/configuration.db /home/df-admin/production/db/configuration.db 
            echo "done."
        ;;
        "Copy usage db from drinkfill to production folder")
            sudo -u df-admin scp /home/df-admin/station_computer/db/sqlite/usage.db /home/df-admin/production/db/usage.db 
            echo "done."
        ;;
        
        "(Re)load services from production")
            # move files to service folder
            ./copy_and_enable_services.sh
            echo 'All done. (note: rtunnel.service is not copied automatically.)'
        ;;
        
        "Deploy productionstatic.zip")
            deploy_production_from_zip.sh
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
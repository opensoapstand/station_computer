#!/usr/bin/env bash

# create dir for screenshots 

screenshotpath="/home/df-admin/production/screenshots"
mkdir -p $screenshotpath

PS3='Which kind of unit are we dealing with? Select number and press Enter: '
echo "|-----Screenshotbot v1.0 2022-05-02----------------------|"
echo "| Lode's screenshot sequence capture bot                 |"
echo "| This is with mouse automation and uses a sequence file.|"
echo "| The application should be active and in idle screen.   |"
echo "| requires: xdotool and scrot (sudo apt-get install xx ) |"
echo "|--------------------------------------------------------|"

options=("Has printer" "Has qr" "Pillar" "Quit")

select opt in "${options[@]}"
do
    case $opt in
        "Has printer")
            echo "Start sequence for units with a receipt printer."
            file="./screenshotbot_sequence_printer.txt"
            break
            ;;
        "Has qr")
            echo "Start sequence for units that work with QR-code payment."
            file="./screenshotbot_sequence_qr.txt"
            break
            ;;
        "Pillar")
            echo "Start sequence for pillar."
            file="./screenshotbot_sequence_pillar.txt"
            break
        ;;
        "Quit")
            file="./dummy.txt"
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done

while read -r line; do
    # disregard lines starting with #
    [[ "$line" =~ ^#.*$ ]] && continue  
    split_line=($line)

    if [ ! -z "${split_line[0]}" ] && [ ! -z "${split_line[1]}" ]; then
        # click mouse position
        DISPLAY=:0 xdotool mousemove ${split_line[0]} ${split_line[1]} 
        DISPLAY=:0 xdotool click 1
        echo "  Click coords x=${split_line[0]} y=${split_line[1]}"
        sleep 0.5
        # take screenshot if a filename is given
        if [ ! -z "${split_line[2]}" ]; then # -z = empty string
            echo "Screen shot, save to : ${screenshotpath}/${split_line[2]}"
            DISPLAY=:0 scrot ${screenshotpath}/${split_line[2]}
            sleep 0.5
        fi
        # read -p "press any key to continue..." < /dev/tty
    fi
done < "$file"

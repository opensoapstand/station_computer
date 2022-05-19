#!/bin/bash

#######################################################################
# created by Lode for Drinkfill.                                      #
# 2022-05-12                                                          #
# Place and run this file in the home folder of the AWS instance      #
#######################################################################

# -----------------------------------
# DO NOT ADD SPACES TO THE ELEMENTS, it causes a mess when looping over it (treats space as array delimiter in some cases).#
station_descriptions=("SS-DEV-LODE" "SS-DEV-ASH" "SS-0000003" "SS-0000004" "EEFC" "UBC" "Ruddy" "Re-Up" "SS-0000009" "Pomme" "Petros11" "Petros12" "Petros13" "Petros14" "SS-0000015" "SS-0000016" "SS-0000017" "Choices" "Stongs" "FamousFoods" "SS-0000021" "Nada" "Petros23" "Petros24" "Petros25" "Petros26");
station_ids=("SS-DEV-LODE" "SS-DEV-003" "SS-0000003" "SS-0000004" "SS-0000005" "SS-0000006" "SS-0000007" "SS-0000008" "SS-0000009" "SS-0000010" "SS-0000011" "SS-0000012" "SS-0000013" "SS-0000014" "SS-0000015" "SS-0000016" "SS-0000017" "SS-0000018" "SS-0000019" "SS-0000020" "SS-0000021" "SS-0000022" "SS-0000023" "SS-0000024" "SS-0000025" "SS-0000026");
station_ports=("44444" "43081" "43003" "43004" "43005" "43006" "43007" "43008" "43009" "43010" "43011" "43012" "43013" "43014" "43015" "43016" "43017" "43018" "43019" "43020" "43021" "43022" "43023" "43024" "43025" "43026");
# -----------------------------------

# https://tldp.org/LDP/Bash-Beginners-Guide/html/sect_10_02.html

get_choice_from_names() {
    # function can only return variables between 0 and 255 it's basically their exit code
    i=0
    select selected in ${station_descriptions[*]}
    
    do
        i=0
        until [[ $selected = ${station_descriptions[$i]} || $i = 250 ]] # https://stackoverflow.com/questions/3427872/whats-the-difference-between-and-in-bash
        do
            # echo $i
            # echo ${station_descriptions[$i]}
            # echo $selected
            let "i+=1"
        done 
        return $i
    done
}

confirm_execute() {
    echo "Execute: $1"
    select yn in "Yes" "No"; do
    case $yn in
        Yes )  
            eval $1
            ;;
        No )
        ;;
    esac
    break
    done
}

continu_or_exit() {
    read -p "Continu [Yy] (any other key to abort)?" -n 1 -r
    echo    # (optional) move to a new line
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
        # do nothing and continu program flow
       :
    else
        # exit bash
        echo "Abort"
        exit 0
    fi
}

function cd_and_stay() {
    cd $1
}

# cd_into_station_AWS_folder () {
#     get_choice_from_names
#     choice_index=$?
#     # menu index is linked to station number

#     station_id="${station_ids[$choice_index]}"
#     station_description="${station_descriptions[$choice_index]}"
#     # echo $station_name
#     echo "cd into /home/ubuntu/Stations/$station_id  (port: $port, description:$station_description )"
#     # https://stackoverflow.com/questions/7114990/pseudo-terminal-will-not-be-allocated-because-stdin-is-not-a-terminal
#     # cmd=( cd /home/ubuntu/Stations/$station_id )
#     # printf -v cmd_str '%q ' "${cmd[@]}"
#     # "${cmd[@]}"
#     # cd_and_stay "/home/ubuntu/Stations/$station_id"
#     bash "cd /home/ubuntu/Stations/$station_id"
# }


ssh_into_station () {
    get_choice_from_names
    choice_index=$?
    # echo $choice_index

    # menu index is linked to station number
    # station_number=$(($choice_index + 1))

    # station_name=$(printf "SS-%07d" $station_number)
    # # echo $station_name
    # port=$(printf "43%03d" $station_number)
    station_id="${station_ids[$choice_index]}"
    station_description="${station_descriptions[$choice_index]}"
    port="${station_ports[$choice_index]}"

    # echo $port
    echo "Log into $station_description Station. (id: $station_id, port: $port)"
    # https://stackoverflow.com/questions/7114990/pseudo-terminal-will-not-be-allocated-because-stdin-is-not-a-terminal
    cmd=( ssh -tt df-admin@localhost -p $port )
    printf -v cmd_str '%q ' "${cmd[@]}"
    echo "Lined up command: "
    echo "$cmd_str"
    # printf -v cmd_str '%q ' "${cmd[@]}"
    "${cmd[@]}"
}

scp_transfer () {
    # arg $1 : source folder. "" is home folder, "production/" is production folder
    # arg $2 : destination folder. "" is home folder, "production/" is production folder
    # ask for init station
    # ask for dest station
    # ask for file/folder (you'll have to know no autofill available)
    
    echo "Choose source station:"
    get_choice_from_names
    choice_index=$?
    source_id="${station_ids[$choice_index]}"
    source_description="${station_descriptions[$choice_index]}"
    source_port="${station_ports[$choice_index]}"

    echo "Choose destination station:"
    get_choice_from_names
    choice_index=$?
    destination_id="${station_ids[$choice_index]}"
    destination_description="${station_descriptions[$choice_index]}"
    destination_port="${station_ports[$choice_index]}"

    read -p "Enter file/folder name will append to /home/df-admin/production/" path

    full_source_path="/home/df-admin/$1$path"
    full_destination_path="/home/df-admin/$2"
    aws_station_path="/home/ubuntu/Stations/$source_id"
    aws_file_path="/home/ubuntu/Stations/$source_id/$path"

    cmd1=( scp -r -P $source_port df-admin@localhost:$full_source_path $aws_station_path )
    cmd2=( scp -r -P $destination_port $aws_file_path df-admin@localhost:$full_destination_path )
    printf -v cmd1_str '%q ' "${cmd1[@]}"
    printf -v cmd2_str '%q ' "${cmd2[@]}"
    
    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd1_str"
    echo "$cmd2_str"
    
    continu_or_exit
    if [[ -f "$aws_file_path" ]]; then
        mv -r "$aws_file_path" "$aws_file_path_bkp"
        echo "Backup made in AWS ($aws_file_path)"
    fi
    "${cmd1[@]}"
    echo "part 1 done"
    "${cmd2[@]}"
}


scp_transfer_db () {
    # arguments:
    # echo $1 #action
    # echo $2 #if action == to_dev or from_dev  : developer station name
    # echo $3 #if action == to_dev or from_dev  : developer rtunnel port

    # get menu index
    get_choice_from_names
    choice_index=$?
    # echo $choice_index

    # menu index is linked to station number
    # station_number=$(($choice_index + 1))
    # echo $station_number

    station_id="${station_ids[$choice_index]}"
   
    # echo $station_name
    # port=$(printf "43%03d" $station_number)
    port="${station_ports[$choice_index]}"
    # echo $port

    if [[ $1 = "to_aws" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd=( scp -P $port df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db Stations/$station_id )
        printf -v cmd_str '%q ' "${cmd[@]}"
        echo "Lined up command: "
        echo "$cmd_str"
        continu_or_exit
        mv Stations/$station_id/drinkfill-sqlite_newlayout.db Stations/$station_id/drinkfill-sqlite_newlayout_bkp.db
        # printf -v cmd_str '%q ' "${cmd[@]}"
        "${cmd[@]}"
       
    elif [[ $1 = "to_unit" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd=( scp -P $port Stations/$station_id/drinkfill-sqlite_newlayout.db df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db )
        printf -v cmd_str '%q ' "${cmd[@]}"
        echo "Lined up command: "        
        echo "$cmd_str"
        # confirm_execute "$cmd_str"
        continu_or_exit
        # ssh  -p $port
        "${cmd[@]}"
    
    elif [[ $1 = "to_dev" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd1=( scp -P $port df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db Stations/$2/drinkfill-sqlite_newlayout_fromUnit.db )
        cmd2=( scp -P $3 Stations/$2/drinkfill-sqlite_newlayout_fromUnit.db df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout_$station_id.db )
        printf -v cmd1_str '%q ' "${cmd1[@]}"
        printf -v cmd2_str '%q ' "${cmd2[@]}"
        
        # confirm_execute "$cmd_str"
        echo "Lined up commands: "
        echo "$cmd1_str"
        echo "$cmd2_str"
        
        continu_or_exit
        mv Stations/$2/drinkfill-sqlite_newlayout_fromUnit.db Stations/$2/drinkfill-sqlite_newlayout_fromUnit_bkp.db
        "${cmd1[@]}"
        "${cmd2[@]}"

    elif [[ $1 = "from_dev" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd1=( scp -P $3 df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout_$station_id.db Stations/$2/drinkfill-sqlite_newlayout_toUnit.db )
        cmd2=( scp -P $port Stations/$2/drinkfill-sqlite_newlayout_toUnit.db df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db )
        printf -v cmd1_str '%q ' "${cmd1[@]}"
        printf -v cmd2_str '%q ' "${cmd2[@]}"
        
        # confirm_execute "$cmd_str"
        echo "Lined up commands: "
        echo "$cmd1_str"
        echo "$cmd2_str"
        
        continu_or_exit
        mv Stations/$2/drinkfill-sqlite_newlayout_toUnit.db Stations/$2/drinkfill-sqlite_newlayout_toUnit_bkp.db
        "${cmd1[@]}"
        "${cmd2[@]}"

    
    else
        echo "Please specify to_aws or to_unit as argument"
    fi
}

echo 'At AWS: Drinkfill file transfer menu. CAUTION:Will impact station functionality.'
PS3='Choose option(digit + enter):'
options=("Quit" "Stations status" "Show Station Descriptions" "Station log in" "Station/production/x to Station/production/x" "Station/production/x to Station/home/x" "Station/home/x to Station/production/x" "Station/home/x to Station/home/x" "Station to AWS DB" "AWS to Station DB" "Station to Lode DB" "Lode to Station DB" "Station to Ash DB" "Ash to Station DB")
select opt in "${options[@]}"
do
    case $opt in
        "Station log in")
            ssh_into_station 
            ;;
        # "cd into Station AWS folder")
        #     cd_into_station_AWS_folder
        #     ;;
        "Show Station Descriptions")
            echo "List for display purposes. Chosing a station will have no effect."
            get_choice_from_names
            ;;
        "Stations status")
            netstat -tulpn | grep LISTEN 
            ;;
        "Station/production/x to Station/production/x")
            scp_transfer "production/" "production/"
            ;;
        "Station/production/x to Station/home/x")
            scp_transfer "production/" ""
            ;;
        "Station/home/x to Station/production/x")
            scp_transfer "" "production/"
            ;;
        "Station/home/x to Station/home/x")
            scp_transfer "" ""
            ;;
        "Station to AWS DB")
            scp_transfer_db "to_aws"
            ;;
        "AWS to Station DB")
            scp_transfer_db "to_unit"
            ;;
        "Station to Lode DB")
            scp_transfer_db "to_dev" "SS-DEV-LODE" "44444"
            ;;
        "Lode to Station DB")
            scp_transfer_db "from_dev" "SS-DEV-LODE" "44444"
            ;;
        "Station to Ash DB")
            scp_transfer_db "to_dev" "SS-DEV-ASH" "43081"
            ;;
        "Ash to Station DB")
            scp_transfer_db "from_dev" "SS-DEV-ASH" "43081"
            ;;
        "Quit") 
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
    break;
done

test_string_equal () {
    i=0
    selected="test"

    until [ "$selected" == "b" ]
    do
        selected="b"
        echo "yest"
    done 
}

test_while () {
    i=0
    while [ 5 -gt $i ] # -gt = greater than -ge  greater or equal then. 
    do
    let "i+=1"
    echo $i
    done 
}
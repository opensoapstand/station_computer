#!/bin/bash

# choose_station(){
# # select name in "SS-0000001" "SS-0000002" "SS-0000003"
# echo hi;
# return 5;
# }

# choose_station   
# echo $;

# https://tldp.org/LDP/Bash-Beginners-Guide/html/sect_10_02.html

# -----------------------------------
# DO NOT ADD SPACES TO THE ELEMENTS, it causes a mess when looping over it (treats space as array delimiter in some cases).#
names=("SS-0000001" "SS-0000002" "SS-0000003" "SS-0000004" "EEFC" "UBC" "Ruddy" "Re-Up" "SS-0000009" "Pomme" "Petros11" "Petros12" "Petros13" "Petros14" "SS-0000015" "SS-0000016" "SS-0000017" "Choices" "Stongs" "FamousFoods" "Nada" "Petros23" "Petros24" "Petros25" "Petros26");
# -----------------------------------


# names=("a" "b" "c");
# IFS="$OLD_IFS"

get_choice_from_names() {
    # function can only return variables between 0 and 255 it's basically their exit code
    i=0
    select selected in ${names[*]}
    
    do
        i=0
        until [[ $selected = ${names[$i]} || $i = 250 ]] # https://stackoverflow.com/questions/3427872/whats-the-difference-between-and-in-bash
        do
            # echo $i
            # echo ${names[$i]}
            # echo $selected
            let "i+=1"
        done 
        return $i
    done
}

scp_unit_to_aws_db () {
    # copy to AWS 
    get_choice_from_names
    choice_index=$?
    echo $choice_index

    station_number=$(($choice_index + 1))
    echo $station_number

    station_name=$(printf "SS-%07d" $station_number)
    echo $station_name
    port=$(printf "43%03d" $station_number)
    echo $port

    # scp unit to aws
    # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
    cmd=( scp -P $port df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db Stations/$station_name )
    printf 'Proposing to run: '
    printf '%q ' "${cmd[@]}"
    printf '\n'
    "${cmd[@]}"
}


scp_unit_to_aws_db

# PS3='Please enter your choice: '
# options=("Unit to AWS DB" "AWS to Unit DB" "Quit")
# select opt in "${options[@]}"
# do
#     case $opt in
#         "Unit to AWS DB")
#             scp_unit_to_aws_db
#             ;;
#         "AWS to Unit DB")
#             scp_aws_to_unit_db
#             ;;
       
#         "Quit")
#             break
#             ;;
#         *) echo "invalid option $REPLY";;
#     esac
#     break;
# done



test_string_equal () {
    i=0
    echo ${names[1]}

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
#!/bin/bash
# -----------------------------------
# DO NOT ADD SPACES TO THE ELEMENTS, it causes a mess when looping over it (treats space as array delimiter in some cases).#
names=("SS-DEV-LODE" "SS-0000002" "SS-0000003" "SS-0000004" "EEFC" "UBC" "Ruddy" "Re-Up" "SS-0000009" "Pomme" "Petros11" "Petros12" "Petros13" "Petros14" "SS-0000015" "SS-0000016" "SS-0000017" "Choices" "Stongs" "FamousFoods" "Nada" "Petros23" "Petros24" "Petros25" "Petros26");
# -----------------------------------

# https://tldp.org/LDP/Bash-Beginners-Guide/html/sect_10_02.html

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

scp_unit_aws_db () {
    echo $1
    echo $2
    echo $3
    # copy to AWS 
    get_choice_from_names
    choice_index=$?
    # echo $choice_index

    station_number=$(($choice_index + 1))
    # echo $station_number

    station_name=$(printf "SS-%07d" $station_number)
    # echo $station_name
    port=$(printf "43%03d" $station_number)
    # echo $port

    if [[ $1 = "to_aws" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd=( scp -P $port df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db Stations/$station_name )
        printf -v cmd_str '%q ' "${cmd[@]}"
        confirm_execute "$cmd_str"
       
    elif [[ $1 = "to_unit" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd=( scp -P $port Stations/$station_name df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db )
        printf -v cmd_str '%q ' "${cmd[@]}"
        confirm_execute "$cmd_str"
    
    elif [[ $1 = "to_lode" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd1=( scp -P $port df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db Stations/SS-DEV-LODE/drinkfill-sqlite_newlayout_fromUnit.db )
        cmd2=( scp -P 44444 Stations/SS-DEV-LODE/drinkfill-sqlite_newlayout_fromUnit.db df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout_$station_name.db )
        printf -v cmd1_str '%q ' "${cmd1[@]}"
        printf -v cmd2_str '%q ' "${cmd2[@]}"
        
        # confirm_execute "$cmd_str"
        echo "Lined up commands: "
        echo "$cmd1_str"
        echo "$cmd2_str"
        
        continu_or_exit
        "${cmd1[@]}"
        "${cmd2[@]}"

    elif [[ $1 = "from_lode" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd1=( scp -P 44444 df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout_$station_name.db Stations/SS-DEV-LODE/drinkfill-sqlite_newlayout_toUnit.db )
        cmd2=( scp -P $port Stations/SS-DEV-LODE/drinkfill-sqlite_newlayout_toUnit.db df-admin@localhost:~/production/db/drinkfill-sqlite_newlayout.db )
        printf -v cmd1_str '%q ' "${cmd1[@]}"
        printf -v cmd2_str '%q ' "${cmd2[@]}"
        
        # confirm_execute "$cmd_str"
        echo "Lined up commands: "
        echo "$cmd1_str"
        echo "$cmd2_str"
        
        continu_or_exit
        "${cmd1[@]}"
        "${cmd2[@]}"

    
    else
        echo "Please specify to_aws or to_unit as argument"
    fi
}

echo 'Drinkfill file transfer menu. CAUTION:Will impact station functionality.'
PS3='Choose option(digit + enter):'
options=("Unit to AWS DB" "AWS to Unit DB" "Unit to SS-DEV-LODE DB" "SS-DEV-LODE to Unit DB" "Unit to SS-DEV-ASH DB" "SS-DEV-ASH to Unit DB" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "Unit to AWS DB")
            scp_unit_aws_db "to_aws"
            ;;
        "AWS to Unit DB")
            scp_unit_aws_db "to_unit"
            ;;
        "Unit to SS-DEV-LODE DB")
            scp_unit_aws_db "to_lode"
            ;;
        "SS-DEV-LODE to Unit DB")
            scp_unit_aws_db "from_lode"
            ;;
        "Unit to SS-DEV-ASH DB")
            scp_unit_aws_db "to_ash"
        ;;
        "SS-DEV-ASH to Unit DB")
            scp_unit_aws_db "from_ash"
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
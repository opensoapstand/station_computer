#!/bin/bash

#######################################################################
# created by Lode for Drinkfill.                                      #
# 2022-05-12                                                          #
# Place and run this file in the home folder of the AWS instance      #
#######################################################################

# -----------------------------------
# DO NOT ADD SPACES TO THE ELEMENTS, it causes a mess when looping over it (treats space as array delimiter in some cases).#
station_descriptions=("SS-DEV-LODE" "SS-DEV-ASH" "SS-DEV-SHOP" "SS-DEV-SHOPFRAME" "EEFC" "UBC" "Ruddy" "Re-Up" "SS-0000009" "Pomme" "Petros11" "Petros12" "Petros13" "Petros14" "SS-0000015" "SS-0000016" "SS-0000017" "Choices" "Stongs" "FamousFoods" "SS-0000021" "Nada" "Petros23" "Petros24" "Petros25" "Petros26" "Petros27" "Petros28" "Petros29" "Petros30" "Petros31" "Petros32" "Petros33" "Petros34" "Petros35" "Petros36" "Petros37" "Petros38" "SS-0000039" "SS-0000040" "SS-0000041" "SS-0000042" "SS-0000043" "SS-0000044" "SS-0000045" "SS-0000046" "SS-0000047" "SS-0000048" "SS-0000049" "SS-0000050" "SS-0000051" "SS-0000052" "SS-0000053" "SS-0000054" "SS-0000055" "SS-0000056" "SS-0000057" "SS-0000058" "SS-0000059" "SS-0000060" "SS-0000061" "SS-0000062" "SS-0000063" "SS-0000064" "SS-0000065" "SS-0000066" "SS-0000067" "SS-0000068" "SS-0000069" "SS-0000070" "SS-0000071" "SS-0000072" "SS-0000073");
station_ids=("SS-DEV-LODE" "SS-DEV-ASH" "SS-DEV-SHOP" "SS-DEV-SHOPFRAME" "SS-0000005" "SS-0000006" "SS-0000007" "SS-0000008" "SS-0000009" "SS-0000010" "SS-0000011" "SS-0000012" "SS-0000013" "SS-0000014" "SS-0000015" "SS-0000016" "SS-0000017" "SS-0000018" "SS-0000019" "SS-0000020" "SS-0000021" "SS-0000022" "SS-0000023" "SS-0000024" "SS-0000025" "SS-0000026" "SS-0000027" "SS-0000028" "SS-0000029" "SS-0000030" "SS-0000031" "SS-0000032" "SS-0000033" "SS-0000034" "SS-0000035" "SS-0000036" "SS-0000037" "SS-0000038" "SS-0000039" "SS-0000040" "SS-0000041" "SS-0000042" "SS-0000043" "SS-0000044" "SS-0000045" "SS-0000046" "SS-0000047" "SS-0000048" "SS-0000049" "SS-0000050" "SS-0000051" "SS-0000052" "SS-0000053" "SS-0000054" "SS-0000055" "SS-0000056" "SS-0000057" "SS-0000058" "SS-0000059" "SS-0000060" "SS-0000061" "SS-0000062" "SS-0000063" "SS-0000064" "SS-0000065" "SS-0000066" "SS-0000067" "SS-0000068" "SS-0000069" "SS-0000070" "SS-0000071" "SS-0000072" "SS-0000073" );
station_ports=("44444" "43081" "44001" "44003" "43005" "43006" "43007" "43008" "43009" "43010" "43011" "43012" "43013" "43014" "43015" "43016" "43017" "43018" "43019" "43020" "43021" "43022" "43023" "43024" "43025" "43026" "43027" "43028" "43029" "43030" "43031" "43032" "43033" "43034" "43035" "43036" "43037" "43038" "43039" "43040" "43041" "43042" "43043" "43044" "43045" "43046" "43047" "43048" "43049" "43050" "43051" "43052" "43053" "43054" "43055" "43056" "43057" "43058" "43059" "43060" "43061" "43062" "43063" "43064" "43065" "43066" "43067" "43068" "43069" "43070" "43071" "43072" "43073");
# -----------------------------------

# https://tldp.org/LDP/Bash-Beginners-Guide/html/sect_10_02.html
global_port=666


# output_list_as_enumerated_columns(){
#     # https://stackoverflow.com/questions/70159505/bash-print-long-list-as-columns
#      $1 | pr -2T
# }

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




#     echo "Please select an option:"

#     options=("Option 1" "Option 2" "Option 3" "Quit")

#     select opt in "${options[@]}"
#     do
#         case $opt in
#             "Option 1")
#                 echo "You chose option 1"
#                 ;;
#             "Option 2")
#                 echo "You chose option 2"
#                 ;;
#             "Option 3")
#                 echo "You chose option 3"
#                 ;;
#             "Quit")
#                 break
#                 ;;
#             *) 
#                 echo "invalid option: $REPLY"
#                 ;;
#         esac
#     done
# }

ssh_into_station () {

    echo "Source Station: "
    get_station_port
    port=$global_port
   
    # echo $port
    # echo "Log into $station_description Station. (id: $station_id, port: $port)"
    # https://stackoverflow.com/questions/7114990/pseudo-terminal-will-not-be-allocated-because-stdin-is-not-a-terminal
    cmd=( ssh -tt df-admin@localhost -p $port )
    printf -v cmd_str '%q ' "${cmd[@]}"
    echo "Lined up command: "
    echo "$cmd_str"
    # printf -v cmd_str '%q ' "${cmd[@]}"
    "${cmd[@]}"
}

transfer_production_usage_db(){
    echo "Source Station: "
    get_station_port
    source_port=$global_port
    echo "Destination Station: "
    get_station_port
    destination_port=$global_port

    PS3='Choose option(digit + enter):'
    options=("dbname to dbname_xsrcx" "dbname_xsrcx to dbname OVERWRITE ALERT" "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE" "dbname to dbname OVERWRITE ALERT" "dbname_xsrcx to dbname_xsrcx")
    # 
    printf "\ndbname=drinkfill-sqlite_newlayout.db, xxxxx=port number (e.g. 43020)\n"
    select opt in "${options[@]}"
    do
        case $opt in
            "dbname to dbname_xsrcx")
            production_usage_db_name_source="usage.db"
            production_usage_db_name_destination="usage_$source_port.db"
                ;;
            "dbname_xsrcx to dbname OVERWRITE ALERT")
            production_usage_db_name_source="usage_$source_port.db"
            production_usage_db_name_destination="usage.db"
                ;;
            "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE")
            production_usage_db_name_source="usage_$destination_port.db"
            production_usage_db_name_destination="usage.db"
                ;;
            "dbname to dbname OVERWRITE ALERT")
            production_usage_db_name_source="usage.db"
            production_usage_db_name_destination="usage.db"
                ;;
            "dbname_xsrcx to dbname_xsrcx")
            production_usage_db_name_source="usage_$source_port.db"
            production_usage_db_name_destination="usage_$source_port.db"
                ;;
            *) echo "invalid option $REPLY";;
        esac
        break;
    done

    production_db_name="configuration_$source_port.db"  # check for where used, not as a variable. Because... it's hard.
    
    cmd0=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/production/db/$production_usage_db_name_source" "/home/ubuntu/Stations/$production_usage_db_name_destination" )
    cmd1=( scp -r -P $destination_port "/home/ubuntu/Stations/$production_usage_db_name_destination" df-admin@localhost:/home/df-admin/production/db)
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"

    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    
    continu_or_exit

    echo "Transfer usage db to aws..."
    "${cmd0[@]}"
    echo "Transfer usage db from aws to station..."
    "${cmd1[@]}"
    echo "done"
}

transfer_production_configuration_db(){
    echo "Source Station: "
    get_station_port
    source_port=$global_port
    echo "Destination Station: "
    get_station_port
    destination_port=$global_port

    PS3='Choose option(digit + enter):'
    options=("dbname to dbname_xsrcx" "dbname_xsrcx to dbname OVERWRITE ALERT" "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE" "dbname to dbname OVERWRITE ALERT" "dbname_xsrcx to dbname_xsrcx")
    # 
    printf "\ndbname=drinkfill-sqlite_newlayout.db, xxxxx=port number (e.g. 43020)\n"
    select opt in "${options[@]}"
    do
        case $opt in
            "dbname to dbname_xsrcx")
            production_configuration_db_name_source="configuration.db"
            production_configuration_db_name_destination="configuration_$source_port.db"
                ;;
            "dbname_xsrcx to dbname OVERWRITE ALERT")
            production_configuration_db_name_source="configuration_$source_port.db"
            production_configuration_db_name_destination="configuration.db"
                ;;
            "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE")
            production_configuration_db_name_source="configuration_$destination_port.db"
            production_configuration_db_name_destination="configuration.db"
                ;;
            "dbname to dbname OVERWRITE ALERT")
            production_configuration_db_name_source="configuration.db"
            production_configuration_db_name_destination="configuration.db"
                ;;
            "dbname_xsrcx to dbname_xsrcx")
            production_configuration_db_name_source="configuration_$source_port.db"
            production_configuration_db_name_destination="configuration_$source_port.db"
                ;;
            *) echo "invalid option $REPLY";;
        esac
        break;
    done

    production_db_name="configuration_$source_port.db"  # check for where used, not as a variable. Because... it's hard.
    
    cmd0=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/production/db/$production_configuration_db_name_source" "/home/ubuntu/Stations/$production_configuration_db_name_destination" )
    cmd1=( scp -r -P $destination_port "/home/ubuntu/Stations/$production_configuration_db_name_destination" df-admin@localhost:/home/df-admin/production/db)
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"

    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    
    continu_or_exit

    echo "Transfer configuration db to aws..."
    "${cmd0[@]}"
    echo "Transfer configuration db from aws to station..."
    "${cmd1[@]}"
    echo "done"
}



transfer_all_production_dbs(){
    echo "Source Station: "
    get_station_port
    source_port=$global_port
    echo "Destination Station: "
    get_station_port
    destination_port=$global_port
    # echo "Source Station: "
    # user_input_port
    # source_port=$global_port
    # echo "Destination Station: "
    # user_input_port
    # destination_port=$global_port

    PS3='Choose option(digit + enter):'
    options=("dbname to dbname_xsrcx" "dbname_xsrcx to dbname OVERWRITE ALERT" "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE" "dbname to dbname OVERWRITE ALERT" "dbname_xsrcx to dbname_xsrcx")
    # 
    printf "\ndbname=drinkfill-sqlite_newlayout.db, xxxxx=port number (e.g. 43020)\n"
    select opt in "${options[@]}"
    do
        case $opt in
            "dbname to dbname_xsrcx")
            production_configuration_db_name_source="configuration.db"
            production_configuration_db_name_destination="configuration_$source_port.db"
            production_usage_db_name_source="usage.db"
            production_usage_db_name_destination="usage_$source_port.db"
                ;;
            "dbname_xsrcx to dbname OVERWRITE ALERT")
            production_configuration_db_name_source="configuration_$source_port.db"
            production_configuration_db_name_destination="configuration.db"
            production_usage_db_name_source="usage_$source_port.db"
            production_usage_db_name_destination="usage.db"
                ;;
            "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE")
            production_configuration_db_name_source="configuration_$destination_port.db"
            production_configuration_db_name_destination="configuration.db"
            production_usage_db_name_source="usage_$destination_port.db"
            production_usage_db_name_destination="usage.db"
                ;;
            "dbname to dbname OVERWRITE ALERT")
            production_configuration_db_name_source="configuration.db"
            production_configuration_db_name_destination="configuration.db"
            productionusagen_db_name_source="usage.db"
            productionusagen_db_name_destination="usage.db"
                ;;
            "dbname_xsrcx to dbname_xsrcx")
            production_configuration_db_name_source="configuration_$source_port.db"
            production_configuration_db_name_destination="configuration_$source_port.db"
            production_usage_db_name_source="usage_$source_port.db"
            production_usage_db_name_destination="usage_$source_port.db"
                ;;
            *) echo "invalid option $REPLY";;
        esac
        break;
    done

    production_db_name="configuration_$source_port.db"  # check for where used, not as a variable. Because... it's hard.
    
    cmd0=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/production/db/$production_configuration_db_name_source" "/home/ubuntu/Stations/$production_configuration_db_name_destination" )
    cmd1=( scp -r -P $destination_port "/home/ubuntu/Stations/$production_configuration_db_name_destination" df-admin@localhost:/home/df-admin/production/db)
    cmd2=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/production/db/$production_usage_db_name_source" "/home/ubuntu/Stations/$production_usage_db_name_destination" )
    cmd3=( scp -r -P $destination_port "/home/ubuntu/Stations/$production_usage_db_name_destination" df-admin@localhost:/home/df-admin/production/db)
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"
    printf -v cmd2_str '%q ' "${cmd2[@]}"
    printf -v cmd3_str '%q ' "${cmd3[@]}"

    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    echo "$cmd2_str"
    echo "$cmd3_str"
    
    continu_or_exit

    echo "Transfer configuration db to aws..."
    "${cmd0[@]}"
    echo "Transfer configuration db from aws to station..."
    "${cmd1[@]}"
    echo "Transfer usage db to aws..."
    "${cmd2[@]}"
    echo "Transfer usage db from aws to station..."
    "${cmd3[@]}"
    echo "done"
}

transfer_production_db_old(){

    echo "Source Station: "
    get_station_port
    source_port=$global_port
    echo "Destination Station: "
    get_station_port
    destination_port=$global_port

    PS3='Choose option(digit + enter):'
    options=("dbname to dbname_xsrcx" "dbname_xsrcx to dbname OVERWRITE ALERT" "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE" "dbname to dbname OVERWRITE ALERT" "dbname_xsrcx to dbname_xsrcx")
    # 
    printf "\ndbname=drinkfill-sqlite_newlayout.db, xxxxx=port number (e.g. 43020)\n"
    select opt in "${options[@]}"
    do
        case $opt in
            "dbname to dbname_xsrcx")
            production_db_name_source="drinkfill-sqlite_newlayout.db"
            production_db_name_destination="drinkfill-sqlite_newlayout_$source_port.db"
                ;;
            "dbname_xsrcx to dbname OVERWRITE ALERT")
            production_db_name_source="drinkfill-sqlite_newlayout_$source_port.db"
            production_db_name_destination="drinkfill-sqlite_newlayout.db"
                ;;
            "dbname_xDEST to dbname OVERWRITE ALERT --> SPECIAL CASE")
            production_db_name_source="drinkfill-sqlite_newlayout_$destination_port.db"
            production_db_name_destination="drinkfill-sqlite_newlayout.db"
                ;;
            "dbname to dbname OVERWRITE ALERT")
            production_db_name_source="drinkfill-sqlite_newlayout.db"
            production_db_name_destination="drinkfill-sqlite_newlayout.db"
                ;;
            "dbname_xsrcx to dbname_xsrcx")
            production_db_name_source="drinkfill-sqlite_newlayout_$source_port.db"
            production_db_name_destination="drinkfill-sqlite_newlayout_$source_port.db"
                ;;
            *) echo "invalid option $REPLY";;
        esac
        break;
    done

    production_db_name="configuration_$source_port.db"  # check for where used, not as a variable. Because... it's hard.
    
    # transfer zip from source station to aws 
    cmd0=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/production/db/$production_db_name_source" "/home/ubuntu/Stations/$production_db_name_destination" )
    # transfer zip from aws to destination station
    cmd1=( scp -r -P $destination_port "/home/ubuntu/Stations/$production_db_name_destination" df-admin@localhost:/home/df-admin/production/db)
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"

    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    
    continu_or_exit

    echo "Transfer db to aws..."
    "${cmd0[@]}"
    echo "Transfer db from aws to station..."
    "${cmd1[@]}"
    echo "done"
}

transfer_production_logging(){

    echo "Source Station: "
    get_station_port
    source_port=$global_port
    echo "Destination Station: "
    get_station_port
    destination_port=$global_port

    logging_zip_name=logging_$source_port.zip  # check for where used, not as a variable. Because... it's hard.
    
    # zip it up
    cmd0=( sudo ssh -t df-admin@localhost -p $source_port 'cd /home/df-admin/production; zip -r logging.zip logging; mv logging.zip ..' )
    
    # transfer zip from source station to aws 
    cmd1=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/logging.zip" "/home/ubuntu/Stations/" )
    # transfer zip from aws to destination station

    cmd2=( scp -r -P $destination_port "/home/ubuntu/Stations/logging.zip" df-admin@localhost:/home/df-admin/$logging_zip_name )

    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"
    printf -v cmd2_str '%q ' "${cmd2[@]}"

    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    echo "$cmd2_str"
    
    continu_or_exit
    echo "Zip production logs..."
    "${cmd0[@]}"
    echo "Transfer production logs to aws..."
    "${cmd1[@]}"
    echo "Transfer production logs from aws to station..."
    "${cmd2[@]}"
    echo "done"
}

get_station_port() {
    # echo "Stations count: ${#station_descriptions[@]}";

    # function can only return variables between 0 and 255 it's basically their exit code
     PS3="Choose an option from the list, or enter a 5 digit port number: "
    i=0
    select selected in ${station_descriptions[*]}
    do
        # echo "$REPLY"
        # echo "$i"
        if [ -z "$selected" ]
        then
            # echo "\$selected is empty"
            global_port=$REPLY
        else
            # echo "\$selected is NOT empty"
            i=0
            until [[ $selected = ${station_descriptions[$i]} || $i -gt ${#station_descriptions[@]} ]] # https://stackoverflow.com/questions/3427872/whats-the-difference-between-and-in-bash
            do
                # echo $i
                # echo ${station_descriptions[$i]}
                # echo $selected
                
                let "i+=1"
            done 
           
            global_port="${station_ports[$i]}"
        fi
        break
    done
        # station_id="${station_ids[$choice_index]}"
        # station_description="${station_descriptions[$choice_index]}"
        # port="${station_ports[$choice_index]}"
}

user_input_port(){


    PS3="Choose an option (or press Enter to exit): "
    valid_choice=false

    while [[ $valid_choice -eq false ]];do
        select option in "port" "id"; do
            case $option in
                "port")
                    read -p "Source station port e.g. 43066: " global_port
                    valid_choice=true
                    break
                    ;;
                "id")
                    get_choice_from_names
                    choice_index=$?
                    global_port="${station_ports[$choice_index]}"
                    valid_choice=true
                    break
                    ;;
                *)
                    echo "Invalid option. Try again."
                    ;;
            esac
        done
        break
    done
    # global_port="$source_port"
}


# user_input_port(){
#     read -p "Source station port e.g. 43066: " global_port
# }
transfer_production_static_files(){

    echo "Source Station: "
    get_station_port
    source_port=$global_port
    echo "Destination Station: "
    get_station_port
    destination_port=$global_port
 
    production_zip_name=productionstatic.zip  # check for where used, not as a variable. Because... it's hard.
    
    # zip it up
    cmd0=( sudo ssh -t df-admin@localhost -p $source_port 'cd /home/df-admin/production; zip -r productionstatic.zip references admin bin; mv productionstatic.zip ..' )
    # cmd0=( sudo ssh -t df-admin@localhost -p $source_port 'cd "/home/df-admin/production"; zip -r production.zip references admin bin; mv production.zip ..' )
    
    # transfer zip from source station to aws 
    cmd1=( scp -r -P $source_port "df-admin@localhost:/home/df-admin/$production_zip_name" "/home/ubuntu/Stations/" )
    # transfer zip from aws to destination station
    cmd2=( scp -r -P $destination_port "/home/ubuntu/Stations/$production_zip_name" df-admin@localhost:/home/df-admin )
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"
    printf -v cmd2_str '%q ' "${cmd2[@]}"

    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    echo "$cmd2_str"
    
    continu_or_exit
    echo "Zip static production data..."
    "${cmd0[@]}"
    echo "Transfer static production data to aws..."
    "${cmd1[@]}"
    echo "Transfer static production data from aws to station..."
    "${cmd2[@]}"
    echo "done"

    if [[ $1 = "deploy" ]]
    then
        ssh -tt df-admin@localhost -p $destination_port 'bash /home/df-admin/production/admin/deploy_production_from_zip.sh'
    fi
}


scp_aws_transfer(){
    echo "Choose destination station:"
    get_choice_from_names
    choice_index=$?
    destination_id="${station_ids[$choice_index]}"
    destination_description="${station_descriptions[$choice_index]}"
    destination_port="${station_ports[$choice_index]}"

    read -e -p "Enter filename (like in normal terminal. e.g. use tab for completion): " file
    ls -l "$file"
    full_path="/home/ubuntu"/"$file"
    echo full_path
    # cd /home/df-admin/Downloads
    # scp -r -i DrinkfillAWS.pem "$full_path" ubuntu@ec2-44-225-153-121.us-west-2.compute.amazonaws.com:/home/ubuntu
    
    cmd0=(scp -r -P $destination_port "$full_path"  df-admin@localhost:/home/df-admin) 
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    
    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    continu_or_exit
    "${cmd0[@]}"
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



    # check argument for being empty (which indicates home folder, in that case, we create a folder with the id of the source)
    if [ -z "$2" ]
    then
        
        cmd0=(ssh -p $destination_port df-admin@localhost mkdir -p $source_id) 
        full_destination_path="/home/df-admin/$source_id/"
    else
        
        cmd0=""
        full_destination_path="/home/df-admin/$2"
    fi

    aws_station_path="/home/ubuntu/Stations/$source_id"
    aws_file_path="/home/ubuntu/Stations/$source_id/$path"
    cmd1=( scp -r -P $source_port df-admin@localhost:$full_source_path $aws_station_path )
    cmd2=( scp -r -P $destination_port $aws_file_path df-admin@localhost:$full_destination_path )
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"
    printf -v cmd2_str '%q ' "${cmd2[@]}"
    
    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    echo "$cmd2_str"
    
    continu_or_exit
    if [[ -f "$aws_file_path" ]]; then
        mv -r "$aws_file_path" "$aws_file_path_bkp"
        echo "Backup made in AWS ($aws_file_path)"
    fi
    "${cmd0[@]}"
    "${cmd1[@]}"
    echo "station to AWS done"
    "${cmd2[@]}"
    echo "AWS to station done"
}

remote_mkdir(){

    echo "Choose station:"
    read -p "Enter port (e.g. 43023)" destination_port
    read -p "Enter file/folder name will append to /home/df-admin/" path
    cmd0=(ssh -p $destination_port df-admin@localhost mkdir -p $path) 
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    echo "$cmd0_str"
    "${cmd0[@]}"
}

scp_transfer_manual_ports () {
    # arg $1 : source folder. "" is home folder, "production/" is production folder
    # arg $2 : destination folder. "" is home folder, "production/" is production folder
    # ask for init station
    # ask for dest station
    # ask for file/folder (you'll have to know no autofill available)
    
    echo "Choose source station:"
    read -p "Enter source port" source_port
    source_id=$source_port
    
    echo "Choose destination station:"
    read -p "Enter destination port" destination_port
    destination_id=$destination_port

    read -p "Enter file/folder name will append to /home/df-admin/production/" path

    full_source_path="/home/df-admin/$1$path"

    # check for empty (which is home folder, in that case, we create a folder with the id of the source)
    if [ -z "$2" ]
    then
        
        cmd0=(ssh -p $destination_port df-admin@localhost mkdir $source_id) 
        full_destination_path="/home/df-admin/$source_id/"
    else
        
        cmd0=""
        full_destination_path="/home/df-admin/$2"
    fi

    aws_station_path="/home/ubuntu/Stations/$source_id"
    # problem: we should be able to create the full path in aws (including subfolders. This doesn't happen though.)
    aws_file_path="/home/ubuntu/Stations/$source_id/$path"
    cmd1=( scp -r -P $source_port df-admin@localhost:$full_source_path $aws_station_path )
    cmd2=( scp -r -P $destination_port $aws_station_path df-admin@localhost:$full_destination_path )
    printf -v cmd0_str '%q ' "${cmd0[@]}"
    printf -v cmd1_str '%q ' "${cmd1[@]}"
    printf -v cmd2_str '%q ' "${cmd2[@]}"
    
    # confirm_execute "$cmd_str"
    echo "Lined up commands: "
    echo "$cmd0_str"
    echo "$cmd1_str"
    echo "$cmd2_str"
    
    continu_or_exit
    if [[ -f "$aws_file_path" ]]; then
        mv -r "$aws_file_path" "$aws_file_path_bkp"
        echo "Backup made in AWS ($aws_file_path)"
    fi
    "${cmd0[@]}"
    "${cmd1[@]}"
    echo "station to AWS done"
    "${cmd2[@]}"
    echo "AWS to station done"
}

scp_transfer_db () {
    # arguments:

    # echo $1 #action: from_dev or to_dev
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
        cmd=( scp -P $port df-admin@localhost:~/production/db/configuration.db Stations/$station_id )
        printf -v cmd_str '%q ' "${cmd[@]}"
        echo "Lined up command: "
        echo "$cmd_str"
        continu_or_exit
        mv Stations/$station_id/configuration.db Stations/$station_id/configuration_bkp.db
        # printf -v cmd_str '%q ' "${cmd[@]}"
        "${cmd[@]}"
       
    elif [[ $1 = "to_unit" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd=( scp -P $port Stations/$station_id/configuration.db df-admin@localhost:~/production/db/configuration.db )
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
        cmd1=( scp -P $port df-admin@localhost:~/production/db/configuration.db Stations/$2/configuration_fromUnit.db )
        cmd2=( scp -P $3 Stations/$2/configuration_fromUnit.db df-admin@localhost:~/production/db/configuration_$station_id.db )
        printf -v cmd1_str '%q ' "${cmd1[@]}"
        printf -v cmd2_str '%q ' "${cmd2[@]}"
        
        # confirm_execute "$cmd_str"
        echo "Lined up commands: "
        echo "$cmd1_str"
        echo "$cmd2_str"
        
        continu_or_exit
        mv Stations/$2/configuration_fromUnit.db Stations/$2/configuration_fromUnit_bkp.db
        "${cmd1[@]}"
        "${cmd2[@]}"

    elif [[ $1 = "from_dev" ]]
    then
        # run command https://stackoverflow.com/questions/2005192/how-to-execute-a-bash-command-stored-as-a-string-with-quotes-and-asterisk
        cmd1=( scp -P $3 df-admin@localhost:~/production/db/configuration_$station_id.db Stations/$2/configuration_toUnit.db )
        cmd2=( scp -P $port Stations/$2/configuration_toUnit.db df-admin@localhost:~/production/db/configuration.db )
        printf -v cmd1_str '%q ' "${cmd1[@]}"
        printf -v cmd2_str '%q ' "${cmd2[@]}"
        
        # confirm_execute "$cmd_str"
        echo "Lined up commands: "
        echo "$cmd1_str"
        echo "$cmd2_str"
        
        continu_or_exit
        mv Stations/$2/configuration_toUnit.db Stations/$2/configuration_toUnit_bkp.db
        "${cmd1[@]}"
        "${cmd2[@]}"
DB
    
    else
        echo "Please specify to_aws or to_unit as argument"
    fi
}


deploy_with_ash () {
    echo "Select Source station"
    get_choice_from_names
    choice_index=$?
    source_id="${station_ids[$choice_index]}"
    echo "$source_id"
    echo "Select Destination station"
    get_choice_from_names
    choice_index=$?
    destination_id="${station_ids[$choice_index]}"
    echo "$destination_id"
    
}

PS3="Choose option(digit + enter) :"
options=("Quit" "Stations status" "Station log in" "Production Folder Copy and Deploy: Static files" "Production Folder Copy: Static files" "Production Folder Copy: Logging Folder" "Production Folder Copy: Databases" "Production Folder Copy: Configuration database" "Production Folder Copy: Usage database" "Production Folder Copy: Database OLD" )
# options=("Quit" "Stations status" "Show Station Descriptions" "Station log in" "Station/production/x to Station/production/x" "Station/production/x to Station/home/x" "Station/home/x to Station/production/x" "Station/home/x to Station/home/x" "AWS to Station/home/x" "Station to AWS DB" "AWS to Station DB" "Station to Lode DB" "Lode to Station DB" "Station to Ash DB" "Ash to Station DB" "Manualport/production/x to Manualport/home/x" "Station mkdir" "Station log in [port]" "Static Production Copy: Station to Station" "Static Production Copy: Station to [port]" "Static Production Copy: [port] to Station" "Static Production Copy: [port] to [port]" "DB Production copy: Station to Station" "DB Production copy: Station to [port]" "DB Production copy: [port] to Station" "DB Production copy: [port] to [port]" "Logs Production Copy: Station to Station" "Logs Production Copy: Station to [port]" "Logs Production Copy: [port] to Station" "Logs Production Copy: [port] to [port]")

select opt in "${options[@]}"
do
    case $opt in
        "Station log in")
            ssh_into_station 
            ;;
        
        "List Stations as Name:Port")
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
        "AWS to Station/home/x")
            scp_aws_transfer
            ;;
        "Station to AWS DB")
            scp_transfer_db "to_aws"
            ;;
        "AWS to Station DB")
            scp_transfer_db "to_unit"
            ;;
        "Deploy wizard with Ash")
            echo "Deploy with Ash"
            deploy_with_ash
            ;;

        "Production Folder Copy: Static files")
            transfer_production_static_files  
            ;;
        "Production Folder Copy and Deploy: Static files")
            transfer_production_static_files "deploy"
            ;;
        "Production Folder Copy: Database OLD")
            transfer_production_db_old
            ;;
        "Production Folder Copy: Databases")
            transfer_all_production_dbs  
            ;;
        "Production Folder Copy: Configuration database")
            transfer_production_configuration_db  
            ;;
            
        "Production Folder Copy: Usage database")
            transfer_production_usage_db  
            ;;

        "Production Folder Copy: Logging Folder")
            transfer_production_logging  
            ;;
      
        "Manualport/production/x to Manualport/home/x")
            scp_transfer_manual_ports "production/" ""
            ;;
        "Station mkdir")
            remote_mkdir 
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


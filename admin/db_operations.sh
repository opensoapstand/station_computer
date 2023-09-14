#!/bin/bash
############################################################################
#
# view and modify the drinkfill/aelen station production databases locally
# by lode  
#
############################################################################
db_path_config="/home/df-admin/production/db/configuration.db"
db_path_usage="/home/df-admin/production/db/usage.db"
global_variable_temporary="empty"


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

add_column() {
    echo "Choose db, type and name and the column will be added. Handle with care! "
    PS3="Choose a db and table for the new column to be added to:"
    options=("configuration.db->machine" "configuration.db->products" "usage.db->transactions" "usage.db->events" "usage.db->payments" "usage.db->temperature" )
    
    select opt in "${options[@]}"
    do
        case $opt in
            "configuration.db->machine")
                table_name="machine"
                db_path="$db_path_config"
                break
                ;;
            "configuration.db->products")
                table_name="products"
                selected_type="REAL"
                db_path="$db_path_config"
                break
                ;;
            "usage.db->transactions")
                table_name="transactions"
                db_path="$db_path_usage"
                break
                ;;
            "usage.db->payments")
                table_name="payments"
                db_path="$db_path_usage"
                break
                ;;
            "usage.db->temperature")
                table_name="temperature"
                db_path="$db_path_usage"
                break
                ;;
            "usage.db->events")
                table_name="events"
                db_path="$db_path_usage"
                break
                ;;
            *)
                echo "Invalid option. Please select a valid data type."
                ;;
        esac
    done
                
    PS3="Choose a data type:"
    options=("INTEGER" "REAL" "TEXT" )
    select opt in "${options[@]}"
    do
        case $opt in
            "INTEGER")
                selected_type="INTEGER"
                break
                ;;
            "REAL")
                selected_type="REAL"
                break
                ;;
            "TEXT")
                selected_type="TEXT"
                break
                ;;
            *)
                echo "Invalid option. Please select a valid data type."
                ;;
        esac
    done

    read -p "Enter the new column name: " new_column_name


    # SQL query to add a new column
    query="ALTER TABLE $table_name ADD COLUMN $new_column_name $selected_type;"
    echo $query
    continu_or_exit

    # Execute the query using sqlite3
    sqlite3 "$db_path" "$query"

    echo "Column '$new_column_name' added to the '$table_name' table in the database."
}

display_records_limited() {
    local db_path="$1"
    local table_name="$2"
    offset=0
    limit=10
    while true; do
        sqlite3 "$db_path" "SELECT * FROM $table_name ORDER BY rowid DESC LIMIT $limit OFFSET $offset;"
        echo "Do you want to display the next $limit records? [enter/y/n/new amount])"
        read user_input
        if [ "$user_input" == "y" ]; then
            offset=$((offset + limit))
        elif [ "$user_input" == "n" ]; then
            break
        elif [[ "$user_input" =~ ^[0-9]+$ ]]; then
            limit=$user_input
            # offset=0
        else
            echo "Invalid input. Please enter 'y', 'n', or a number."
        fi
        offset=$((offset + limit))
    done
}

display_and_change_table() {
    local db_path="$1"
    local table_name="$2"
    local condition="$3" # e.g. WHERE slot=2
    echo "$condition"
    display_table_records "$db_path_config" "$table_name" "1" "$condition"
    echo "Enter column number to change (0 to quit):"
    read user_input
    if [ "$user_input" = "0" ]; then
        return
    fi

    table_column_number_to_name "$db_path" "$table_name" "$user_input"
    column_name="$global_variable_temporary"
    echo "Enter new value for $column_name:"
    read user_input
    db_change_record_value "$db_path" "$table_name" "$column_name" "$user_input" "$condition"
}

table_column_number_to_name() {
    local db_path="$1"
    local table_name="$2"
    local column_number="$3"
    local condition="$4" # e.g. WHERE slot=2

    table_column_names "$db_path" "$table_name" "1" "$condition"
    # echo $global_variable_temporary 
    field_name=$(echo "$global_variable_temporary" | cut -d ',' -f $column_number)
    # field_name=$(echo "$input_string" | cut -d ',' -f $column_number)
    global_variable_temporary="$field_name"
}

table_column_names() {
    local db_path="$1"
    local table_name="$2"
    local max_record_count="$3"
    local condition="$4" # e.g. WHERE slot=2

    # Get the column names and types from PRAGMA table_info
    column_info=$(sqlite3 "$db_path" "PRAGMA table_info($table_name);")
    global_variable_temporary=""
    i=1
    while read -r column; do
        column_name=$(echo "$column" | cut -d '|' -f 2)
        if [ -n "$global_variable_temporary" ]; then
            global_variable_temporary+=","  # Add a comma separator if needed
        fi
        global_variable_temporary+="$column_name"
        ((i++))

    done  <<< "$column_info"
}

display_table_schema() {
    local db_path="$1"
    local table_name="$2"

    echo "Layout of table '$table_name' of '$db_path'"
    column_info=$(sqlite3 "$db_path" "PRAGMA table_info($table_name);")
    i=1
    while read -r column; do
        column_name=$(echo "$column" | cut -d '|' -f 2)
        column_type=$(echo "$column" | cut -d '|' -f 3)
        # record_value=$(echo "$record_data" | cut -d '|' -f 3)
        
        printf "%-3s|%-10s|%-30s\n" $i "$column_type" "$column_name" 
        ((i++))
    done  <<< "$column_info"
}

display_table_records() {
    local db_path="$1"
    local table_name="$2"
    local max_record_count="$3"
    local condition="$4" # e.g. WHERE slot=2
    
    echo "Layout of table '$table_name' of '$db_path'"
    column_info=$(sqlite3 "$db_path" "PRAGMA table_info($table_name);")
    sql_statement="SELECT * FROM $table_name $condition LIMIT $max_record_count;"
    echo "SQL statement:  '$sql_statement'"
    # Get the column names and types from PRAGMA table_info
    record_data=$(sqlite3 "$db_path" "$sql_statement" | tr -cd '[:print:]') # | tr -cd '[:print:]' ---> deletes all non printable characters!
    i=1
    while read -r column; do
        column_name=$(echo "$column" | cut -d '|' -f 2)
        column_type=$(echo "$column" | cut -d '|' -f 3)
        # record_value=$(echo "$record_data" | cut -d '|' -f 3)
        
        record_value=$(echo "$record_data" | awk -F "|" -v col=$i '{ print $col }')
        truncated_value=${record_value:0:100}
        printf "%-3s|%-10s|%-30s|%s\n" $i "$column_type" "$column_name" "$truncated_value"
        # printf "%s|%s|%s|%s\n" $i $column_type $column_name $truncated_value
        ((i++))
    done  <<< "$column_info"
}

db_change_record_value() {
    local db_path="$1"
    local table_name="$2"
    local column_name="$3"
    local new_value="$4"
    local condition="$5" # e.g. WHERE slot=3

    # update_sql="UPDATE $table_name SET $column_name='$new_value' WHERE $condition;"
    update_sql="UPDATE $table_name SET $column_name='$new_value' $condition;"
    echo "$update_sql"
    sqlite3 "$db_path" "$update_sql"
}


display_table_column_count() {
    local db_path="$1"
    local table_name="$2"

    column_names=$(sqlite3 "$db_path" "PRAGMA table_info($table_name);" | awk -F "|" '{print $1,$2, $3}')

    # Count the number of columns
    num_columns=$(echo "$column_names" | wc -l)

    printf "Column count: "
    printf "%-5s\t%-20s\t(%s)\n" "$num_columns" "$table_name" "$db_path"

    # echo "$db_path->$table_name colums: $num_columns"
}

if [ -f "$db_path_config" ]; then
    : # null command
else
    echo "ERROR: Database file not found: "$db_path_config""
fi 

if [ -f "$db_path_usage" ]; then
    : # null command
else
    echo "ERROR: Database file not found: $db_path_usage"
fi 

PS3="Choose an option (press the digit and enter):"
options=("Quit" "add column to db table" "create production config.db backup" "column count" "configuration.db->machine general" "configuration.db->products slot 1" "configuration.db->products slot 2" "configuration.db->products slot 3" "configuration.db->products slot 4" "usage.db->transactions" "usage.db->events" "usage.db->payments" "usage.db->temperature" )
select opt in "${options[@]}"
do
    case $opt in
        "add column to db table")
            add_column
            ;;
        "create production config.db backup")
            timestamp="$(date +%Y%m%d-%H%M%S)"
            cp "$db_path_config" "$db_path_config"_BKP"$timestamp"
            ;;
        "configuration.db->products slot 1")
            display_and_change_table "$db_path_config" "products" "WHERE slot=1"
            ;;
        "configuration.db->products slot 2")
            display_and_change_table "$db_path_config" "products" "WHERE slot=2"
            ;;
        "configuration.db->products slot 3")
            display_and_change_table "$db_path_config" "products" "WHERE slot=3"
            ;;
        "configuration.db->products slot 4")
            display_and_change_table "$db_path_config" "products" "WHERE slot=4"
            ;;
        "configuration.db->machine general")
            display_and_change_table "$db_path_config" "machine" ""
            ;;
        "truncated string")
            value="1234567890brecht"
            truncated_string=${value:0:7}
            printf "%-10s\n" $truncated_string
            ;;
        "column count")
            display_table_column_count "$db_path_config" "machine"
            display_table_column_count "$db_path_config" "products"
            display_table_column_count "$db_path_usage" "events"
            display_table_column_count "$db_path_usage" "payments"
            display_table_column_count "$db_path_usage" "transactions"
            display_table_column_count "$db_path_usage" "temperature"
            ;;
        "configuration machine")
            display_table_schema "$db_path_config" "machine"
            # sqlite3 "$db_path_config" "SELECT * FROM machine ORDER BY rowid DESC LIMIT 100;"
            ;;
        "configuration products")
            display_table_schema "$db_path_config" "products"
            ;;
        "usage.db->events")
            display_table_schema "$db_path_usage" "events"
            display_records_limited "$db_path_usage" "events"
            ;;
        "usage.db->transactions")
            display_table_schema "$db_path_usage" "transactions"
            display_records_limited "$db_path_usage" "transactions"
            ;;
        "usage.db->payments")
            display_table_schema "$db_path_usage" "payments"
            display_records_limited "$db_path_usage" "payments"
            ;;
        "usage.db->temperature")
            display_table_schema "$db_path_usage" "temperature"
            display_records_limited "$db_path_usage" "temperature"
            ;;
        "display events 100")
           display_records_limited "$db_path_usage" "temperature"
           ;;
        "Quit")
            echo "Exiting."
            exit 0
            ;;
        *)
            echo "Invalid option. Select a valid option."
            ;;
    esac

    break; # comment if menu loop is desired
done



#include "setup_Tap.h"
#include "commands.h"

std::map<std::string, std::string> registerDevice(int socket){
    std::string public_key = read_public_key();
    std::string registerCommand = "<TRANSACTION> <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>9121</ENTRY_CODE>\
                        <KEY>"+public_key+"</KEY> \
                    </TRANSACTION>";
    std::cout << registerCommand;
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(registerCommand, socket, true);
    createOrUpdateConfigFile(dataReceived["MAC_KEY"], dataReceived["MAC_LABEL"], "192.168.1.64", "0");
    close(socket);
    return dataReceived;
}

std::string getCounter(int socket, std::string MAC_LABEL, std::string MAC_KEY){
    std::string get_counter_command = "<TRANSACTION> \
                            <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE> \
                            <COMMAND>GET_COUNTER</COMMAND> \
                            <MAC>"+ MAC_KEY + "</MAC>\
                            <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL> \
                        </TRANSACTION>";
    std::map<std::string, std::string> responseObj = sendAndReceivePacket(get_counter_command, socket, false);
    std::cout << "Response" << responseObj["COUNTER"];
    return responseObj["COUNTER"];      
}

std::map<std::string, std::string> getNextCounterMac(int socket, std::string MAC_LABEL, std::string MAC_KEY){
    int counter = std::stoi(getCounter(socket, MAC_LABEL, MAC_KEY));
    counter +=1;
    std::string encoded_counter = create_counter_mac(counter, MAC_KEY);
    std::map<std::string, std::string> responseObj;
    responseObj["COUNTER"] = std::to_string(counter);
    responseObj["COUNTER_ENCODED"] = encoded_counter;
    return responseObj;
}

std::map<std::string, std::string> startSession(int socket, std::string MAC_LABEL, std::string MAC_KEY, int invoice){
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    // Convert the time to a string
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d");
    std::string dateString = ss.str();

    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>SESSION</FUNCTION_TYPE>\
                    <COMMAND>START</COMMAND>\
                    <BUSINESSDATE>"+dateString+"</BUSINESSDATE>\
                    <SWIPE_AHEAD>1</SWIPE_AHEAD>\
                    <TRAINING_MODE>0</TRAINING_MODE>\
                    <INVOICE>"+std::to_string(invoice)+"</INVOICE>\
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER> \
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC> \
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                    <POS_IP>\
                    </POS_IP>\
                    <POS_PORT>5015</POS_PORT>\
                    <NOTIFY_SCA_EVENTS>FALSE</NOTIFY_SCA_EVENTS>\
                    </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    updateInvoiceValueInConfig(std::to_string(invoice));
    return dataReceived;
}

std::map<std::string, std::string> finishSession(int socket, std::string MAC_LABEL, std::string MAC_KEY){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);

    std::string command = "<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>FINISH</COMMAND>\
                        <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                        <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                        <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                    </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);

    return dataReceived;
}

std::map<std::string, std::string> authorization(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string amount){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                        <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                        <COMMAND>AUTH</COMMAND> \
                        <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                        <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                        <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL> \
                        <TRANS_AMOUNT>"+amount+"</TRANS_AMOUNT>\
                        <MANUAL_ENTRY>FALSE</MANUAL_ENTRY>\
                        <RECURRING>Y</RECURRING> \
                        <BILLPAY>TRUE</BILLPAY> \
                        <OC_INDUSTRY_CODE>M</OC_INDUSTRY_CODE> \
                        <ENCRYPT>TRUE</ENCRYPT> \
                        <SCMCI_INDICATOR>2</SCMCI_INDICATOR> \
                    </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    return dataReceived;
}
std::map<std::string, std::string> capture(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string CTROUTD, std::string amount){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                    <COMMAND>CAPTURE</COMMAND>\
                    <CTROUTD>"+CTROUTD+"</CTROUTD> \
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                    <TRANS_AMOUNT>"+amount+"</TRANS_AMOUNT>\
                    <PAYMENT_TYPE>CREDIT</PAYMENT_TYPE>\
                    <FORCE_FLAG>1</FORCE_FLAG>\
                </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    return dataReceived;
}

std::map<std::string, std::string> voidTransaction(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string CTROUTD){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                <COMMAND>VOID</COMMAND> \
                <CTROUTD>"+CTROUTD+"</CTROUTD> \
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                <PAYMENT_TYPE>CREDIT</PAYMENT_TYPE>\
            </TRANSACTION>";
        std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    return dataReceived;
}
std::map<std::string, std::string> testMac(int socket, std::string MAC_KEY, std::string MAC_LABEL){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                    <COMMAND>TEST_MAC</COMMAND> \
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    close(socket);
    return dataReceived;
}

int createOrUpdateConfigFile (std::string macKey,std::string macLabel,std::string iP,std::string invoiceNumber){
    std::ofstream configFile("/home/df-admin/production/admin/config.txt");
    if (configFile.is_open()) {
        chmod("config.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        configFile << "MAC_KEY=" + macKey +"\n";
        configFile << "MAC_LABEL="+macLabel+"\n";
        configFile << "IP="+iP+"\n";
        configFile << "INVOICE="+invoiceNumber+"\n";
        configFile.close();
    }
    else {
        std::cout << "Unable to open file" << std::endl;
    }
    return 0;
}


std::map<std::string, std::string> readConfigFile(){
    std::ifstream configFile("/home/df-admin/production/admin/config.txt");
    std::map<std::string, std::string> configMap;
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            // Split line into key-value pair
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                configMap[key] = value;
            }
        }
        configFile.close();
    }
    else {
        registerDevice(connectSocket());
    }
    return configMap;
}

std::string updateInvoiceValueInConfig(std::string invoiceNumber){
    
    std::map<std::string, std::string> configMap = readConfigFile();
    std::string key, value;

    configMap["invoiceNumber"] = invoiceNumber;
    
    // Write the updated configMap back to the file
    std::ofstream outFile("/home/df-admin/production/admin/config.txt");

    if (outFile.is_open()) {
        for (auto& [key, value] : configMap) {
            outFile << key << "=" << value << std::endl;
        }
        outFile.close();
    }
    else {
        std::cout << "Unable to write to file" << std::endl;
        return "1";
    }
    return "0";
}
// int socket = connectSocket();
//     std::string MAC_LABEL = "P_GQ63SC";
//     std::string MAC_KEY = "c0oOuJjLxFnt/e/43FqGSW+7xkuwQonAaNHusrdXHWZnhiX14EZeA32uLGvGz5LvUorrCEWQmbaezJR1ICKUgZQa4zE0GbmxZF+tKJa7V4d31o1y2IkgBx97ErA8HY9MegWhFr+2YOJoYtkrf62bjPAAZ6Ge2etpTAve/CaRa9rKiI5lbmucj7ygs2/7l6YoSspbSWyPZr2gML8plmZk0J6TWYOEB3IOdV1r4yzSTp6FMnnKPQafEScJ+jqbUrF54BQKU3UcAQbFI8WGEHYOS8FDRg8gjRlcviSwCZr7bslgp+9ndQMJPtmph9YhWCggTA6fJNziGWKjzwbwORzGRQ==";
//     // std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
//     startSession(socket, MAC_LABEL, MAC_KEY);
//     std::map<std::string, std::string> responseObj = authorization(socket, MAC_LABEL, MAC_KEY, "3.00");
//     capture(socket, MAC_LABEL, MAC_KEY,responseObj["CTROUTD"], "2.00");
//     finishSession(socket, MAC_LABEL, MAC_KEY);
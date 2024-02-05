#include "setup_Tap.h"
#include "commands.h"
#include <QDebug>
#include <QString>



std::map<std::string, std::string> registerDevice(int socket){
    std::string public_key = read_public_key();
    std::cout << "Registering tap device";
    std::string registerCommand = "<TRANSACTION> <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>9121</ENTRY_CODE>\
                        <KEY>"+public_key+"</KEY> \
                    </TRANSACTION>";
    std::cout << registerCommand; 
    qDebug() << QString::fromUtf8(registerCommand.c_str());
    qDebug() << "Socket id" << socket;
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(registerCommand, socket, true);
    if(dataReceived["MAC_KEY"]!=""){
        qDebug() << "Mac Key received after registration";
        createOrUpdateConfigFile(dataReceived["MAC_KEY"], dataReceived["MAC_LABEL"], "0");
        close(socket);
    }
    else{
        qDebug() << "Registration failed";
        // rebootDevice(connectSecondarySocket());
    }
    
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
                    <POS_IP>192.168.1.2</POS_IP>\
                    <POS_PORT>5017</POS_PORT>\
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
    close(socket);
    return dataReceived;
}

std::map<std::string, std::string> cancelTransaction(int socket){

    std::string command = "<TRANSACTION> \
                        <FUNCTION_TYPE>SECONDARYPORT</FUNCTION_TYPE> \
                        <COMMAND>CANCEL</COMMAND> \
                    </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    // close(socket);
    return dataReceived;
}

std::map<std::string, std::string> checkDeviceStatus(int socket){
    std::string command = "<TRANSACTION> \
                        <FUNCTION_TYPE>SECONDARYPORT</FUNCTION_TYPE> \
                        <COMMAND>STATUS</COMMAND> \
                    </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    // close(socket);
    return dataReceived;
}

std::map<std::string, std::string> rebootDevice(int socket){

    std::string command = "<TRANSACTION> \
                        <FUNCTION_TYPE>SECONDARYPORT</FUNCTION_TYPE> \
                        <COMMAND>REBOOT</COMMAND> \
                    </TRANSACTION>";
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    // close(socket);
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

std::string authorizationCommand(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string amount){
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
    return command;
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
                    <FORCE_FLAG>FALSE</FORCE_FLAG>\
                    <ENCRYPT>TRUE</ENCRYPT>\
                </TRANSACTION>";
    std::cout << command << std::endl; 
    qDebug() << QString::fromUtf8(command.c_str());
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    return dataReceived;
}

std::map<std::string, std::string> captureOffline(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string AUTH_CODE, std::string amount){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                    <COMMAND>CAPTURE</COMMAND>\
                    <AUTH_CODE>"+AUTH_CODE+"</AUTH_CODE> \
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                    <TRANS_AMOUNT>"+amount+"</TRANS_AMOUNT>\
                    <FORCE_FLAG>FALSE</FORCE_FLAG>\
                    <MANUAL_ENTRY>FALSE</MANUAL_ENTRY>\
                    <CARD_PRESENT>FALSE</CARD_PRESENT>\
                    <ENCRYPT>TRUE</ENCRYPT>\
                </TRANSACTION>";
    std::cout << command << std::endl;
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    return dataReceived;
}


std::map<std::string, std::string> editSaf(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string SAF_NUM, std::string amount, std::string SAF_STATUS){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>SAF</FUNCTION_TYPE> \
                    <COMMAND>EDIT</COMMAND>\
                    <SAF_NUM>"+SAF_NUM+"</SAF_NUM> \
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
                    <TRANS_AMOUNT>"+amount+"</TRANS_AMOUNT>\
                    <SET_SAF_STATUS>"+SAF_STATUS+"</SET_SAF_STATUS>\
                </TRANSACTION>";
    std::cout << command << std::endl;
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
        std::cout << command << std::endl;
        std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket, true);
    return dataReceived;
}



std::map<std::string, std::string> voidTransactionOffline(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string SAF_NUM){
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                <FUNCTION_TYPE>SAF</FUNCTION_TYPE> \
                    <COMMAND>REMOVE</COMMAND> \
                    <SAF_NUM_BEGIN>"+SAF_NUM+"</SAF_NUM_BEGIN> \
                    <SAF_NUM_END>"+SAF_NUM+"</SAF_NUM_END> \
                    <COUNTER>"+responseObj["COUNTER"]+"</COUNTER>\
                    <MAC>"+responseObj["COUNTER_ENCODED"]+"</MAC>\
                    <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL>\
            </TRANSACTION>";
    std::cout << command << std::endl;
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

int createOrUpdateConfigFile (std::string macKey,std::string macLabel,std::string invoiceNumber){
    std::ofstream configFile("/home/df-admin/production/admin/tap_payment/config.txt");
    std::ofstream configHistoryFile("/home/df-admin/production/admin/tap_payment/configHistory.txt", std::ios::app);
    qDebug() << "Creating or updating config file";
    if (configFile.is_open()) {
        chmod("config.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        configFile << "MAC_KEY=" + macKey +"\n";
        configFile << "MAC_LABEL="+macLabel+"\n";
        configFile << "INVOICE="+invoiceNumber+"\n";
        configFile.close();
    }
    if (configHistoryFile.is_open()) {
        chmod("configHistory.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        configHistoryFile << "---------------------- \n";
        configHistoryFile << "MAC_KEY=" + macKey +"\n";
        configHistoryFile << "MAC_LABEL="+macLabel+"\n";
        configHistoryFile.close();
    }
    else {
        std::cout << "Unable to open file" << std::endl;
    }
    return 0;
}


std::map<std::string, std::string> readConfigFile(){
    std::ifstream configFile("/home/df-admin/production/admin/tap_payment/config.txt");
    std::map<std::string, std::string> configMap;
    qDebug() << "Reading config file";
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

    configMap["INVOICE"] = invoiceNumber;
    
    // Write the updated configMap back to the file
    std::ofstream outFile("/home/df-admin/production/admin/tap_payment/config.txt");

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

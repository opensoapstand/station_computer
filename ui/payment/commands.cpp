#include "setup_Tap.h"
#include "commands.h"

std::string registerDevice(int socket, std::string public_key){
    std::string registerCommand = "<TRANSACTION> <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>9121</ENTRY_CODE>\
                        <KEY>"+public_key+"</KEY> \
                    </TRANSACTION>";
    return registerCommand;
}

std::string getCounter(int socket, std::string MAC_LABEL, std::string MAC_KEY){
    std::string get_counter_command = "<TRANSACTION> \
                            <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE> \
                            <COMMAND>GET_COUNTER</COMMAND> \
                            <MAC>"+ MAC_KEY + "</MAC>\
                            <MAC_LABEL>"+MAC_LABEL+"</MAC_LABEL> \
                        </TRANSACTION>";
    std::map<std::string, std::string> responseObj = sendAndReceivePacket(get_counter_command, socket);
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

std::map<std::string, std::string> startSession(int socket, std::string MAC_LABEL, std::string MAC_KEY){
    int invoice = 1;
    std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>SESSION</FUNCTION_TYPE>\
                    <COMMAND>START</COMMAND>\
                    <BUSINESSDATE>20230213</BUSINESSDATE>\
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
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket);
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
    std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket);

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
        std::map<std::string, std::string> dataReceived = sendAndReceivePacket(command, socket);

    return dataReceived;
}

std::string testMac(int socket, std::string counter, std::string MAC, std::string MAC_LABEL){
    std::string command = "<TRANSACTION> \
                    <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                    <COMMAND>TEST_MAC</COMMAND> \
                    <MAC_LABEL>"+ MAC_LABEL +"</MAC_LABEL> \
                    <COUNTER>"+ counter+"</COUNTER> \
                    <MAC>"+MAC+"</MAC> \
                </TRANSACTION>";
    return command;
}



int main(){
    int socket = connectSocket();
    std::string MAC_LABEL = "P_GQ63SC";
    std::string MAC_KEY = "c0oOuJjLxFnt/e/43FqGSW+7xkuwQonAaNHusrdXHWZnhiX14EZeA32uLGvGz5LvUorrCEWQmbaezJR1ICKUgZQa4zE0GbmxZF+tKJa7V4d31o1y2IkgBx97ErA8HY9MegWhFr+2YOJoYtkrf62bjPAAZ6Ge2etpTAve/CaRa9rKiI5lbmucj7ygs2/7l6YoSspbSWyPZr2gML8plmZk0J6TWYOEB3IOdV1r4yzSTp6FMnnKPQafEScJ+jqbUrF54BQKU3UcAQbFI8WGEHYOS8FDRg8gjRlcviSwCZr7bslgp+9ndQMJPtmph9YhWCggTA6fJNziGWKjzwbwORzGRQ==";
    // std::map<std::string, std::string> responseObj = getNextCounterMac(socket, MAC_LABEL, MAC_KEY);
    // startSession(socket, MAC_LABEL, MAC_KEY);
    finishSession(socket, MAC_LABEL, MAC_KEY);
    return 1;
}


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <locale>
#include <unistd.h>
#include <sys/stat.h>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>


std::map<std::string, std::string> registerDevice(int socket);
std::string getCounter(int socket, std::string MAC_LABEL, std::string MAC_KEY);
std::map<std::string, std::string> startSession(int socket, std::string MAC_LABEL, std::string MAC_KEY, int invoice);
std::map<std::string, std::string> finishSession(int socket, std::string MAC_LABEL, std::string MAC_KEY);
std::map<std::string, std::string> getNextCounterMac(int socket, std::string MAC_LABEL, std::string MAC_KEY);
std::map<std::string, std::string> authorization(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string amount);
std::map<std::string, std::string> capture(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string CTROUTD, std::string amount);
std::map<std::string, std::string> captureOffline(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string AUTH_CODE, std::string amount);
std::map<std::string, std::string> editSaf(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string SAF_NUM, std::string amount, std::string SAF_STATUS);
std::map<std::string, std::string> voidTransaction(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string CTROUTD);
std::map<std::string, std::string> voidTransactionOffline(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string AUTH_CODE);
int createOrUpdateConfigFile (std::string macKey, std::string macLabel, std::string iP, std::string invoiceNumber);
std::map<std::string, std::string>  testMac(int socket, std::string MAC_KEY, std::string MAC_LABEL);
std::map<std::string, std::string> readConfigFile();
std::string updateInvoiceValueInConfig(std::string invoiceNumber);
std::string authorizationCommand(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string amount);
std::map<std::string, std::string> cancelTransaction(int socket);
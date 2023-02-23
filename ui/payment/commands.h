#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <locale>
#include <unistd.h>

std::string registerDevice(int socket, std::string public_key);
std::string getCounter(int socket, std::string MAC_LABEL, std::string MAC_KEY);
std::map<std::string, std::string> startSession(int socket, std::string MAC_LABEL, std::string MAC_KEY, int invoice);
std::map<std::string, std::string> finishSession(int socket, std::string MAC_LABEL, std::string MAC_KEY);
std::map<std::string, std::string> getNextCounterMac(int socket, std::string MAC_LABEL, std::string MAC_KEY);
std::map<std::string, std::string> authorization(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string amount);
std::map<std::string, std::string> capture(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string CTROUTD, std::string amount);
std::map<std::string, std::string> voidTransaction(int socket, std::string MAC_LABEL, std::string MAC_KEY, std::string CTROUTD);
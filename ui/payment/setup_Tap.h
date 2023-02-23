#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/hmac.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <locale>
#include <unistd.h>
#include<codecvt>
#include <iomanip>
#include <cmath>

std::string read_public_key();
std::map<std::string,std::string> readXmlPacket(std::string xmlString);
std::map<std::string, std::string> sendAndReceivePacket(std::string command, int sockfd);
std::string create_counter_mac(int counter, std::string encrypted_mac);
int connectSocket();
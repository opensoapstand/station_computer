#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <openssl/asn1.h>
#include <unistd.h>
#include <vector>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <codecvt>
#include <locale>


int generateRsaKey()
{
    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    int ret = BN_set_word(e, RSA_F4);
    if (ret != 1)
    {
        std::cerr << "Error setting value for BIGNUM e." << std::endl;
        RSA_free(rsa);
        BN_free(e);
        return 1;
    }

    ret = RSA_generate_key_ex(rsa, 2048, e, nullptr);
    if (ret != 1)
    {
        std::cerr << "Error generating RSA key pair." << std::endl;
        RSA_free(rsa);
        BN_free(e);
        return 1;
    }
    std::cout << ret;

    // Save private key to DER format
    int privateKeySize = i2d_RSAPrivateKey(rsa, nullptr);
    if (privateKeySize <= 0)
    {
        std::cerr << "Error getting size of private key in DER format." << std::endl;
        RSA_free(rsa);
        BN_free(e);
        return 1;
    }

    unsigned char *privateKeyBuffer = new unsigned char[privateKeySize];
    unsigned char *privateKeyBufferPointer = privateKeyBuffer;
    ret = i2d_RSAPrivateKey(rsa, &privateKeyBufferPointer);
    if (ret <= 0)
    {
        std::cerr << "Error converting private key to DER format." << std::endl;
        RSA_free(rsa);
        BN_free(e);
        delete[] privateKeyBuffer;
        return 1;
    }

    std::ofstream privateKeyFile("private.der", std::ios::binary);
    if (!privateKeyFile)
    {
        std::cerr << "Error opening private key file for writing." << std::endl;
        RSA_free(rsa);
        BN_free(e);
        delete[] privateKeyBuffer;
        return 1;
    }

    privateKeyFile.write(reinterpret_cast<char *>(privateKeyBuffer), privateKeySize);
    privateKeyFile.close();
    delete[] privateKeyBuffer;

    // Save public key to DER format
    int publicKeySize = i2d_RSAPublicKey(rsa, nullptr);
    if (publicKeySize <= 0)
    {
        std::cerr << "Error getting size of public key in DER format." << std::endl;
        RSA_free(rsa);
        BN_free(e);
        return 1;
    }

    unsigned char *publicKeyBuffer = new unsigned char[publicKeySize];
    unsigned char *publicKeyBufferPointer = publicKeyBuffer;
    ret = i2d_RSAPublicKey(rsa, &publicKeyBufferPointer);
    // // Write public key to file in DER format
    std::ofstream publicKeyFile("public.der", std::ios::out | std::ios::binary);
    publicKeyFile.write((char *) publicKeyBuffer, publicKeySize);
    publicKeyFile.close();
    
}

RSA *read_public_key(const std::string &filename)
{
    RSA *rsa = RSA_new();
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return nullptr;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    const unsigned char *der = reinterpret_cast<const unsigned char *>(buffer.data());
    rsa = d2i_RSAPublicKey(NULL, &der, buffer.size());
    file.close();
    return rsa;
}

std::string encode_base64(const std::vector<char> &input)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input.data(), input.size());
    BIO_flush(b64);

    char *buffer = NULL;
    long size = BIO_get_mem_data(bmem, &buffer);
    std::string output(buffer, size);
    BIO_free_all(b64);
    return output;
}


int connectSocket(){
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    // Set up the server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5015);
    if (inet_pton(AF_INET, "192.168.1.73", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Error converting IP address" << std::endl;
        return 1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }
    RSA *rsa = read_public_key("public.der");
    if (!rsa) {
        // Handle the error
        std::cout << "Error reading Public Key";
    }
    int key_size = i2d_RSAPublicKey(rsa, NULL);
    std::vector<unsigned char> der(key_size);
    unsigned char *p = der.data();
    i2d_RSAPublicKey(rsa, &p);

    std::vector<char> key(der.begin(), der.end());
    std::string encoded = encode_base64(key);

    RSA_free(rsa);
    // Prepare the message
    memset(buffer, 0, sizeof(buffer));
    std::string registerCommand = "<TRANSACTION> <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>9131</ENTRY_CODE>\
                        <KEY>"+encoded+"</KEY> \
                    </TRANSACTION>";
    std::cout << registerCommand;
    strcpy(buffer, registerCommand.c_str());
    // Send the message
    if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
        std::cerr << "Error sending message" << std::endl;
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
        std::cerr << "Error receiving message" << std::endl;
        return 1;
    }
    std::cout << "Received message: " << buffer << std::endl;
    // Clean up
    close(sockfd);
    return 0;
}



int main(){
    // std::cout << "In main";
    // connectSocket();
    generateRsaKey();
    // std::cout<< readPublicKey();
    return 1;
}
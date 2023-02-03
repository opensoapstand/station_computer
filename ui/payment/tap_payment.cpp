#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/hmac.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
using namespace std;

int generateKeyPair(){
    int key_size = 2048;
    RSA *rsa = RSA_generate_key(key_size, RSA_F4, nullptr, nullptr);
    if (rsa == nullptr) {
        std::cerr << "Error generating RSA key" << std::endl;
        ERR_print_errors_fp(stderr);
        return 1;
    }

    int private_key_len = i2d_RSAPrivateKey(rsa, nullptr);
    unsigned char *private_key_buf = (unsigned char *)malloc(private_key_len);
    unsigned char *private_key_buf_ptr = private_key_buf;
    i2d_RSAPrivateKey(rsa, &private_key_buf_ptr);

    std::ofstream private_key_file("private_key.der", std::ios::binary);
    private_key_file.write((char *)private_key_buf, private_key_len);
    private_key_file.close();
    free(private_key_buf);

    int public_key_len = i2d_RSA_PUBKEY(rsa, nullptr);
    unsigned char *public_key_buf = (unsigned char *)malloc(public_key_len);
    unsigned char *public_key_buf_ptr = public_key_buf;
    i2d_RSA_PUBKEY(rsa, &public_key_buf_ptr);

    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, public_key_buf, public_key_len);
    BIO_flush(b64);

    char *public_key_b64;
    long public_key_b64_len = BIO_get_mem_data(bmem, &public_key_b64);

    std::ofstream public_key_file("public_key.der");
    public_key_file.write(public_key_b64, public_key_b64_len);
    public_key_file.close();
    BIO_free_all(b64);
    free(public_key_buf);

    RSA_free(rsa);
    return 1;
}

int main() {
  
  const int BUFFER_SIZE = 1024;

//   int s = socket(AF_INET, SOCK_STREAM, 0);
//     if (s == -1) {
//         cerr << "Error creating socket" << endl;
//         return 1;
//     }
//     struct sockaddr_in server;
//     server.sin_family = AF_INET;
//     server.sin_port = htons(5015);
//     inet_aton("192.168.1.78", &(server.sin_addr));

//     if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
//        std::cout << "Error connecting to server" ;
//         return 1;
//     }
//     std::string xml_packet_sent = '<TRANSACTION> \
//                         <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
//                         <COMMAND>REGISTER</COMMAND>\
//                         <ENTRY_CODE>9134</ENTRY_CODE>\
//                         <KEY>'+public_key+'</KEY> \
//                     </TRANSACTION>';
//     send(s, xml_packet_sent.c_str(), xml_packet_sent.length(), 0);
//      char buffer[BUFFER_SIZE];
//     memset(buffer, 0, BUFFER_SIZE);
//     int bytes_received = recv(s, buffer, BUFFER_SIZE, 0);
//     if (bytes_received == -1) {
//         std::cerr << "Error receiving data" << std::endl;
//         return 1;
//     }

//     std::string xml_packet(buffer, bytes_received);
//     std::cout << xml_packet << std::endl;
    // socket code goes here

    // close(s);
    return 0;
}
#include "setup_Tap.h"
#include "../page_tap_payment.h"

bool xml_packet_received_complete;
bool xml_card_tap_complete;
bool xml_card_tap_failed;
std::string xml_packet_string;
std::string xml_card_tapped;
std::atomic<bool> stop_tap_action_thread(false);
std::atomic<bool> stop_authorization_thread(false);


std::string read_public_key()
{
    std::ifstream infile("/home/df-admin/production/admin/tap_payment/public_key.txt");
    if (!infile.is_open())
    {
        std::cerr << "Failed to open file" << std::endl;
        return "";
    }
    // Read the file line by line
    std::string line;
    std::string public_key = "";
    while (std::getline(infile, line))
    {
        public_key += line;
    }
    // Close the file
    infile.close();
    return public_key;
}
std::map<std::string, std::string> readXmlPacket(std::string xmlString)
{
    std::string tag, attribute, value;             // variables to store tag, attribute, and value information
    std::map<std::string, std::string> dictionary; // dictionary to store parsed XML information
    int textStart = 0;
    int lenValue = 0;
    for (size_t i = 0; i < xmlString.length(); i++) // loop through each character in the XML string
    {
        if (xmlString[i] == '<' && xmlString[i + 1] != '/') // if the character is the start of a new tag
        {
            lenValue = 0;
            tag = xmlString.substr(i + 1, xmlString.find('>', i + 1) - i - 1); // extract the tag name
            dictionary[tag] = "";
            textStart = i + tag.length() + 2;
            lenValue = lenValue - tag.length() - 2;
        }
        else if (xmlString[i] == '<' && xmlString[i + 1] == '/')
        {
            value = xmlString.substr(textStart, lenValue + 1);
            dictionary[tag] = value;
            tag = "";
        }
        else
        {
            lenValue += 1;
        }
    }
    return dictionary;
}

std::string sendPacket(std::string command, int sockfd, bool logging)
{
    char buffer[4096];

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, command.c_str());
    // Send the message
    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
    {
        std::cerr << "Error sending message" << std::endl;
        return {};
    }
    memset(buffer, 0, sizeof(buffer));

    return "Packet sent";
}

void receiveAuthorizationThread(int sockfd)
{
    char bufferReceived[4096];
    memset(bufferReceived, 0, sizeof(bufferReceived));
    const char *delimiter = "</RESPONSE>";
    xml_packet_received_complete = false;
    while ((!xml_packet_received_complete && !stop_authorization_thread))
    {
        int bytes_received = recv(sockfd, bufferReceived, sizeof(bufferReceived), 0);
        if (bytes_received < 0)
        {
            std::cerr << "Error receiving message" << std::endl;
            break;
        }
        bufferReceived[bytes_received] = '\0';
        xml_packet_string += bufferReceived;
        if (strstr(xml_packet_string.c_str(), delimiter) != NULL)
        {
             std::cout << "received packet complete. " << std::endl;
            xml_packet_received_complete = true;
            break;
        }
    }
}

bool checkPacketReceived(bool logging, std::map<std::string, std::string> *xml_packet)
{ //
   //https://stackoverflow.com/questions/18849288/how-to-create-and-connect-slot-in-qt-to-tcp-recv-without-using-qtcpsockets
    if (xml_packet_received_complete || stop_authorization_thread)
    {
        if (logging)
        {
            std::cout << xml_packet_string << std::endl;
        }
        std::map<std::string, std::string> xmlObject = readXmlPacket(xml_packet_string);
        *xml_packet = xmlObject;
        xml_packet_string = "";
        xmlObject = {};
        xml_packet_received_complete = false;
        return true;
    }
    else
    {
        return false;
    }
}

void receiveCardTapAction()
{
    xml_card_tap_complete = false;
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[1024] = {};
    int opt = 1;
    int addrlen = sizeof(server_address);
    const int PORT = 5017;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsocket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("192.168.1.2");
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on " << PORT << std::endl;

    // Accept a connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Received connection from " << inet_ntoa(client_address.sin_addr) << std::endl;

    // Receive data from the client in a loop
    while ((!stop_tap_action_thread))
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
        {
            // If no more data is received, close the connection
            std::cout << "Connection closed by client." << std::endl;
            break;
        }
        else
        {
            xml_card_tapped += buffer;
            if (strstr(xml_card_tapped.c_str(), "TAPPED") != NULL)
            {
                xml_card_tap_complete = true;
                std::cout << "Received data from client: " << buffer << std::endl;
                close(client_socket);
                close(server_socket);
            }
            else if(strstr(xml_card_tapped.c_str(), "Failed to Read Card") != NULL)
            {
                xml_card_tap_failed = true;
                std::cout << "Received data from client: " << buffer << std::endl;
                close(client_socket);
                close(server_socket);
            }
            
        }
    }
}
std::tuple<bool , std::string> checkCardTapped(bool logging, std::map<std::string, std::string> *card_tapped)
{ 
    if(xml_card_tap_complete || stop_tap_action_thread){
        if (logging)
        {
            std::cout << xml_card_tapped << std::endl;
        }
        std::map<std::string, std::string> xmlObject = readXmlPacket(xml_card_tapped);
        
        *card_tapped = xmlObject;
        xml_card_tapped="";
        xmlObject = {};
        xml_card_tap_complete = false;
        return std::make_tuple(true, "Success");
    }
    else if(xml_card_tap_failed){
        return std::make_tuple(true, "Failed");
    }
    else{
        return std::make_tuple(false, "No response");
    }
        
    }

std::map<std::string, std::string> sendAndReceivePacket(std::string command, int sockfd, bool logging)
{
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, command.c_str());
    // Send the message
    int bytesSent = send(sockfd, buffer, strlen(buffer), 0);
    qDebug() << bytesSent;
    if (bytesSent < 0)
    {
        qDebug() << "Error sending message";
        std::cerr << "Error sending and receiving message" <<strerror(errno)<< std::endl;
        return {};
    }
    memset(buffer, 0, sizeof(buffer));
    char bufferReceived[4096];
    memset(bufferReceived, 0, sizeof(bufferReceived));
    const char *delimiter = "</RESPONSE>";
    bool found_delimiter = false;
    std::string xml_string;
    while (!found_delimiter)
    {
        int bytes_received = recv(sockfd, bufferReceived, sizeof(bufferReceived), 0);
        if (bytes_received < 0)
        {
            std::cerr << "Error receiving message" << std::endl;
            qDebug() << strerror(errno);
            break;
        }
        bufferReceived[bytes_received] = '\0';
        xml_string += bufferReceived;
        if (strstr(xml_string.c_str(), delimiter) != NULL)
        {
            found_delimiter = true;
        }
    }

    if (logging)
    {
        std::cout << xml_string << std::endl;
    qDebug() << QString::fromUtf8(xml_string.c_str());
    }
    std::map<std::string, std::string> xmlObject = readXmlPacket(xml_string);
    memset(bufferReceived, 0, sizeof(bufferReceived));
    return xmlObject;
}

int connectSocket()
{
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }
    // Set up the server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5015);
    if (inet_pton(AF_INET, "192.168.1.25", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Error converting IP address" << std::endl;
        return 1;
    }
    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }
    return sockfd;
}

int connectSecondarySocket()
{
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }
    // Set up the server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5016);
    if (inet_pton(AF_INET, "192.168.1.25", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Error converting IP address" << std::endl;
        return 1;
    }
    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }

    return sockfd;
}

std::vector<unsigned char> base64_decode(const std::string &input)
{
    // Initialize OpenSSL BIO
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    // Create an input memory buffer and link it to the BIO
    BIO *input_mem = BIO_new_mem_buf(input.c_str(), input.length());
    input_mem = BIO_push(b64, input_mem);
    // Determine the output size and allocate memory for it
    const size_t output_length = input.length() * 3 / 4;
    std::vector<unsigned char> output(output_length);

    // Decode the input and store the output in the output buffer
    const int bytes_written = BIO_read(input_mem, output.data(), input.length());
    output.resize(bytes_written);
    // Free resources
    BIO_free_all(input_mem);

    return output;
}

RSA *load_private_key(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        perror("Failed to open private key file");
        return NULL;
    }
    RSA *private_key = d2i_RSAPrivateKey_fp(fp, NULL);
    fclose(fp);
    return private_key;
}

std::vector<unsigned char> rsa_decrypt(RSA *private_key, const std::vector<unsigned char> &input)
{
    const int input_length = static_cast<int>(input.size());
    const int output_length = sqrt(RSA_size(private_key));
    // It is a hack! The number 32 should be logical or why it is 16. As of now, it is assumption that it is sqrt of output_length
    std::vector<unsigned char> output(output_length);
    const int bytes_decrypted = RSA_private_decrypt(input_length, input.data(), output.data(), private_key, RSA_PKCS1_PADDING);
    if (bytes_decrypted < 0)
    {
        perror("Failed to decrypt input data");
        return {};
    }
    return output;
}

std::vector<unsigned char> sha256_hash(const std::vector<unsigned char> &data)
{
    const int hash_size = EVP_MD_size(EVP_sha256());
    std::vector<unsigned char> hash(hash_size);

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    EVP_DigestFinal_ex(ctx, hash.data(), NULL);
    EVP_MD_CTX_free(ctx);

    return hash;
}

void update_hmac(HMAC_CTX *ctx, const std::vector<unsigned char> &data)
{
    HMAC_Update(ctx, data.data(), data.size());
}

std::vector<unsigned char> int_to_bytes(unsigned int value)
{
    std::vector<unsigned char> result;
    while (value > 0)
    {
        result.insert(result.begin(), static_cast<unsigned char>(value & 0xFF));
        value >>= 8;
    }
    return result;
}

void print_vector_as_hex(const std::vector<unsigned char> &vec)
{
    std::cout << std::hex << std::setfill('0');
    for (const auto &val : vec)
    {
        std::cout << std::setw(2) << static_cast<unsigned>(val);
    }
    std::cout << std::dec << std::endl;
}

std::string create_counter_mac(int counter, std::string encrypted_mac)
{
    std::vector<unsigned char> ciphertext = base64_decode(encrypted_mac);
    RSA *private_key = load_private_key("/home/df-admin/production/admin/tap_payment/private_key.der");
    if (private_key == NULL)
    {
        // Handle error
        std::cout << "Error in reading private key";
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring myWideString = std::to_wstring(counter);
    std::string myUtf8String = converter.to_bytes(myWideString);
    // Conversion to UTF-8
    std::vector<unsigned char> message(myUtf8String.begin(), myUtf8String.end());
    std::vector<unsigned char> mac_key = rsa_decrypt(private_key, ciphertext);
    // print_vector_as_hex(mac_key);
    HMAC_CTX *hmac_ctx = HMAC_CTX_new();
    HMAC_Init_ex(hmac_ctx, mac_key.data(), mac_key.size(), EVP_sha256(), NULL);

    update_hmac(hmac_ctx, message);
    unsigned int mac_length = HMAC_size(hmac_ctx);
    std::vector<unsigned char> mac(mac_length);
    HMAC_Final(hmac_ctx, mac.data(), &mac_length);
    HMAC_CTX_free(hmac_ctx);

    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    // Create a memory buffer to hold the base64-encoded HMAC
    BIO *mem = BIO_new(BIO_s_mem());
    BIO_push(b64, mem);

    // Write the HMAC bytes to the base64 encoder
    BIO_write(b64, mac.data(), mac.size());
    BIO_flush(b64);

    // Read the base64-encoded HMAC from the memory buffer
    char *encoded_hmac = nullptr;
    long encoded_hmac_len = BIO_get_mem_data(mem, &encoded_hmac);

    // Convert the base64-encoded HMAC to a std::string
    std::string encoded_hmac_string(encoded_hmac, encoded_hmac_len);
    // Clean up the BIO objects
    BIO_free_all(b64);
    return encoded_hmac_string;
}

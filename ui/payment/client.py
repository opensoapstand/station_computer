import socket

HOST = "192.168.1.64"  # The server's hostname or IP address
PORT = 5015  # The port used by the server
about_command = '<xml version="1.0" encoding="UTF-8"><header/><body><TRANSACTION> \
            <FUNCTION_TYPE>CONFIGURE</FUNCTION_TYPE> \
            <COMMAND>ABOUT</COMMAND>\
            </TRANSACTION><body/>'

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.send(about_command)

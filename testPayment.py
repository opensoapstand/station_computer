import socket


def connect_device():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('192.168.1.2', 5017)
    server_socket.bind(server_address)
    server_socket.listen(1)

    client_socket, client_address = server_socket.accept()
    data = client_socket.recv(2048)
    print(data) 
    server_socket.close()    
connect_device()
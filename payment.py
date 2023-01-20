import socket

HOST = '192.168.1.78'
PORT = 5015

about_command = '<TRANSACTION> \
                    <FUNCTION_TYPE>CONFIGURE</FUNCTION_TYPE> \
                        <COMMAND>ABOUT</COMMAND> \
                    </TRANSACTION>'

def connect_device():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        s.send(about_command.encode('UTF-8'))
        data = s.recv(1024)

    print(f"Received {data!r}")


def start_session():
    start_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>START</COMMAND>\
                        <COUNTER>1</COUNTER>\
                        <MAC> ... </MAC>\
                        <MAC_LABEL>REG2</MAC_LABEL>\
                    </TRANSACTION>'
    return start_command.encode('UTF-8')

def register_device(counter):
    if counter=='':
        counter = 1
    register_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>START</COMMAND>\
                        <COUNTER>'+ str(counter) + '</COUNTER>\
                        <MAC> ... </MAC>\
                        <MAC_LABEL>REG2</MAC_LABEL>\
                    </TRANSACTION>'
    return register_command.encode('UTF-8')

def finish_session():
    finish_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>FINISH</COMMAND>\
                        <COUNTER>1</COUNTER>\
                        <MAC> ... </MAC>\
                        <MAC_LABEL>REG2</MAC_LABEL>\
                    </TRANSACTION>'

def cancel_transaction():
    cancel_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECONDARY_PORT</FUNCTION_TYPE> \
                        <COMMAND>CANCEL</COMMAND> \
                    </TRANSACTION>'

def authorization(amount):
    auth_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                        <COMMAND>CAPTURE</COMMAND> \
                        <COUNTER>1</COUNTER>\
                        <MAC>………..</MAC>\
                        <MAC_LABEL>REG_2</MAC_LABEL> \
                        <TRANS_AMOUNT>'+amount+'</TRANS_AMOUNT>\
                        <ENCRYPT>TRUE</ENCRYPT>\
                    </TRANSACTION>'

    return auth_command.encode('UTF-8')
connect_device()
import socket
import base64
import hmac
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_v1_5
from lxml import etree
import time
import signal

HOST = '192.168.1.78'
PORT = 5015
encrypted_mac = 'CSpa8GKq4wLVHbRMxj+1zkOQxIOixYbbOPNsWe8qtoSVcTnHsI+kcjCWf+twCvNfuHpEWSwjloyzdQYI6hq/ekEZlyjkpdGvR7+NGOP84aTRXMl+CpBI1rxN4fBOHTaqwpWvYWiLpuXx1sVrzR2GyooI/IT+o3qony2idfLmByrK9flbgRUXU2671KUmmaFdhvS0UudUmDkYUfV6+glN4BPDGLdoTlyoD268p6bfQy2KOcBjP2/SSGL738PGxNdMSZ4vxbCVe4M2vbCL9edO7p2BKOE59xhncmHRnNBQyu+k4TpfQYnQt6iHz4t0itog7fLRE1TAaiSCytgbpxZgYw=='
MAC_LABEL = 'P_49S1XT'


def generate_rsa_keys():
    # Generate the key and write to a file
    key = RSA.generate(2048)
    private_key = key.exportKey(format="DER")
    public_key = key.publickey().exportKey(format="DER")
    pubkey_b64 = base64.b64encode(public_key).decode()

    with open("private_key.der", "wb") as key_file:
        key_file.write(private_key)

    with open("public_key.der", "w") as key_file:
        key_file.write(pubkey_b64)
    return pubkey_b64

def create_counter_mac(counter):
    #Decode MAC to base 64
    macKeyBase64decoded = base64.b64decode(encrypted_mac)
    #Set the counter value
    encoded_counter = bytes(counter, 'utf-8')
    # Read the private key file
    with open("private_key.der", "rb") as key_file:
        key_data = key_file.read()
    # Import the private key
    private_key = RSA.importKey(key_data)
    #Decrypt Mac key
    cipher = PKCS1_v1_5.new(private_key)
    macKeyDecrypted = cipher.decrypt(macKeyBase64decoded, None)

    signing_key = hmac.new(macKeyDecrypted, digestmod='sha256')
    # Update the HMAC object with the bytes of the data
    signing_key.update(encoded_counter)

    counter_mac = signing_key.digest()
    counter_mac = base64.b64encode(counter_mac).decode()
    return counter_mac

def get_next_counter_and_mac(active_socket):
    active_socket.send(get_counter(encrypted_mac,MAC_LABEL))
    data = active_socket.recv(2048)
    data = data.decode()
    root = etree.XML(data)
    counter_val = root.find('./COUNTER')
    counter_val = int(counter_val.text) +1
    counter_mac = create_counter_mac(str(counter_val))
    return str(counter_val), counter_mac


def register_device(public_key):
    register_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>9134</ENTRY_CODE>\
                        <KEY>'+public_key+'</KEY> \
                    </TRANSACTION>'
    return register_command.encode('UTF-8')

def unregisterall():
    register_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>UNREGISTERALL</COMMAND>\
                    </TRANSACTION>'
    return register_command.encode('UTF-8')


def start_session(counter_val, mac_key):
    start_command = '<TRANSACTION> \
                    <FUNCTION_TYPE>SESSION</FUNCTION_TYPE>\
                    <COMMAND>START</COMMAND>\
                    <BUSINESSDATE>20230128</BUSINESSDATE>\
                    <SWIPE_AHEAD>1</SWIPE_AHEAD>\
                    <TRAINING_MODE>0</TRAINING_MODE>\
                    <INVOICE>123456</INVOICE>\
                    <COUNTER>'+counter_val+'</COUNTER> \
                    <MAC>'+mac_key+'</MAC> \
                    <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    <POS_IP>\
                    </POS_IP>\
                    <POS_PORT>5017</POS_PORT>\
                    <NOTIFY_SCA_EVENTS>FALSE</NOTIFY_SCA_EVENTS>\
                    </TRANSACTION>'
    return start_command.encode('UTF-8')

def finish_session(counter_val, counter_mac):
    finish_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>FINISH</COMMAND>\
                        <COUNTER>'+counter_val+'</COUNTER>\
                        <MAC>'+counter_mac+'</MAC>\
                        <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    </TRANSACTION>'
    return finish_command.encode('UTF-8')

def cancel_transaction():
    cancel_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECONDARY_PORT</FUNCTION_TYPE> \
                        <COMMAND>CANCEL</COMMAND> \
                    </TRANSACTION>'

def authorization(amount,counter_val,counter_mac):
    auth_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                        <COMMAND>CAPTURE</COMMAND> \
                        <COUNTER>'+counter_val+'</COUNTER>\
                        <MAC>'+counter_mac+'</MAC>\
                        <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                        <TRANS_AMOUNT>'+amount+'</TRANS_AMOUNT>\
                        <MANUAL_ENTRY>FALSE</MANUAL_ENTRY>\
                        <FORCE_FLAG>FALSE</FORCE_FLAG> \
                        <ENCRYPT>TRUE</ENCRYPT>\
                    </TRANSACTION>'
    return auth_command.encode('UTF-8')


def get_counter(mac, mac_label):
    get_counter_command = '<TRANSACTION> \
                            <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE> \
                            <COMMAND>GET_COUNTER</COMMAND> \
                            <MAC>'+ mac + '</MAC>\
                            <MAC_LABEL>'+mac_label+'</MAC_LABEL> \
                        </TRANSACTION>'
    return get_counter_command.encode('UTF-8')

def TEST_MAC(counter,mac,mac_label):
    command = '<TRANSACTION> \
                    <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                    <COMMAND>TEST_MAC</COMMAND> \
                    <MAC_LABEL>'+mac_label+'</MAC_LABEL> \
                    <COUNTER>'+ counter+'</COUNTER> \
                    <MAC>'+mac+'</MAC> \
                </TRANSACTION>'
    return command.encode('UTF-8')

about_command = '<TRANSACTION> \
                    <FUNCTION_TYPE>CONFIGURE</FUNCTION_TYPE> \
                        <COMMAND>ABOUT</COMMAND> \
                        <MAC>'+encrypted_mac+'</MAC> \
                        <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                    </TRANSACTION>'


def connect_device():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(start_session(counter_val,counter_mac))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(authorization('1.00', counter_val, counter_mac))
        # s.send(finish_session(counter_val,counter_mac))
        data2 = s.recv(2048)

        print(data2)

connect_device()


import socket
import base64
import hmac
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_v1_5
from lxml import etree
import time
import signal
import configparser
from datetime import datetime
import random
import binascii

# config = configparser.ConfigParser()
# def initial_config_setup():
#     config.add_section("payment_config")
#     config.set("payment_config", "HOST", "192.168.0.1")
#     config.set("payment_config", "PORT", 5015)
#     config.set("payment_config", "secondary_port", 5016)
#     config.set("payment_config", "MAC_LABEL", "")
#     config.set("payment_config", "encrypted_mac", "")
#     with open('config.ini', 'w') as configfile:
#         config.write(configfile)
#     return "Initialized the config file"

# try:
#     config.read('config.ini')
#     encrypted_mac = config.get('payment_config', 'encrypted_mac')
#     MAC_LABEL = config.get('payment_config','MAC_LABEL')
#     HOST = config.get('payment_config','HOST')
#     PORT = int(config.get('payment_config','PORT'))
#     SECONDARY_PORT = int(config.get('payment_config', 'secondary_port'))
# except:
#     print("No initial setup yet. Running the initial setup function now")
#     initial_config_setup()

# encrypted_mac = 'cYu4M3K86YIePHAzlxnyQmegCOa16oh/wGNMAk5SpWVyq9saFkHFCDtKUkk3Ww1p4XcoEXBNtKRuFqB+T2VryP04WvP7fqXR2bPCVWToUwAsbJ3Vok6qRTMNWIs5hRGk6924abc2dFcEVjysDPrb65+OCZja0ZJ0a2EZse4pdQ0rK/i8/ONw4FZ8K1aKpcWFYA+QSJCU+8q51FJ1L4uLpdfsm45CFDDcxz5Ir3OD3YOY/9XJQw2zKlyuG/2RYDlL8vJPSEpJjGPCArPao8vltLrKj+bgLDcu9PYycK+cYu2bQ5fKwzRW7Sgk/yH8YimaWwT+nwg4wAqPmyFYMXVDpg=='
# MAC_LABEL = 'P_OD43FG'

INVOICE=2
HOST = "192.168.1.25"
encrypted_mac="ixOX9x532pKrRYIEnHfSTZIWMlfWujsG7apNb16Vp0/wKO+DYhh+TnTXDc4f6dt0w+6f3I8ebLVTxVZM0tDpscFRt+3rLUw6ut4PEfmlWMUQrG+AODspAVCoqr8Aya17/XH5fXdwAZIhrAX74QwfkemSzMf/oWWbFIOoQ5g5QUNEdZTHKPY9MCmh4zv3r2eLawQcuF+QtuWwx4pqVbc/UJaz9loMmt8xmhM/lhbsRbgPwTV4acgHqI4dtvS4Ti59ojmS9f+TrDSTo0jxcL6LBfHZ6RJwQRCm+DJuGTiTqNB7T5oR9l18ZYi6mndNgdE2t4PpvCh/B+4P6ZVZ8x4gHg=="
MAC_LABEL="P_G6BRBM"
PORT = 5015

def capture(counter_val, counter_mac, CTROUTD):
    command = '<TRANSACTION> \
                    <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                    <COMMAND>CAPTURE</COMMAND>\
                    <CTROUTD>'+CTROUTD+'</CTROUTD> \
                    <COUNTER>'+counter_val+'</COUNTER>\
                    <MAC>'+counter_mac+'</MAC>\
                    <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    <TRANS_AMOUNT>1.00</TRANS_AMOUNT>\
                    <PAYMENT_TYPE>CREDIT</PAYMENT_TYPE>\
                    <FORCE_FLAG>1</FORCE_FLAG>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')
def get_current_time():
    t = datetime.now()
    current_time = str(t).replace("-","").replace(" ","").replace(":","").split(".")[0]
    return current_time

def generate_rsa_keys():
    # Generate the key and write to a file
    key = RSA.generate(2048)
    private_key = key.exportKey(format="DER")
    public_key = key.publickey().exportKey(format="DER")
    # print(base64.b64encode(public_key))
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
    hex_str = binascii.hexlify(macKeyDecrypted).decode('utf-8')  # convert bytes to hex string
    print(hex_str)
    signing_key = hmac.new(macKeyDecrypted, digestmod='sha256')
    # Update the HMAC object with the bytes of the data
    signing_key.update(encoded_counter)
    counter_mac = signing_key.digest()
    counter_mac = base64.b64encode(counter_mac).decode()
    print(counter_mac)
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

def get_version(counter, mac):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>DEVICE</FUNCTION_TYPE> \
                <COMMAND>VERSION</COMMAND> \
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                <COUNTER>'+counter+'</COUNTER>\
                <MAC>'+mac+'</MAC>\
                <DISPLAY_VERSION>0</DISPLAY_VERSION> \
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def get_device_name(counter, mac):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>DEVICE</FUNCTION_TYPE> \
                <COMMAND>GET_DEVICENAME</COMMAND> \
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                <COUNTER>'+counter+'</COUNTER>\
                <MAC>'+mac+'</MAC>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def set_device_name(counter, mac, new_name):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>DEVICE</FUNCTION_TYPE> \
                <COMMAND>SET_DEVICENAME</COMMAND> \
                <DEVICENAME>'+new_name+'</DEVICENAME> \
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                <COUNTER>'+counter+'</COUNTER>\
                <MAC>'+mac+'</MAC>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')


def register_device(active_socket, public_key):
    register_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>9134</ENTRY_CODE>\
                        <KEY>'+public_key+'</KEY> \
                    </TRANSACTION>'
    # print(register_command)
    active_socket.send(register_command.encode('UTF-8'))
    data = active_socket.recv(2048)
    data = data.decode()
    root = etree.XML(data)
    encrypted_mac = root.find('./MAC_KEY')
    MAC_LABEL = root.find('./MAC_LABEL')
  
    config.set("payment_config", "MAC_LABEL", str(MAC_LABEL.text))
    config.set("payment_config", "encrypted_mac", str(encrypted_mac.text))
    with open('config.ini', 'w') as configfile:
        config.write(configfile)
    print("Registered")
    return MAC_LABEL

def unregisterall():
    unregister_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>UNREGISTERALL</COMMAND>\
                    </TRANSACTION>'
    print(unregister_command)
    return unregister_command.encode('UTF-8')

def unregister(MAC_LABEL):
    unregister_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>UNREGISTER</COMMAND>\
                        <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                    </TRANSACTION>'
    print(unregister_command)
    return unregister_command.encode('UTF-8')

def start_session(counter_val, mac_key):
    invoice = random.randint(0,1000000)
    print(invoice)
    start_command = '<TRANSACTION> \
                    <FUNCTION_TYPE>SESSION</FUNCTION_TYPE>\
                    <COMMAND>START</COMMAND>\
                    <BUSINESSDATE>20230213</BUSINESSDATE>\
                    <SWIPE_AHEAD>1</SWIPE_AHEAD>\
                    <TRAINING_MODE>0</TRAINING_MODE>\
                    <INVOICE>'+str(invoice)+'</INVOICE>\
                    <COUNTER>'+counter_val+'</COUNTER> \
                    <MAC>'+mac_key+'</MAC> \
                    <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    <POS_IP>\
                    </POS_IP>\
                    <POS_PORT>5015</POS_PORT>\
                    <NOTIFY_SCA_EVENTS>FALSE</NOTIFY_SCA_EVENTS>\
                    </TRANSACTION>'
    # print(start_command)
    return start_command.encode('UTF-8')

def finish_session(counter_val, counter_mac):
    finish_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>FINISH</COMMAND>\
                        <COUNTER>'+counter_val+'</COUNTER>\
                        <MAC>'+counter_mac+'</MAC>\
                        <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    </TRANSACTION>'
    print(finish_command)
    return finish_command.encode('UTF-8')

def cancel_transaction():
    cancel_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECONDARYPORT</FUNCTION_TYPE> \
                        <COMMAND>CANCEL</COMMAND> \
                    </TRANSACTION>'
    print(cancel_command)
    return cancel_command.encode('UTF-8')

def authorization(amount,counter_val,counter_mac):
    auth_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                        <COMMAND>AUTH</COMMAND> \
                        <COUNTER>'+counter_val+'</COUNTER>\
                        <MAC>'+counter_mac+'</MAC>\
                        <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL> \
                        <TRANS_AMOUNT>'+amount+'</TRANS_AMOUNT>\
                        <MANUAL_ENTRY>FALSE</MANUAL_ENTRY>\
                        <RECURRING>Y</RECURRING> \
                        <BILLPAY>TRUE</BILLPAY> \
                        <OC_INDUSTRY_CODE>M</OC_INDUSTRY_CODE> \
                        <ENCRYPT>TRUE</ENCRYPT> \
                        <SCMCI_INDICATOR>2</SCMCI_INDICATOR> \
                    </TRANSACTION>'    
    print(auth_command)           
    return auth_command.encode('UTF-8')

def get_counter(mac, mac_label):
    get_counter_command = '<TRANSACTION> \
                            <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE> \
                            <COMMAND>GET_COUNTER</COMMAND> \
                            <MAC>'+ mac + '</MAC>\
                            <MAC_LABEL>'+mac_label+'</MAC_LABEL> \
                        </TRANSACTION>'
    # print(get_counter_command)
    return get_counter_command.encode('UTF-8')

def TEST_MAC(counter,mac,mac_label):
    command = '<TRANSACTION> \
                    <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                    <COMMAND>TEST_MAC</COMMAND> \
                    <MAC_LABEL>'+mac_label+'</MAC_LABEL> \
                    <COUNTER>'+ counter+'</COUNTER> \
                    <MAC>'+mac+'</MAC> \
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')


def get_payment_types(counter_val, counter_mac):
    command = '<TRANSACTION>\
                <FUNCTION_TYPE>DEVICE</FUNCTION_TYPE> \
                <COMMAND>GET_PAYMENT_TYPES</COMMAND>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
            </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')
            
def display_message(counter_val, counter_mac):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>DEVICE</FUNCTION_TYPE>\
                <COMMAND>DISPLAY_MESSAGE</COMMAND>\
                <DISPLAY_TEXT>WELCOME TO Soapstand</DISPLAY_TEXT>\
                <TIMEOUT_DATA>10</TIMEOUT_DATA>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
            </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def check_approval(data):
    data = data.decode()
    root = etree.XML(data)
    print(data)
    CTROUTD = (root.find('./CTROUTD')).text
    AUTHCODE = (root.find('./AUTH_CODE')).text
    result = (root.find('./RESULT')).text
    return CTROUTD, result, AUTHCODE

def reboot():
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>SECONDARYPORT</FUNCTION_TYPE>\
                <COMMAND>REBOOT</COMMAND>\
                </TRANSACTION>'
    print(command)
    return command.encode

def deviceStatus():
    command = '<TRANSACTION>\
                <FUNCTION_TYPE>SECONDARYPORT</FUNCTION_TYPE>\
                <COMMAND>STATUS</COMMAND>\
                </TRANSACTION>'
    return command.encode('UTF-8')


def laneClosed(counter_val, counter_mac):
    command = '<TRANSACTION>\
                <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE> \
                <COMMAND>LANE_CLOSED</COMMAND>\
                <DISPLAY_TEXT>Lane Closed</DISPLAY_TEXT>\
                <FONT_COL_VALUE>FF0000</FONT_COL_VALUE>\
                <FONT_SIZE>72</FONT_SIZE>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def setTime(counter_val, counter_mac):
    current_time = get_current_time()
    command = '<TRANSACTION>\
                <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE>\
                <COMMAND>SETTIME</COMMAND>\
                <TIME>'+current_time+'</TIME>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
            </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def apply_updates(counter_val, counter_mac):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE>\
                <COMMAND>APPLYUPDATES</COMMAND>\
                <FLAG>TRUE</FLAG>\
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def credit(counter_val,counter_mac):
    command = '<TRANSACTION> \
                    <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE>\
                    <COMMAND>CREDIT</COMMAND>\
                    <COUNTER>'+counter_val+'</COUNTER>\
                    <MAC>'+counter_mac+'</MAC>\
                    <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    <PAYMENT_TYPE>CREDIT</PAYMENT_TYPE>\
                    <TRANS_AMOUNT>4.50</TRANS_AMOUNT>\
                    <OC_INDUSTRY_CODE>M</OC_INDUSTRY_CODE>\
                    <ENCRYPT>TRUE</ENCRYPT>\
                    <SCMCI_INDICATOR>2</SCMCI_INDICATOR>\
                    <TAX_IND>1</TAX_IND>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def creditWithCtroud(counter_val, counter_mac, CTROUTD):
    command = '<TRANSACTION> \
                    <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE>\
                    <COMMAND>CREDIT</COMMAND>\
                    <COUNTER>'+counter_val+'</COUNTER>\
                    <MAC>'+counter_mac+'</MAC>\
                    <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                    <CTROUTD>'+CTROUTD+'</CTROUTD>\
                    <PAYMENT_TYPE>CREDIT</PAYMENT_TYPE>\
                    <TRANS_AMOUNT>1.00</TRANS_AMOUNT>\
                    <OC_INDUSTRY_CODE>M</OC_INDUSTRY_CODE>\
                    <ENCRYPT>TRUE</ENCRYPT>\
                    <SCMCI_INDICATOR>2</SCMCI_INDICATOR>\
                    <TAX_IND>1</TAX_IND>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')


def voidTransaction(counter_val, counter_mac, CTROUTD):
    command = '<TRANSACTION>\
                <FUNCTION_TYPE>PAYMENT</FUNCTION_TYPE> \
                <COMMAND>VOID</COMMAND> \
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <CTROUTD>'+CTROUTD+'</CTROUTD>\
                <PAYMENT_TYPE>CREDIT</PAYMENT_TYPE>\
            </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def lastTransaction(counter_val, counter_mac):
    command =  '<TRANSACTION>\
                <FUNCTION_TYPE>REPORT</FUNCTION_TYPE> \
                <COMMAND>LAST_TRAN</COMMAND> \
                <COUNTER>'+counter_val+'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
            </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def querySAF(counter_val, counter_mac):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>SAF</FUNCTION_TYPE>\
                <COMMAND>QUERY</COMMAND>\
                <SAF_NUM_BEGIN>0</SAF_NUM_BEGIN>\
                <SAF_NUM_END>100</SAF_NUM_END>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <COUNTER>'+ counter_val +'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')

def removeSaf(counter_val, counter_mac):
    command = '<TRANSACTION> \
                <FUNCTION_TYPE>SAF</FUNCTION_TYPE>\
                <COMMAND>REMOVE</COMMAND>\
                <SAF_NUM_BEGIN>0</SAF_NUM_BEGIN>\
                <SAF_NUM_END>100</SAF_NUM_END>\
                <MAC_LABEL>'+MAC_LABEL+'</MAC_LABEL>\
                <COUNTER>'+ counter_val +'</COUNTER>\
                <MAC>'+counter_mac+'</MAC>\
                </TRANSACTION>'
    print(command)
    return command.encode('UTF-8')


def secondary_port():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,5016))
        s.send(cancel_transaction())
        data = s.recv(8192)
        print(data)
        return

def connect_device():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(querySAF(counter_val, counter_mac))
        s.send(unregisterall())
        data = s.recv(8192)
        print(data)
        # s.send(unregisterall())
        # register_device(s, generate_rsa_keys())

        # s.send(unregister(MAC_LABEL))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(TEST_MAC(counter_val, counter_mac, MAC_LABEL))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(start_session(counter_val,counter_mac))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(authorization('15.00', counter_val, counter_mac))
        # CTROUTD, result,AUTHCODE = check_approval(s.recv(8192))
        # print(result)
        # print(CTROUTD)
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(capture(counter_val, counter_mac,CTROUTD))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(credit(counter_val,counter_mac))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(voidTransaction(counter_val,counter_mac,'1688'))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(creditWithCtroud(counter_val,counter_mac, '1633'))
        # print(s.recv(4096))
        
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(set_device_name(counter_val, counter_mac, 'DRINKFILL_POS'))
        # s.send(get_device_name(counter_val, counter_mac))
        # s.send(get_version(counter_val, counter_mac))
        # s.send(get_payment_types(counter_val, counter_mac))
        # s.send(display_message(counter_val, counter_mac))
        # s.send(laneClosed(counter_val, counter_mac))
        # s.send(setTime(counter_val, counter_mac))
        # s.send(apply_updates(counter_val, counter_mac))

        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(lastTransaction(counter_val,counter_mac))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(querySAF(counter_val,counter_mac))
        # print(s.recv(4096))
        # counter_val, counter_mac = get_next_counter_and_mac(s)
        # s.send(finish_session(counter_val,counter_mac))
        # data2 = s.recv(8192)
        # print(data2)

connect_device()
# generate_rsa_keys()
# secondary_port()

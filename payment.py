import socket
import rsa
HOST = '192.168.1.78'
PORT = 5015
MAC = 'PY5WznnkiyHZrNssL9rti/r4eot48ycc/sPyGNG0bMhbQKhOHziZy7ABLR1Hfrs3wCcBcCiww8mc5siAZJUUyurLsZiNbCxnDjPM7u3pFpH4vZD/0dWw9lujxtYJft27ef5ja01Lnp0VbWrOUiBH31b269JPNhQ+2OWFrT2qhZnpooY/TPAVRIs6NRl6YPy7hsNNMKBoKJ1WWkRezlflK2XGzf2V25JlvvZILS2WmG4+hy2iUQhfZDStFH+LJ079GhccbHfKNWRA/QsTJAqYxtB1DCmVEzCeUoDnhNn2fGkuKyMU+029W6LLGk4L9c2BcjZZwxWo5o5oVV6H5O+A4A=='
MAC_LABEL = 'P_5H9NRT'

def register_device():
    register_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                        <COMMAND>REGISTER</COMMAND>\
                        <ENTRY_CODE>1234</ENTRY_CODE>\
                         <KEY>MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqEL8EhIr8+JHrtM80MwrnAucWQ967u+uswgEYoPQEFQa/yjWCtii5LbfwSG20LHiNX+uhbZrLIvg7oSRqQztNXebt30Nsrs69xUbOf62qM4J3wHUc95SA33Lo6eo36haITq0M7GPBIOU9QZFBjHoCT9I1eeyragivShg67zBKwQCYRC2BKuxeVid9xvCY/bU+YXBXlNbumVY9fv/WhqO/hkMv+95JR5OhlKyEEg/13KKyjE17f6h//FRPKrmv1APDSOMpOuoxQzV6zRfLZycpnRr2PVqgf2fq0fa+1KH6cgiAblwOqhQyUoQKH7QhRQ97HpQh0FCpONznFts131UPQIDAQAB</KEY> \
                        <REG_VER> 1 </ REG_VER> \
                    </TRANSACTION>'
    return register_command.encode('UTF-8')





def start_session():
    start_command = '<TRANSACTION> \
                        <FUNCTION_TYPE>SESSION</FUNCTION_TYPE> \
                        <COMMAND>START</COMMAND>\
                        <COUNTER>1</COUNTER>\
                        <MAC_LABEL>REG1</MAC_LABEL>\
                    </TRANSACTION>'
    return start_command.encode('UTF-8')

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


def get_counter(mac, mac_label):
    get_counter_command = '<TRANSACTION> \
                            <FUNCTION_TYPE>ADMIN</FUNCTION_TYPE> \
                            <COMMAND>GET_COUNTER</COMMAND> \
                            <MAC>'+ mac + '</MAC>\
                            <MAC_LABEL>'+mac_label+'</MAC_LABEL> \
                        </TRANSACTION>'
    return get_counter_command.encode('UTF-8')

#  <MAC>jbO3+7XRKTMW4y5c1OIcb38VZRSQTtozkDfQPKu4h9xO2Qyfrse8gyeyEN90MzAmaKA6RYjLeGAILviv9OB+m378XmjRq3n4fvqCPDVsDbzldkJqnZS3h5m7UfKiWLO+0wnURKguiQcMnBFAj5n9uYREnO1yBA+a802y2nJQlCbi8zlXYZfJ8cisLOOhIjH+SThHnRhkGpxjAPAicFnFhgLYSjsu6xw9eJ1pMGYMSCl6/8Resuw238iOPiH4KXBp15VD3bsiZiLcNcBWKzKiuOl7ah9G8AuHjikEnU1ZdxVWCDS2MMjXJBQmRkzmvHB+lA3F7amvWp67dTccfZ+kEg==</MAC> \
#                         <MAC_LABEL>P_CWSE2Q</MAC_LABEL> \
def TEST_MAC(counter,mac,mac_label):
    command = '<TRANSACTION> \
                    <FUNCTION_TYPE>SECURITY</FUNCTION_TYPE> \
                    <COMMAND>TEST_MAC</COMMAND> \
                    <COUNTER>'+ str(counter)+'</COUNTER> \
                    <MAC>' + mac + '</MAC>\
                    <MAC_LABEL>'+mac_label+'</MAC_LABEL> \
                </TRANSACTION>'
    return command.encode('UTF-8')

about_command = '<TRANSACTION> \
                    <FUNCTION_TYPE>CONFIGURE</FUNCTION_TYPE> \
                        <COMMAND>ABOUT</COMMAND> \
                        <MAC>jbO3+7XRKTMW4y5c1OIcb38VZRSQTtozkDfQPKu4h9xO2Qyfrse8gyeyEN90MzAmaKA6RYjLeGAILviv9OB+m378XmjRq3n4fvqCPDVsDbzldkJqnZS3h5m7UfKiWLO+0wnURKguiQcMnBFAj5n9uYREnO1yBA+a802y2nJQlCbi8zlXYZfJ8cisLOOhIjH+SThHnRhkGpxjAPAicFnFhgLYSjsu6xw9eJ1pMGYMSCl6/8Resuw238iOPiH4KXBp15VD3bsiZiLcNcBWKzKiuOl7ah9G8AuHjikEnU1ZdxVWCDS2MMjXJBQmRkzmvHB+lA3F7amvWp67dTccfZ+kEg==</MAC> \
                        <MAC_LABEL>P_CWSE2Q</MAC_LABEL> \
                    </TRANSACTION>'


def connect_device():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        # s.send(get_counter('P_CWSE2Q', 'jbO3+7XRKTMW4y5c1OIcb38VZRSQTtozkDfQPKu4h9xO2Qyfrse8gyeyEN90MzAmaKA6RYjLeGAILviv9OB+m378XmjRq3n4fvqCPDVsDbzldkJqnZS3h5m7UfKiWLO+0wnURKguiQcMnBFAj5n9uYREnO1yBA+a802y2nJQlCbi8zlXYZfJ8cisLOOhIjH+SThHnRhkGpxjAPAicFnFhgLYSjsu6xw9eJ1pMGYMSCl6/8Resuw238iOPiH4KXBp15VD3bsiZiLcNcBWKzKiuOl7ah9G8AuHjikEnU1ZdxVWCDS2MMjXJBQmRkzmvHB+lA3F7amvWp67dTccfZ+kEg=='))
        # s.send(get_counter(MAC,MAC_LABEL))
        s.send(TEST_MAC(1,MAC,MAC_LABEL))
        data = s.recv(2048)
        

        print(f"Received {data!r}")


connect_device()
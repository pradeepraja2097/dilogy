import socket
import json
UDP_IP = "0.0.0.0"
UDP_PORT = 5683
 
sock = socket.socket(socket.AF_INET, # Internet
           socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(4096) # buffer size is 1024 bytes
    print ("received message:", data)
    my_details={'server_port':addr,'client_ip':data}

    

    if data:
        send_data = b'DEWA Data Testing'
        #send_data = bytes(response_param, 'utf-8')
        sent = sock.sendto(send_data, addr)
        #print(response_param)
        print('sent {} bytes back to {}'.format(sent, addr))

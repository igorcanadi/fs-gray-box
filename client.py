import socket
import sys
PORT = 10056

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ('optimus.cs.wisc.edu', PORT)

sock.connect(server_address)
sock.sendall('CLEAR')
data = sock.recv(16)
if data == 'OK':
    print "yay!"
else:
    print "WTF"
sock.close()


import socket
import subprocess
import sys
PORT = 10056

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('optimus.cs.wisc.edu', PORT)
try:
    sock.bind(server_address)
    sock.listen(1)


    while True:
        print >> sys.stderr, "waiting for a connection"
        connection, client_address = sock.accept()
        print >> sys.stderr, "got a connection"
        data = connection.recv(30)
        print >> sys.stderr, "got %s" % data
        subprocess.check_output('sync', shell=True)
        subprocess.check_output('echo 3 >| /proc/sys/vm/drop_caches', shell=True)
        connection.sendall('OK')
        connection.close()
finally:
    sock.close()

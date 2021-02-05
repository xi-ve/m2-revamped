import socket
import json
from enum import IntEnum

class OpCodes(IntEnum):
    Add_Server = 1#Adds Servers
    Add_Forwarding = 2 #Add a Forwarding to a server for multiple channels, so you just keep 1 whiteliste
    Add_Ip = 3 #whitelists ip for x duration for a Server, 
    Remove_Server = 4
    Remove_Forwarding = 5
    Remove_Ip = 6

class m2proxy():

    def __init__(self,ip,port):
        self.sock = socket.socket()
        self.ip = ip
        self.port = port
        self.connect()

    def connect(self):
        self.sock.connect((self.ip,self.port))
    def read(self):
        CHUNK_SIZE = 16  # you can set it larger or smaller
        buffer = bytearray()
        while True:
            chunk = self.sock.recv(CHUNK_SIZE)
            buffer.extend(chunk)
            if b'\n' in chunk:
                break
        firstline = buffer[:buffer.find(b'\n')]
        return firstline.decode("utf-8")
    def Add_Ip(self,ip):
        self.sock.send("{}\n".format(json.dumps({"op":OpCodes.Add_Ip,"ip":ip})).encode('utf-8'))
        return self.read()


m2proxy = m2proxy("127.0.0.1",1337)
print(m2proxy.Add_Ip("127.0.0.1"))
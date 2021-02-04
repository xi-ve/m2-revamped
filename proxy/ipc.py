import socket
import json
from enum import IntEnum

class OpCodes(IntEnum):
    Add_Ip = 1

sock = socket.socket()

sock.connect(("127.0.0.1",1337))
sock.send("{}\n".format(json.dumps({"op":OpCodes.Add_Ip})).encode('utf-8'))
CHUNK_SIZE = 16  # you can set it larger or smaller
buffer = bytearray()
while True:
  chunk = sock.recv(CHUNK_SIZE)
  buffer.extend(chunk)
  if b'\n' in chunk:
    break
firstline = buffer[:buffer.find(b'\n')]
print(firstline.decode("utf-8"))
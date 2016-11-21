import socket
import time
import struct

s = socket.socket()
s.bind(("192.168.1.51", 9999))
s.listen(1)

sc, addr = s.accept()

while True:
      time.sleep(1)
      recibido = sc.recv(4)
      myint = struct.unpack("<L", recibido)[0]
      if myint == 9999:
         break

      print "Recibido: ", myint
      #print "Recibido:", recibido
      #sc.send(recibido)

print "adios"
sc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sc.close()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.close()


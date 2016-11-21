import socket  
  
s = socket.socket()   
s.connect(("192.168.1.51", 9999))  
  
while True:  
      mensaje = raw_input("> ")  
      s.send(mensaje)  
      if mensaje == "quit":  
         break  
  
print "adios"  
  
s.close()  

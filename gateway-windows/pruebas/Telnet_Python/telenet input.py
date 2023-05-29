import os,re,telnetlib, time
host = "localhost"
port = 7777

telnet = telnetlib.Telnet()
print ("abriendo conexion")
telnet.open(host, port)
print ("envia datos...")

#while True:
#    print(telnet.read_until(b"\n"))
time.sleep(1)
print("paso espera")
telnet.write(b"Hola que tal\n")

while True:
    print("recibiendo: ");
    print(telnet.read_until(b"\n"))

telnet.close()
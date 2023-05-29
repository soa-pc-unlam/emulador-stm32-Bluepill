import os,re,telnetlib
host = "localhost"
port = 7777

telnet = telnetlib.Telnet()
print ("abriendo conexion")
telnet.open(host, port)
print ("recibiendo datos...")

while True:
    print(telnet.read_until(b"\n"))

#print (telnet.read_all())
#print(tn.read_all().decode('ascii'))
telnet.close()
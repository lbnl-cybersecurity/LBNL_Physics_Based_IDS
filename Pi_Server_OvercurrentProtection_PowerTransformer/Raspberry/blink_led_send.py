import socket
import sys

TCP_IP = '127.0.0.1' #IP address of the Rasberry Pi
TCP_PORT = 27088
BUFFER_SIZE = 512
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
print 'connected to Raspberry Pi'

while true:
	iterations=raw_input("Enter the total number of times to blink")
	if (int(iterations)==0):
		break
	s.send(iterations)
	
s.close();	
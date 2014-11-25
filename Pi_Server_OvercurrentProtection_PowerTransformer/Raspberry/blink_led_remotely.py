import RPi.GPIO as GPIO ## Import GPIO Library
import time ## Import 'time' library.  Allows us to use 'sleep'
import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
server_address = ('127.0.0.1', 27088)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
# Listen for incoming connections
sock.listen(1)

GPIO.setmode(GPIO.BOARD) ## Use BOARD pin numbering
GPIO.setup(7, GPIO.OUT) ## Setup GPIO pin 7 to OUT

def Blink(numTimes, speed):
    for i in range(0,numTimes): ## Run loop numTimes
        print "Iteration " + str(i+1) ##Print current loop
        GPIO.output(7, True) ## Turn on GPIO pin 7
        time.sleep(speed) ## Wait
        GPIO.output(7, False) ## Switch off GPIO pin 7
        time.sleep(speed) ## Wait
    print "Done" ## When loop is complete, print "Done"
    GPIO.cleanup()

while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()
    try:
        print >>sys.stderr, 'connection from', client_address
		break

while true:
   iterations=connection.recv(160)
   speed=0.25 #seconds
   Blink(int(iterations),float(speed))


## Prompt user for input
#iterations = raw_input("Enter the total number of times to blink: ")
#speed = raw_input("Enter the length of each blink in seconds: ")
        
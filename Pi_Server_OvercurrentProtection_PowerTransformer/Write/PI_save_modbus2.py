'''
Packet sniffer in python using the pcapy python library
 
Project website
 
http://oss.coresecurity.com/projects/pcapy.html
 
 This script stored modbus packet in the pi server. 
 
 The Tags used are:
plc_source_ip
plc_dest_ip
plc_trans_identify
plc_protocol_identify
plc_length_modbus
plc_unit_identifier
plc_function_code
plc_reference_number
plc_bit_count
plc_byte_count
plc_payload
plc_query_or_response
plc_timestamp
plc_packetnumber

 TODO for the user:
 run the c++ code with the following argument:
 gaiavm1.ece.ucdavis.edu plc_source_ip plc_dest_ip plc_trans_identify plc_protocol_identify plc_length_modbus plc_unit_identifier plc_function_code plc_reference_number plc_bit_count plc_byte_count plc_payload plc_query_or_response plc_timestamp plc_packetnumber

 Note when reading from a file there is a 1second gap between each modbus packet. In if capturing life it runs realtime.
'''
# Author - Vishak Muthukumar and Reinhard Gentz
import socket
from struct import *
import datetime
import pcapy
import sys
from operator import xor
import time
#import datetime



TCP_IP = '127.0.0.1' #IP address of the PI Server
#TCP_IP = '169.237.74.55' #IP address of the PI Server 
TCP_PORT = 27042
BUFFER_SIZE = 1024
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
packet_counter = 1
pause_requested=0

def main(argv):
    #Variables
    #ask user to enter device name to sniff
    print("Menu ----\n1. Monitoring interface \n2. Offline monitoring using pcap files.\n")
    choice=input("Enter your choice: ")
    #choice=2
    if choice == 1:
        pause_requested=0 #no sleeping time later on
        devices = pcapy.findalldevs()
        

        print("Available devices are :")
        for d in devices :
            print(d)
     
        dev = input("Enter device number to sniff : ")
     
        print("Sniffing device " + devices[dev])
     
        '''
        open device
        # Arguments here are:
        #   device
        #   snaplen (maximum number of bytes to capture _per_packet_)
        #   promiscious mode (1 for true)
        #   timeout (in milliseconds)
        '''
        cap = ppycapy.open_live(devices[dev] , 65536 , 1 , 0)
    elif choice == 2:
        trace=raw_input("type name of the pcap file ")
        #trace="netTraffic1"
        cap = pcapy.open_offline(trace+".pcap")
    #start sniffing packets
    while(1) :
        try:
            (header, packet) = cap.next()
            #print ('%s: captured %d bytes, truncated to %d bytes' %(datetime.datetime.now(), header.getlen(), header.getcaplen()))
            parse_packet(packet)
        except pcapy.PcapError:
            continue
 
#Convert a string of 6 characters of ethernet address into a dash separated hex string
def eth_addr (a) :
    b = "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x" % (ord(a[0]) , ord(a[1]) , ord(a[2]), ord(a[3]), ord(a[4]) , ord(a[5]))
    return b


#function to parse a packet
def parse_packet(packet) :
     #globals
    global transidread,transidwrite,flag,master_ip,slave_ip,pause_requested, packet_counter
    #parse ethernet header
    eth_length = 14
    
    eth_header = packet[:eth_length]
    eth = unpack('!6s6sH' , eth_header)
    eth_protocol = socket.ntohs(eth[2])
    #print('Destination MAC : ' + eth_addr(packet[0:6]) + ' Source MAC : ' + eth_addr(packet[6:12]) + ' Protocol : ' + str(eth_protocol))
 
    #Parse IP packets, IP Protocol number = 8
    if eth_protocol == 8 :
        #Parse IP header
        #take first 20 characters for the ip header
        ip_header = packet[eth_length:20+eth_length]
         
        #now unpack them :)
        iph = unpack('!BBHHHBBH4s4s' , ip_header)
 
        version_ihl = iph[0]
        version = version_ihl >> 4
        ihl = version_ihl & 0xF
 
        iph_length = ihl * 4
 
        ttl = iph[5]
        protocol = iph[6]
        s_addr = socket.inet_ntoa(iph[8]);
        d_addr = socket.inet_ntoa(iph[9]);
        #print s_addr
        #print('Version : ' + str(version) + ' IP Header Length : ' + str(ihl) + ' TTL : ' + str(ttl) + ' Protocol : ' + str(protocol) + ' Source Address : ' + str(s_addr) + ' Destination Address : ' + str(d_addr))
        
        #TCP protocol
        if protocol == 6 :
            t = iph_length + eth_length
            tcp_header = packet[t:t+20]
 
            #now unpack them :)
            tcph = unpack('!HHLLBBHHH' , tcp_header)
             
            source_port = tcph[0]
            dest_port = tcph[1]
            sequence = tcph[2]
            acknowledgement = tcph[3]
            doff_reserved = tcph[4]
            tcph_length = doff_reserved >> 4
            h_size = eth_length + iph_length + tcph_length * 4
            data_size = len(packet) - h_size
            data = packet[h_size:]
            hex_packet=[]
            for elt in data:
                hex_packet.append(elt.encode("hex"))

            #print('Source Port : ' + str(source_port) + ' Dest Port : ' + str(dest_port) + ' Sequence Number : ' + str(sequence) + ' Acknowledgement : ' + str(acknowledgement) + ' TCP header length : ' + str(tcph_length))

            #filter too small packets that would cause this program to crash
            if len(hex_packet)<10:
                #print 'The packet recieved is too small, discarding packet'
                return
            if len(hex_packet)>20:
                #print 'Ethernet Sequence incorrect, ignoring'
                return
            
            ############################################################
            #last_packet_query='0'
            #filter packets based on port
            if int(str(source_port),10) == 502:
                print('received a response, fc is ' + str(int("".join(hex_packet[7]),16)))
                timestamp=datetime.datetime.now().isoformat()

                ############################################################
                #if last_packet_query=='R':
                #    print 'Response Attack'
                #last_packet_query='R'
                
                #Save source
                s.send(str(s_addr))
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                #Save destination #should be the master ip
                s.send(str(d_addr))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save transaction identifier
                transidentify= int("".join(hex_packet[:2]),16)
                s.send(str(transidentify))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save protocol identifier
                protocol_identifyer= int("".join(hex_packet[2:4]),16)
                s.send(str(protocol_identifyer))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save length
                length_modbus= int("".join(hex_packet[4:6]),16)
                s.send(str(length_modbus))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save Unit Identifier #I think for us this is constant 255
                unit_identifier= int("".join(hex_packet[6]),16)
                s.send(str(unit_identifier))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save Function Code
                function_code= int("".join(hex_packet[7]),16)
                s.send(str(function_code))    
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                if function_code==4:
                    #Save Reference Number dummy
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    #Send  dummie because we do not have Bit Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Send Byte Count
                    byte_count= int("".join(hex_packet[8]),16)
                    s.send(str(byte_count))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    #Save the payload data #e.g the ack from the write cmd or the value from the read response
                    payload_data= int("".join(hex_packet[9:]),16)
                    s.send(str(payload_data))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 

                elif function_code==15:
                    #Save Reference Number
                    reference_number= int("".join(hex_packet[8:10]),16)
                    s.send(str(reference_number))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    #Send  dummie because we do not have Bit Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Send  dummie because we do not have Byte Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    #Save the payload data #e.g the ack from the write cmd or the value from the read response
                    payload_data= int("".join(hex_packet[10:]),16)
                    s.send(str(payload_data))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 

                else:
                    print 'unsupported function code received, all data after function code stored in the payloadtag '
                    #Save Reference Number dummy
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    #Send  dummie because we do not have Bit Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Send  dummie because we do not have Byte Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Save the payload data #e.g the ack from the write cmd or the value from the read response
                    payload_data= int("".join(hex_packet[8:]),16)
                    s.send(str(payload_data))
                    print "payload_data unreferenced"
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                
                #Save that this is a response
                s.send("R")
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                #Send timestamp
                timestamp=datetime.datetime.now().isoformat()
                s.send(timestamp)
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                #Count the packet_counter
                s.send(str(packet_counter))
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                packet_counter=packet_counter+1
                #done
                #to make it easier to stop:
                time.sleep(pause_requested)
                
            if int(str(dest_port),10) == 502:
                print('received a query, fc is '+ str(int("".join(hex_packet[7]),16)))
                timestamp=datetime.datetime.now().isoformat()

                ############################################################
                #if last_packet_query=='Q':
                #    print 'Querry Attack'
                #last_packet_query='Q'
                #print 'Q'
                
                 #Save source
                s.send(str(s_addr))
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                #Save destination #should be the master ip
                s.send(str(d_addr))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save transaction identifier
                transidentify= int("".join(hex_packet[:2]),16)
                s.send(str(transidentify))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save protocol identifier
                protocol_identifyer= int("".join(hex_packet[2:4]),16)
                s.send(str(protocol_identifyer))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save length
                
                length_modbus= int("".join(hex_packet[4:6]),16)
                #print 'packet is' , hex_packet
                #print 'length modbus' , hex_packet[4,5] , 'is infact length_modbus'
                s.send(str(length_modbus))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save Unit Identifier #I think for us this is constant 255
                unit_identifier= int("".join(hex_packet[6]),16)
                s.send(str(unit_identifier))
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save Function Code
                function_code= int("".join(hex_packet[7]),16)
                s.send(str(function_code))    
                data = s.recv(BUFFER_SIZE) #the same value id sent back 
                #Save Reference Number
                reference_number= int("".join(hex_packet[8:10]),16)
                s.send(str(reference_number))
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                if(function_code==15): #then we have the bit and byte count
                    #Save Bit count
                    bit_count= int("".join(hex_packet[10:12]),16)
                    s.send(str(bit_count))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    #Save Byte count
                    byte_count= int("".join(hex_packet[12]),16)
                    s.send(str(byte_count))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back
                    # Save the Send payload
                    payload_data= int("".join(hex_packet[13:]),16)
                    s.send(str(payload_data))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    
                elif(function_code==4):
                    #Send  dummie because we do not have Bit Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Send  dummie because we do not have Byte Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Save the payload data #e.g the ack from the write cmd or the value from the read response
                    payload_data= int("".join(hex_packet[10:]),16)
                    s.send(str(payload_data))
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                else:
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Send  dummie because we do not have Byte Count
                    s.send("-1")
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 
                    #Save the payload data #e.g the ack from the write cmd or the value from the read response
                    payload_data= int("".join(hex_packet[10:]),16)
                    s.send(str(payload_data))
                    #print "unsupported fc: ", str(payload_data)
                    payload_data= int("".join(hex_packet[:]),16)
                    

                    print "unsupported fc: "
                    print hex_packet[:]
                    data = s.recv(BUFFER_SIZE) #the same value id sent back 

                            
                #Save that this is a query
                s.send("Q")
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                #Send timestamp
                
                s.send(timestamp)
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                #Count the packet_counter
                s.send(str(packet_counter))
                data = s.recv(BUFFER_SIZE) #the same value id sent back
                packet_counter=packet_counter+1
                #done
                #to make it easier to stop:
                time.sleep(pause_requested)
                        
                        
if __name__ == "__main__":
    main(sys.argv)

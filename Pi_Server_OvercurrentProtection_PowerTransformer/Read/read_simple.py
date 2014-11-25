'''
Reading Tags from the pyserver

Author Reinhard Gentz

1)Run the CPP code with argument 'gaiavm1.ece.ucdavis.edu'
2)Run this code and request data for the Tag of interest as follows:

1) s.send('Tagname Number_of_Items_Req 22-Sep-2014 11:51:41:882795') #keep the notation exactly as is
2)then cpp will answer for as many times as specified, see example code below.
3) Each Item needs to be 'acked'
4) Once all items are transfered, cpp will send an 'req done'
5) You can start with the next tag/time: See 1)
 
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
'''


import socket
import sys

def getDataFromPi(tagname, number, req_timestamp):
    #sample call: values,timestamp=getDataFromPi('plc_source_ip','3','22-Sep-2014 11:51:41:882795')
    joined_string= []
    joined_string.append(tagname)
    joined_string.append(number)
    joined_string.append(req_timestamp)
    string_to_send=' '.join(joined_string)
    s.send(string_to_send)
    output=[]
    timestamp=[]
    for x in range(0,int(number)):
        output.append(s.recv(BUFFER_SIZE))
        s.send('ack')
        timestamp.append(s.recv(BUFFER_SIZE))              
        s.send('ack')
    ack = s.recv(BUFFER_SIZE)
    return output, timestamp

def getDataFromPiAtLaterTime(tagname, number, req_timestamp):
    #This function does start after the first item. Thus if calling with the exact timestamp the item at the exact timestamp is not returned
    numberint= (int(number)+1)
    number=str(numberint)
    output1,timestamp=getDataFromPi(tagname, number, req_timestamp)
    output1=output1[1:]
    timestamp=timestamp[1:]
    return output1, timestamp

    
# Create a TCP/IP socket

TCP_IP = '127.0.0.1' #IP address of the CPP-Script
TCP_PORT = 27038
BUFFER_SIZE = 512
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
print 'connected to cpp:'
print

number_of_items_each_run='5'
#initial timestamp
timestamp=['08-Oct-2014 10:28:06.606323']
print 'Requested initial timestamp is ', timestamp
counter=0
while True:
    if counter==0: #only needed to not skip the first value
        #getting the first value
        plc_source_ip,timestamp1=getDataFromPi('plc_source_ip',number_of_items_each_run,timestamp[-1])
        plc_dest_ip,timestamp1=getDataFromPi('plc_dest_ip',number_of_items_each_run,timestamp[-1])
        #....
        plc_query_or_response, timestamp1 = getDataFromPi('plc_query_or_response',number_of_items_each_run,timestamp[-1])
        plc_function_code,timestamp=getDataFromPi('plc_function_code',number_of_items_each_run,timestamp[-1])
        counter=1
    else:
        #skipping the first value returned.
        plc_source_ip,timestamp1=getDataFromPiAtLaterTime('plc_source_ip',number_of_items_each_run,timestamp[-1])
        plc_query_or_response, timestamp1 = getDataFromPiAtLaterTime('plc_query_or_response',number_of_items_each_run,timestamp[-1])
        plc_dest_ip,timestamp1=getDataFromPiAtLaterTime('plc_dest_ip',number_of_items_each_run,timestamp[-1])
        #....
        plc_function_code,timestamp=getDataFromPiAtLaterTime('plc_function_code',number_of_items_each_run,timestamp[-1])
       
    
    #notice the difference between timestamp and timestamp1!!! otherwise you overwrite the current value requested
    #print 'sourceip is:', plc_source_ip
    #print 'dest ip is:',plc_dest_ip
    #print 'function code is:',plc_function_code
    print 'timestamp is:',timestamp
    print 'Query or response is: ', plc_query_or_response
    #comment this break if you want to run forever, note the sript will slow down and wait if no more new items are available
    #break

print 'Python done'

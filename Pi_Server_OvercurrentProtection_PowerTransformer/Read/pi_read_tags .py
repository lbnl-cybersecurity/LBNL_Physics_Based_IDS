####### Gina Koutsandria

import socket
import sys
import time
import datetime
import math

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

number_of_items_each_run = '50'
requested_number_of_packets = 50
#initial timestamp
timestamp = ['10-Oct-14 16:45:00.230469']
print 'Requested initial timestamp is ', timestamp

I_pickUp = 380
first_packet = 0
while True:
    log_file = open("log_file.txt", 'a')
    block = 1
    counter = 0
    packet_counter = 1
    iteration = 0
   
    if first_packet == 0: #only needed to not skip the first value
        #getting the first value
        plc_source_ip,timestamp1 = getDataFromPi('plc_source_ip',number_of_items_each_run,timestamp[-1])
        plc_dest_ip,timestamp1 = getDataFromPi('plc_dest_ip',number_of_items_each_run,timestamp[-1])
        plc_query_or_response, timestamp1 = getDataFromPi('plc_query_or_response',number_of_items_each_run,timestamp[-1])
        plc_payload, timestamp1 = getDataFromPi('plc_payload',number_of_items_each_run,timestamp[-1])
        #....
        plc_function_code,timestamp = getDataFromPi('plc_function_code',number_of_items_each_run,timestamp[-1])
        first_packet = 1

    else:
        #skipping the first value returned.
        plc_source_ip,timestamp1 = getDataFromPiAtLaterTime('plc_source_ip',number_of_items_each_run,timestamp[-1])
        plc_dest_ip,timestamp1 = getDataFromPiAtLaterTime('plc_dest_ip',number_of_items_each_run,timestamp[-1])
        plc_query_or_response, timestamp1 = getDataFromPiAtLaterTime('plc_query_or_response',number_of_items_each_run,timestamp[-1])
        plc_payload, timestamp1 = getDataFromPiAtLaterTime('plc_payload',number_of_items_each_run,timestamp[-1])
        #....
        plc_function_code,timestamp=getDataFromPiAtLaterTime('plc_function_code',number_of_items_each_run,timestamp[-1])


    
    alert_counter = 6
    timestamp_read = []
    timestamp_write = []
    difference = []
    mean = []
    print plc_function_code
    print plc_query_or_response
    print plc_payload
    variance_list = []
    sumation_list = []
    overcurrent_events = []
    plc_payload1 = []
    cycle = 0
    print "Block:"
    print block
    for x1 in range(0,requested_number_of_packets):
        
        #NIDS1--check for not acceptable function codes in the requested block of packets
        print plc_function_code[x1]
        print plc_query_or_response[x1]
        print timestamp[x1]
        plc_payload1.append(plc_payload[x1])
       
        if plc_function_code[x1] != '4' and plc_function_code[x1] != '15':
            print 'Illegal action detected: function code used is not included in the list of acceptable function codes. Packet timestamp:', timestamp[x1]
            log_file.write("Function code used(" + plc_function_code[x1] + ") is not included in the list of acceptable function codes. Packet timestamp: " + timestamp[x1] + "\n")

        #Check packet sequence+physical constraints (NIDS2+NIDS3+NIDS4)           
        if packet_counter == 1:
            if plc_query_or_response[x1] == 'Q' and plc_function_code[x1] == '4':
                print 'Read Query'                        
                packet_counter += 1
                #this line only works on python 2.7
                t1 = datetime.datetime.strptime(timestamp[x1], "%d-%b-%Y %H:%M:%S.%f")
                #print t1
            elif plc_query_or_response[x1] == 'Q' and plc_function_code[x1] == '15':
                print 'NIDS will be activated at the end of the cycle.'
                packet_counter = 1
            elif plc_query_or_response[x1] == 'R' and plc_function_code[x1] == '15':
                print 'NIDS will be activated at the end of the cycle.'
                packet_counter = 1
            elif plc_query_or_response[x1] == 'R' and plc_function_code[x1] == '4':
                print 'NIDS will be activated at the end of the cycle.'
                packet_counter = 1
            else:
                print 'Illegal action detected: wrong packet sequence observed: no read query'
                log_file.write("Possible attemp of attack1. Wrong packet sequence observed: no read query. Timestamp: " + timestamp[x1] + "\n")
                alert_counter += 1
                packet_counter = 1
          
            '''
            if iteration == 0:
                if plc_query_or_response[x1] == 'Q' and plc_function_code[x1] == '4':
                    print 'Read Query'                        
                    packet_counter += 1
                    #this line only works on python 2.7
                    t1 = datetime.datetime.strptime(timestamp[x1], "%d-%b-%Y %H:%M:%S.%f")
                    print t1
                elif plc_query_or_response[x1] == 'Q' and plc_function_code[x1] == '15':
                    print 'NIDS will be activated at the end of the cycle.'
                    packet_counter = 1
                elif plc_query_or_response[x1] == 'R' and plc_function_code[x1] == '15':
                    print 'NIDS will be activated at the end of the cycle.'
                    packet_counter = 1
                elif plc_query_or_response[x1] == 'R' and plc_function_code[x1] == '4':
                    print 'NIDS will be activated at the end of the cycle.'
                    packet_counter = 1
                else:
                    print 'Illegal action detected: wrong packet sequence observed: no read query'
                    log_file.write("Possible attemp of attack1. Wrong packet sequence observed: no read query. Timestamp: " + timestamp[x1] + "\n")
                    alert_counter += 1
                    packet_counter = 1
            else:
                if plc_query_or_response[x1] == 'Q' and plc_function_code[x1] == '4':
                    print 'Read Query'
                    packet_counter += 1
                    #this line only works on python 2.7
                    t1 = datetime.datetime.strptime(timestamp[x1], "%d-%b-%Y %H:%M:%S.%f")
                    print t1
                else:
                    print 'Illegal action detected: wrong packet sequence observed: no read query'
                    log_file.write("Possible attemp of attack1. Wrong packet sequence observed: no read query. Timestamp: " + timestamp[x1] + "\n")
                    alert_counter += 1
                    packet_counter = 1
            iteration += 1
            '''
        elif packet_counter == 2:
            if plc_query_or_response[x1] == 'R' and plc_function_code[x1] == '4':
                counter_gina = 0
                print 'Read Response'
                
                packet_counter += 1
                print 'plc_payloas is: ' , plc_payload[x1]
                if int(plc_payload[x1])>=380:
                    print "Overcurrent"
   
                    counter_gina =1
                    cycle = 5
                    overcurrent_events.append(1)
                else:
                    counter_gina = 0
                    print "No Overcurrent"
                    overcurrent_events.append(0)
                    cycle -= 1                      
           
            else:
                print 'Illegal action detected: wrong packet sequence observed: no read response'
                log_file.write("Possible attemp of attack2. Wrong packet sequence observed: no response query. Timestamp: " + timestamp[x1] + "\n")
                alert_counter += 1
                packet_counter = 2
        elif packet_counter == 3:
            if plc_query_or_response[x1] == 'Q' and plc_function_code[x1] == '15':
                print 'Write Query'
                packet_counter += 1
                print "frefsddddddffffffffffffffffffffffffffffffffffffffff"
                print counter_gina
                print plc_payload[x1]
                print cycle
                    
                if counter_gina == 1 and plc_payload[x1] == '3':
                    print "7 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                    log_file.write("7Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                    
                elif counter_gina == 0 and plc_payload[x1] == '0':
                    if cycle == 4:
                        print overcurrent_events
                        print overcurrent_events[0]
                        print x1
                        print block
                        print overcurrent_events[block-1]
                        print overcurrent_events[block-2]
                        if(overcurrent_events[block-2] == 1):
                            print "it comes here"
                        else:
                            print "1 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                            log_file.write("1Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                    elif cycle == 3:
                        if(overcurrent_events[block-3] == 1):
                            print "gina"
                        else:
                            print "2 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                            log_file.write("2Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                    elif cycle == 2:
                        if(overcurrent_events[block-4] == 1):
                            print "gina"
                        else:
                            print "3 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                            log_file.write("3Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                    elif cycle == 1:
                        if(overcurrent_events[block-5] == 1):
                            print "gina"
                        else:
                            print "4 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                            log_file.write("4Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                    elif cycle == 0:
                        if(overcurrent_events[block-6] == 1):
                            print "gina"
                        else:
                            print "5 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                            log_file.write("5Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                    else:
                            print "6 Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault."
                            log_file.write("6Possible attemp of attack. Circuit breakers are activated without the ocurrence of an overcurrent fault. Timestamp: " + timestamp[x1] + "\n")
                   
                '''
                #this line only works on python 2.7
                t2 = datetime.datetime.strptime(timestamp[x1], "%d-%b-%Y %H:%M:%S.%f")
                #print t2
                test = t2-t1
                test1 = ((test.seconds * (1000000)) + test.microseconds)
                if block!= 1:
                    difference.append(test1)
                    #mu = (1/block) * sum (difference)
                    mu = 0
                    sum_square = 0
                    for m in range(0, len(difference)):
                        #print m
                        #print len(difference)
                        mu = mu + difference[m]
                        sum_square = sum_square + (difference[m]*difference[m])
                    
                    mu = mu/len(difference)
                    sum_square = sum_square/len(difference)
                    
                    mean.append(mu)
                    #print mean[block-1]
                    variance = sum_square - (mu*mu)
                    variance_list.append(variance)
                    #print variance_list[block-1]
                    check = abs(test1 - mean[block-2])
                    print "check"
                    print check
                    calculation = 0.05*(test1*test1)+0.95 *sumation_list[block-2]
                    sumation_list.append(calculation)
                    print"calculation"
                    print math.sqrt(calculation)
                    
                    if check > 1*math.sqrt(sumation_list[block-2]):
                        print "Attackkkkkkk"
                        log_file.write("Possible attemp of attack. Packets issued with a lower frequency. Timestamp: " + timestamp[x1] + "\n")
                else:
                    mu = test1
                    mean.append(mu)

                    sum_square = test1*test1                    
                    variance = sum_square - (mu*mu)
                    variance_list.append(variance)

                    calculation = 0.05 * (test1*test1)
                    sumation_list.append(calculation)
                    
                    print sumation_list[block-1]
            '''   
            else:
                print 'Illegal action detected: wrong packet sequence observed: no write query'
                print 'function code'
                print plc_function_code[x1]
                print 'type'
                print plc_query_or_response[x1]
                log_file.write("Possible attemp of attack3. Wrong packet sequence observed: no write query. Timestamp: " + timestamp[x1] + "\n")
                alert_counter += 1
                packet_counter = 3
        elif packet_counter == 4:
            if plc_query_or_response[x1] == 'R' and plc_function_code[x1] == '15':
                print 'Write Response'
                packet_counter = 1
                
            else:
                print 'Illegal action detected: wrong packet sequence observed: no write response'
                log_file.write("Possible attemp of attack4. Wrong packet sequence observed: no write response. Timestamp: " + timestamp[x1] + "\n")
                alert_counter += 1
                packet_counter = 4
            print "Block:"
            print block
            block += 1
            
                
        if x1 == (requested_number_of_packets):
            if plc_query_or_response[(requested_number_of_packets)] != 'R' or plc_function_code[(requested_number_of_packets)] !='15':
                print 'Cycle is not complete'
           
    #notice the difference between timestamp and timestamp1!!! otherwise you overwrite the current value requested
    #print 'plc_payload', plc_payload
    #print 'plc_query_or_response', plc_query_or_response
    #print 'sourceip is:', plc_source_ip
    #print 'dest ip is:',plc_dest_ip
    #print 'function code is:',plc_function_code
    #print 'timestamp is:',timestamp
   
    #comment this break if you want to run forever, note the sript will slow down and wait if no more new items are available
    #break

print 'Python done'


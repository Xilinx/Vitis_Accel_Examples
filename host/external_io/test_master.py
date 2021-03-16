#!/usr/local/bin/python3
from xilinx_xtlm import ipc_axis_master_util
from xilinx_xtlm import xtlm_ipc 

import random #for Random pattern
import string
import struct
import time

#Stores data sent to Device
send_file = open("send_data.txt", "w") 
#This is expected data from device [We know the functionality of kernel in this case]
expected_file = open("expected_data.txt", "w") 

#Instantiating AXI Master Utilities 
master_util = ipc_axis_master_util("gt_master")

#We are sending few payloads to the device 
for i in range(1,512):
    #Create payload
    payload = xtlm_ipc.axi_stream_packet()
    #Generate Random Data length
    #Here we are generating 1-25 random integers. Each integer can have any value
    #We are multiplying by 4 as, each ineger will take 4 byes of memory 
    payload.data_length = 4*random.randint(1,25)

    rand_int = [] #A list containing random numbers

    for j in range(0, int(payload.data_length/4)):
        #Generate a random integer and append to list 
        rand_int.append(random.randint(0, 10000)) #We can choose any range. 

    #Convert generated random number to byte form (as supported by C style struct), 
    #More features packing are available at https://docs.python.org/3/library/struct.html
    #User is free to choose his own way of packing as long as it is in-line with C types
    format_string = "<"+str(int(payload.data_length/4))+"I" 
    payload.data = bytes(bytearray(struct.pack(format_string, *tuple(rand_int))))
    payload.tlast = True # to mark end of packet. This is user's choice based on application

    #Write Data to File (For Verification)
    for element in rand_int:
        send_file.write(str(element) + "\n")
    send_file.write(str(payload.data_length) + "\n")

    #Generate expected data
    expected_data = [element +1 for element in rand_int]

    #Write expected data to file
    for element in expected_data:
        expected_file.write(str(element) + "\n")
    expected_file.write(str(payload.data_length) + "\n")

    #Send the transaction 
    print ("Sending Txn " + str(i))
    master_util.b_transport(payload)

#Wait sometime for kernel to process
time.sleep(5)
#If user want to end the simulation, user can use following API
master_util.end_of_simulation()

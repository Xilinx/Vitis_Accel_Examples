#!/usr/local/bin/python3
from xilinx_xtlm import ipc_axis_slave_nb_util 
from xilinx_xtlm import xtlm_ipc

import random #for Random pattern
import string
import struct
import time

#File to store received data
recv_file = open("recv_data_non_blocking.txt", "w")

#Function which stores payload data to the file
def my_cb(payload):
    #More features packing/Unpacking are available at https://docs.python.org/3/library/struct.html
    format_string = "<"+str(int(payload.data_length/4))+"I"
    recv_tuple = struct.unpack(format_string, payload.data)
    for element in recv_tuple:
        recv_file.write(str(element) + "\n")
    recv_file.write(str(payload.data_length)+"\n" )

#Instantiate AXI Stream Slave util
slave_util = ipc_axis_slave_nb_util("gt_slave")

count = 1
#Sample the payload for given number of times
while(count < 512) :
    if( slave_util.no_of_available_txns() > 0):
        payload = slave_util.nb_sample_transaction()
        my_cb(payload)
        print ("Recived " + str(count))
        count = count + 1
    else :
        time.sleep(0.1)

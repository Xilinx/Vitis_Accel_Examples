#
# Copyright 2022 Xilinx, Inc.
# Copyright 2022-2023 Advanced Micro Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
from x3ntg import *
from scapy.all import *
import os

pwd = os.getcwd()
filesend = pwd + "/pcapsend.txt"
filerecv = pwd + "/pcaprecv.txt"
print("Running pcap  flow..")
print("Input file", filesend)
print("Output file", filerecv)
# Create RX class object
rxSendObj = rxin()
rxReceiveObj = rxout()
# Create TX class object
txSendObj = txin()
txRecieveObj = txout()

print ("Sending pcap traffic to txin0")
txSendObj.generate_traffic(filesend)


frame_id = 0
frames = []
while frame_id < 10:
    # [X3NTG API]: Recieve Frames List using RXOUT Object
    frames_list = txRecieveObj.receive(0)

    if frames_list is None:
        continue
    else:
        print ("Received frame",frame_id,"on txout0  port")
        # Read frames from list and
        frame_id += 1
        rxSendObj.enqueue(frames_list[0], 0, 1500)
print("Looping back all 10 frame received in rxout0 to txin0")
rxSendObj.send()

# User provides list of receive frame count per PCAP file
pcapReceiveFrameCount = [7]

rxReceiveObj.receive_traffic(filerecv, pcapReceiveFrameCount)
print("Received only 7 frames from rxout0 as 3 frames were dropped by exerciser")
print("PCAP Based NTG API Flow Ran successfully")
print("TEST PASSSED")

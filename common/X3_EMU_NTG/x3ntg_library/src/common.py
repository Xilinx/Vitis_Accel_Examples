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

from xilinx_xtlm import ipc_axis_multi_intf_util
from xilinx_xtlm import xtlm_ipc
from xilinx_xtlm import decrease_interface_throughput
from scapy.all import *
import random
import enum

class x3ntg_errcode(enum.Enum):
    # Indicates good frame
    GOOD_FRAME = 0 
    
    # Indicates bad frame
    BAD_FRAME = 1
    
    # Indicates incorrect even parity
    WRONG_PARITY = 2

class x3ntg_iframe_delay(enum.Enum):
    # PCAP Mode
    PCAP = 0
    
    # Fastest Mode
    FIXED = 1   

class x3ntg_ntwrk_path(enum.Enum):
    # RX Mode
    RX = 0
    
    # TX Mode
    TX = 1

# Ethernet packet information
x3ntg_maxFrameLength = 1518 # Maximum Ethernet Frame Size
x3ntg_datawidth = 4 # Rx data width
x3ntg_clkperiodns = 2 # Clock frequence X3 500MHz
x3ntg_intfcnt = 2 # X3 has 2 interfaces per port
x3ntg_tuser_rxwidth = 16 # MAX TUSER width supported bytes
x3ntg_tuser_txwidth = 8 # MAX TUSER width supported bytes
x3ntg_mintf_cnt = 8 # Max interface count  
x3ntg_tuser_width = 2 # MAX TUSER width supported
 
# Calculate maximum time taken to process a frame  
x3ntg_maxNsPerFrame = (x3ntg_maxFrameLength / x3ntg_datawidth) * x3ntg_clkperiodns

class x3ntg_top:
    def validate(self, expected_pcap, output_pcap):
        if expected_pcap is not None:
            expected_buffer = rdpcap(expected_pcap) if expected_pcap is not None else []
            totalFramesExpected = len(expected_buffer)                  
        
        if output_pcap is not None:
            output_buffer = rdpcap(output_pcap) if output_pcap is not None else []
            totalFramesOutput = len(output_buffer)

        # Check total frame count
        if totalFramesExpected != totalFramesExpected:
            print("Validate: PCAP frame length mismatch")
            return True
        else:
            frameId = 0
            for frameId in range(0, totalFramesOutput):
                outFrame = output_buffer[frameId]
                expFrame = expected_buffer[frameId]
                if outFrame != expFrame:
                    print("Validate: Frames mismatch: ", frameId)
                    return True
                frameId += 1           
    
    def compute_pcap_delay(self, inputPcapBuffer, time_scale_factor):
        FrameIdx = 0
        old_frame = inputPcapBuffer[FrameIdx]
        min_val = 0 
        reltimes = []
        delay_list = []
        
        
        sumreltimes = 0
        items = 0
        # Calculate average relative time
        for frame in inputPcapBuffer:
            relative_time = frame.time - old_frame.time
            reltimes.append(relative_time)
            sumreltimes += relative_time
            items += 1
            old_frame = frame
        avg_rel_time = sumreltimes / items
        
        # Time scale factor     
        tscale_factor = (avg_rel_time * 1000000000) / x3ntg_maxNsPerFrame
        
        scale_factor = 0
        if time_scale_factor == 1:
            scale_factor = tscale_factor
        else:
            scale_factor = time_scale_factor

        if scale_factor == 0:
            scale_factor = 1
        
        # Calculate Delay per frame 
        for items in reltimes:
            delay_calc = (items * 1000000000) / scale_factor
            delay_list.append(delay_calc)

        return delay_list

    def dissect_pcap_port(self, list_pcap_ethernet_values):
        pcapFiles = []
        portNumbers = []
        # Read pcap file names and port values
        with open(list_pcap_ethernet_values) as files:
            for line in files:
                x = line.split(",") 
                pcapFiles.append(x[0])
                portNumbers.append(x[1])

        return [pcapFiles, portNumbers]

    def extract_bitval(self, val, bitIdx):
        # Extract value at a bit index provided
        num = val & (1 << bitIdx)
        if num > 0:
            return 1
        else:
            return 0

    def even_parity(self, val):
        # bitwise XOR operation to find even parity
        even_parity = self.extract_bitval(val, 0) ^ self.extract_bitval(val,1)
        for i in range(2,8):
            even_parity = even_parity ^ self.extract_bitval(val, i)
        return even_parity

    def drc_parity(self, frame, ntwrk_path):

        stride = int((frame.data_length/4) - 1)
        idx = 0
        tidx = 0
       
        index = 0
        if ntwrk_path == x3ntg_ntwrk_path.RX: 
            index = int((stride-1) * x3ntg_tuser_rxwidth)
        else:
            index = int((stride-1) * x3ntg_tuser_txwidth)
            
        lastval = frame.tuser[index]
        lastval >>= 2

        for dix in range(0, stride):
            byte = 0
            b1=b2=b3=b4=0

            byte_1 = frame.data[idx]
            byte_2 = frame.data[idx + 1]            
            byte_3 = frame.data[idx + 2]            
            byte_4 = frame.data[idx + 3]            

            if dix == stride - 1:
                if lastval == 0x01:
                    byte_2 = 0 
                    byte_3 = 0 
                    byte_4 = 0 
                if lastval == 0x03:
                    byte_3 = 0
                    byte_4 = 0
                if lastval == 0x07:
                    byte_4 = 0
               
            # Calculate even parity per byte
            if idx < frame.data_length:
                b1 = self.even_parity(byte_1)
            if (idx + 1) < frame.data_length:
                b2 = self.even_parity(byte_2)
            if (idx + 2) < frame.data_length:
                b3 = self.even_parity(byte_3)
            if (idx + 3) < frame.data_length:
                b4 = self.even_parity(byte_4)
            
            # Construct even parity byte
            if (b1 == 1):
                byte = 1
            if (b2 == 1):
                byte |= 2
            if (b3 == 1):
                byte |= 4
            if (b4 == 1):
                byte |= 8
            idx += 4

            # Even parity difference 
            parity = frame.tuser[tidx + 1]
            parity >>= 2
            frameType = False
            if (byte != parity):
                frameType = True 

            if ntwrk_path == x3ntg_ntwrk_path.RX:
                tidx += x3ntg_tuser_rxwidth
            else:
                tidx += x3ntg_tuser_txwidth

        return frameType
    
    def transmit_thread(self, inputPcap, port, delaymode, tsfactor_delay):
        print("Transmit Thread ... Port: ", port)
        if inputPcap is not None:
            inputPcapBuffer = rdpcap(inputPcap) if inputPcap is not None else []
            totalFramesPcap = len(inputPcapBuffer)
            logging.info("Input PCAP File: %s || Total Frames: %d || port: %d", inputPcap, totalFramesPcap, port)
        p_id = 0
        FrameIdx = 0
        cnt = 0
        
        if delaymode == x3ntg_iframe_delay.PCAP:
            # Calculate inter frame delay based on EPOCH time coming from PCAP file       
            delay_list = self.compute_pcap_delay(inputPcapBuffer, tsfactor_delay)
            
            for frame in range(totalFramesPcap):
                # Pick the frame from input buffer         
                p = inputPcapBuffer[FrameIdx]
                FrameIdx += 1
                self.enqueue(p, port, int(delay_list[frame]))
                cnt = frame
            self.send()

        if delaymode == x3ntg_iframe_delay.FIXED:
            # Run this for all the frames in buffer
            for frame in range(totalFramesPcap):
                # Interframe delay mode
                # Default 5 clock cycles delay - Fast mode
                # User can provide based on requirement as well
                delay = tsfactor_delay * x3ntg_clkperiodns
       
                # Pick the frame from input buffer         
                p = inputPcapBuffer[FrameIdx]
                FrameIdx += 1
                self.enqueue(p, port, int(delay))
                cnt = frame

            self.send()
        logging.info("Transmit Frame:  %d / %d  Port = %d", cnt+1, totalFramesPcap,port)

    def recv_frame_tkeep(self, frame, ntwrk_path):
        data = bytearray(frame.data_length)
        dataFinal = []
        stride = len(data) / 4
        index = 0
        
        if ntwrk_path == x3ntg_ntwrk_path.RX:
            index = int((stride-1) * x3ntg_tuser_rxwidth)
        else:
            index = int((stride-1) * x3ntg_tuser_txwidth)
            
        lastval = frame.tuser[index]
        lastval >>= 2
        
        last_chunk = stride - 1 
        
        idx = 0
        for i in range(0,int(stride - 1)):
            dataFinal.append(frame.data[idx])
            dataFinal.append(frame.data[idx + 1])
            dataFinal.append(frame.data[idx + 2])
            dataFinal.append(frame.data[idx + 3])
            idx += 4
        
        total_valid_bytes = 0
        
        if lastval == 1:
            total_valid_bytes = 1
        if lastval == 3:
            total_valid_bytes = 2
        if lastval == 7:
            total_valid_bytes = 3
        if lastval == 15:
            total_valid_bytes = 4
        
        for i in range(0,total_valid_bytes):
            dataFinal.append(frame.data[idx])
            idx += 1
 
        return dataFinal 

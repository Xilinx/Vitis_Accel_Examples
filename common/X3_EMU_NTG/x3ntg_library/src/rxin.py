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
from common import*

class rxin(x3ntg_top):
    def __init__(self):
        # Emulation Setup: Create sim_ipc connection to rx_in
        self.m_rxInIpc = ipc_axis_multi_intf_util("rx_in")

        # Global data structure holds data for all interfaces driven by sim ipc
        self.transmit_frames = [[],[],[],[],[],[],[],[]]

        # Thread lock 
        self.sendLock = threading.Lock()

    def kill_simulation(self):
        self.m_rxInIpc.end_of_simulation()
    
    def enqueue(self, network_frame, port, delay_val=5, bad_frame = x3ntg_errcode.GOOD_FRAME):
        # Rx Data Frame
        data = bytes(network_frame)

        # Delay in clock period
        delay_in_clkperiod = delay_val * x3ntg_clkperiodns

        # TODO: Unaligned case handling, XTLM IPC needs updates
        dataLen = len(data)
        leftOver = dataLen % 4
        alignLen = dataLen - leftOver
        if leftOver > 0:
            alignLen += 4

        payload = xtlm_ipc.axi_stream_packet()
        payload.data = data
        payload.data_length = alignLen
        payload.tlast = True
        payload.delay = delay_in_clkperiod
        
        x = [0xff] * len(data)
        val = 4 - (len(data) % 4) 
        if val != 4:
            x += [0x00] * val
        barray = bytearray(x)
        
        stride = int(alignLen / 4)
        tuserArray = bytearray(x3ntg_tuser_rxwidth * stride)
        
        idx = 0
        for dix in range(0, stride):
            stride_idx = dix * x3ntg_tuser_rxwidth
            if stride_idx == 0:
                # set data_sop
                tuserArray[stride_idx] |= 1
                # Set meta_sop - 18th bit
                tuserArray[stride_idx + 2] |= 4

            # Add parity calculation logic
            # B1 - 10-15, B2-0,1
            byte = 0
            b1=b2=b3=b4=0
            
            # Calculate even parity per byte 
            if idx < len(data):
                b1 = self.even_parity(data[idx])
            if (idx + 1) < len(data):
                b2 = self.even_parity(data[idx + 1])
            if (idx + 2) < len(data):
                b3 = self.even_parity(data[idx + 2])
            if (idx + 3) < len(data):
                b4 = self.even_parity(data[idx + 3])
            
            # Construct even parity byte 
            if (b1 == 1):
                byte = 1
            if (b2 == 1):
                byte |= 2
            if (b3 == 1):
                byte |= 4
            if (b4 == 1):
                byte |= 8
        
            eparity = byte
            eparity <<= 2
            tuserArray[stride_idx + 1] |= eparity

            if dix == stride-1:
                tuserArray[stride_idx] = 0x00
                #print("Data EOP Handle, dix, stride_idx: ", dix, stride_idx)
                # set data eop
                tuserArray[stride_idx] = 0x02 

                # Data Eop Ptr set (TKEEP alternative)
                # B0 - 2,3,4,5,6,7, B1: 8(0),9(1)
                if barray[idx] == 0xff:
                    tuserArray[stride_idx] |= 0x04
                if barray[idx+1] == 0xff:
                    tuserArray[stride_idx] |= 0x08 
                if barray[idx+2] == 0xff:
                    tuserArray[stride_idx] |= 0x10
                if barray[idx+3] == 0xff:
                    tuserArray[stride_idx] |= 0x20
                
                # set meta eop
                # 19th bit
                tuserArray[stride_idx + 2] = 0x08
                
                # set bad packet
                # 101th bit set to indicate bad packet
                if bad_frame == x3ntg_errcode.BAD_FRAME:
                    tuserArray[stride_idx + 12] = 0x20

            idx += 4
            # Add parity calculation logic
        
        # Update tuser with bad packet and even parity
        payload.tuser = bytes(tuserArray)
        
        portIdx = port * x3ntg_intfcnt 
    
        with self.sendLock:
            # Datawidth = 4, Max Throughput = 16gbps, Expected Throughput=10gbps
            [payload_beats,data_delay] = decrease_interface_throughput(payload, 4, 16, 10)
            self.transmit_frames[portIdx] += payload_beats
            self.m_rxInIpc.add_barrier([portIdx], self.transmit_frames)

    def send(self):
        with self.sendLock:
            self.m_rxInIpc.transport_payloads(self.transmit_frames)
            for interface in self.transmit_frames:
                interface.clear()
    

    # Default mode set to FIXED and delay is 5 clock cycles
    def generate_traffic(self, list_pcap_ethernet_values, iframe_delay_mode=x3ntg_iframe_delay.FIXED, tsfactor_delay=1):
        print("In generate traffic")
        pcapPortLists = self.dissect_pcap_port(list_pcap_ethernet_values)
        threads = [] 
        index = 0 

        if iframe_delay_mode == x3ntg_iframe_delay.FIXED:
            if tsfactor_delay == 1:
                tsfactor_delay = 5
        
        # Run through each pcap file in a seperate thread
        for pcap in pcapPortLists[0]:
            t = threading.Thread(target=self.transmit_thread, args=[pcap, int(pcapPortLists[1][index]), iframe_delay_mode, tsfactor_delay])
            t.start()
            threads.append(t)
            index += 1
        
        # Join threads
        for t in threads:
            t.join()

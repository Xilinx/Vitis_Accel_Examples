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

from common import *

class rxout(x3ntg_top):
    def __init__(self):
        # Emulation Setup: Create sim_ipc connection to rx_out
        self.m_rxOutIpc = ipc_axis_multi_intf_util("rx_out")
        
        # Global data structure holds data for all interfaces driven by sim ipc
        self.m_receive_frames = [[],[],[],[],[],[],[],[]]
        
        # Thread lock 
        self.m_receiveLock = threading.Lock()
    
    def kill_simulation(self):
        self.m_rxOutIpc.end_of_simulation()
        
    def receive_payloads(self):
        with self.m_receiveLock:
            data_strm = self.m_rxOutIpc.sample_payloads()
            for i in range(0,x3ntg_mintf_cnt):
                if (len(data_strm[i]) > 0):
                    for frame in  data_strm[i]:
                        self.m_receive_frames[i].append(frame)
    
    def receive_list(self, port=0):
        self.receive_payloads()
        portIdx = port * x3ntg_intfcnt
        if len(self.m_receive_frames[portIdx]) > 0:
            frameList = self.m_receive_frames[portIdx] 
            listRet = []
            
            # Copy frames from global list
            for frame in frameList:
                listRet.append(frame)
            
            # Clear global list 
            self.m_receive_frames[portIdx].clear()

            return listRet
        else:
            return None 


    def receive(self, port):
        # Sample payloads 
        self.receive_payloads()
        err_codes = []
        portIdx = port * x3ntg_intfcnt
        # If data is available then return otherwise return none 
        if len(self.m_receive_frames[portIdx]) > 0:
            # Return frame by frame
            frame = self.m_receive_frames[portIdx].pop(0) 
            
            # Check for even parity
            error_parity = self.drc_parity(frame, x3ntg_ntwrk_path.RX)
            # Will enable even parity in future release
            #if error_parity == True:
            #   err_codes.append(x3ntg_errcode.WRONG_PARITY)
            
            tuserLen = len(frame.tuser)
            stride = (tuserLen / x3ntg_tuser_rxwidth) - 1
            badPktVal = bool(frame.tuser[int(stride * x3ntg_tuser_rxwidth) + 12] & 0x20)
            if badPktVal == True:
                err_codes.append(x3ntg_errcode.BAD_FRAME)
 
            # Extract exact receive frame based on tkeep
            dataFinal = self.recv_frame_tkeep(frame, x3ntg_ntwrk_path.RX)

            # structure to be returned to user <frame, port, error>
            frame_struct = [dataFinal, port, err_codes]
    
            return frame_struct
        else:
            return None
        
    def receive_thread(self, outputPcap, port, outputFrameSize):
        print("Receive Thread ... Port: ", port)
        p_id = 0 
        while p_id < outputFrameSize:
            frameList = self.receive_list(port)
            if frameList == None:
                time.sleep(1)
                continue
            
            for frame in frameList:
                logging.info("Receive Frame %d/%d (%.2f %%). || Port: %d", p_id+1, outputFrameSize, (float(p_id+1) / outputFrameSize) * 100, port)
                
                # Extract exact receive frame based on tkeep
                dataFinal = self.recv_frame_tkeep(frame, x3ntg_ntwrk_path.RX)
 
                # Create ethernet frame using scapy 
                ether_frame = Ether(bytes(dataFinal))

                # Write out this frame to output pcap file
                if outputPcap is not None:
                    wrpcap(outputPcap, ether_frame, append=True)
                
                p_id += 1 
                if (p_id == outputFrameSize):
                    break
    
    def receive_traffic(self, list_pcap_ethernet_values, list_receive_sizes):
        pcapPortLists = self.dissect_pcap_port(list_pcap_ethernet_values)
      
        threads = [] 
        index = 0 
        
        time.sleep(5) 
        # Run through each pcap file in a seperate thread
        for pcap in pcapPortLists[0]:
            outFilename = str(pcap) + str(index)
            try:
                if outFilename is not None:
                    os.remove(outFilename)
            except OSError:
                pass
            
            t = threading.Thread(target=self.receive_thread, args=[outFilename, int(pcapPortLists[1][index]), list_receive_sizes[index]])
            t.start()
            threads.append(t)
            index += 1
        
        # Join threads
        for t in threads:
            t.join()

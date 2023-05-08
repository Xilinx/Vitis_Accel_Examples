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
from txin import *
class txout(x3ntg_top):
    def __init__(self):
        # Emulation Setup: Create sim_ipc connection to tx_out
        self.m_txOutIpc = ipc_axis_multi_intf_util("tx_out")
        
        # Global data structure holds data for all interfaces driven by sim ipc
        self.m_receive_frames = [[],[],[],[],[],[],[],[]]
        
        # Data structure to hold responses 
        self.m_transmit_frames = [[],[],[],[]]

        # Thread lock
        self.m_receiveLock = threading.Lock()
    
    def kill_simulation(self):
        self.m_txOutIpc.end_of_simulation()
    
    def receive_payloads(self):
        with self.m_receiveLock:
            # Sample payloads from emulation, this call returns 
            # all the available data on all interfaces in a 2D list form
            data_strm = self.m_txOutIpc.sample_payloads()
            for i in range(0,x3ntg_mintf_cnt):
                if (len(data_strm[i]) > 0):
                    for frame in  data_strm[i]:
                        # Update global data structure with valid data
                        self.m_receive_frames[i].append(frame)

    def receive_list(self, port=0):
        # Sample payloads
        self.receive_payloads()

        # PortIdx calculation
        portIdx = port * x3ntg_intfcnt 
        
        # Based on input port return list containing data/meta received output
        if len(self.m_receive_frames[portIdx]) > 0:
            frameList = self.m_receive_frames[portIdx]
            listRet = [] 

            # Copy frames from global list
            for frame in frameList:
                listRet.append(frame)
           
            # Clear global lists
            self.m_receive_frames[portIdx].clear()
            return listRet
        else:
            return None

    def receive(self, port=0):
        # Sample payloads 
        self.receive_payloads()
        
        # PortIdx calculation
        portIdx = port * x3ntg_intfcnt

        # Drive through meta request outputs            
        for metaReq in self.m_receive_frames[portIdx]:
            self.tx_response(metaReq, port)    
            
        err_codes = [] 
        # If data is available then return otherwise return none 
        if len(self.m_receive_frames[portIdx]) > 0:
            # Return frame by frame
            frame = self.m_receive_frames[portIdx].pop(0)
    
            # Check for even parity and bad frame
            error_parity = self.drc_parity(frame, x3ntg_ntwrk_path.TX)            
            # Will enable even parity in future release
            #if error_parity  == True:
            #    err_codes.append(x3ntg_errcode.WRONG_PARITY)

            tuserLen = len(frame.tuser)
            stride = (tuserLen / x3ntg_tuser_txwidth) - 1
            badPktVal = bool(frame.tuser[int(stride * x3ntg_tuser_txwidth) + 6] & 0x20)
            if badPktVal == True:
                err_codes.append(x3ntg_errcode.BAD_FRAME)
        
            # Extract exact receive frame based on tkeep
            dataFinal = self.recv_frame_tkeep(frame, x3ntg_ntwrk_path.TX)
            
            # structure to be returned to user <frame, port, error>
            frame_struct = [dataFinal, port, err_codes]

            return frame_struct
        else:
            return None
 
    def tx_response(self, metarout, port = 0):
        
        # TX Meta Response 
        payload_response = xtlm_ipc.axi_stream_packet()
        txmetaresp = bytearray(16)
        
        # 0-80 TSTAMP, 10bytes
        txmetaresp[0] = 0
        txmetaresp[1] = 0
        txmetaresp[2] = 0
        txmetaresp[3] = 0
        txmetaresp[4] = 0
        txmetaresp[5] = 0
        txmetaresp[6] = 0
        txmetaresp[7] = 0
        txmetaresp[8] = 0
        txmetaresp[9] = 0
        
        # Update sequence - 1byte
        metaReqData = bytearray(metarout.tuser)
        seqId = 0
        # Get sequence Id from meta request output
        txmetaresp[10] = (metaReqData[2] >> 4) | ((metaReqData[3] & 0x0f) << 4)

        # Get qid from meta request output 
        txmetaresp[11] |= (metaReqData[3] & 0xf0) >> 4
        
        # Update 92bit to indicate mac tx request successfully transmitted
        txgood = 1 << 4
        txmetaresp[11] |= txgood 
 
        # Extract 93rd bit from set in metarequest
        ulpmetreq = self.extract_bitval(metaReqData[6],6) 
        ulpmetresp = 0
        if ulpmetreq == 1:
            ulpmetresp = 1<<5
        txmetaresp[11] |= ulpmetresp

        # 94,95,96 - Left untouched as ULP is reponsible
        # TX_RCVD_BAD, TX_UNDERRUN, ULP_TERMINATED
        # 97-127 bits a reserved
        payload_response.data = bytes(txmetaresp)
        payload_response.data_length = 16
        payload_response.tlast = True
        payload_response.delay = 0

        with self.m_receiveLock:
            self.m_transmit_frames[port].append(payload_response)
            self.m_txOutIpc.transport_payloads(self.m_transmit_frames) 
            self.m_transmit_frames[port].clear()

    def receive_thread(self, outputPcap, port, outputFrameSize):
        print("Receive Thread ... Port: ", port)
        frame_id = 0 
        meta_id = 0
        dataIdx = 0
        while (frame_id < outputFrameSize):
            frameList = self.receive_list(port)
            if frameList == None:
                time.sleep(1)
                continue
        
            # Drive through frame data
            for frame in frameList:
                logging.info("Receive Frame %d/%d (%.2f %%). || Port: %d", frame_id+1, outputFrameSize, (float(frame_id+1) / outputFrameSize) * 100, port)
                
                # Extract exact receive frame based on tkeep
                dataFinal = self.recv_frame_tkeep(frame, x3ntg_ntwrk_path.TX)
            
                # Create ethernet frame using scapy 
                ether_frame = Ether(bytes(dataFinal))
   
                # Write frame information
                if outputPcap is not None:
                    wrpcap(outputPcap, ether_frame, append=True)
                
                # Send response                
                self.tx_response(frame, port)    

                frame_id += 1 
                if (frame_id == outputFrameSize):
                    break
                
        
    def receive_traffic(self, list_pcap_ethernet_values, list_receive_sizes):
        pcapPortLists = self.dissect_pcap_port(list_pcap_ethernet_values)
        
        threads = [] 
        index = 0 
        
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

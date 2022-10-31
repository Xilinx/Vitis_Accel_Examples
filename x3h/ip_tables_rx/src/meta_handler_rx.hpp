/*
 * Copyright (C) 2022 Xilinx, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#include "x3_specs.hpp"
#include "net_utils.hpp"

void mac_handler_rx(hls::stream<AxiX3RxData<0> >& mac2ulpData,
                    RxPktStream& ulpInData) {
    AxiRxData macPkt;
    X3RxData ethPkt;
    
    macPkt = mac2ulpData.read();
    
    ethPkt.data = macPkt.data;
    ethPkt.user = macPkt.user;
    ethPkt.keep = macPkt.keep;
    ethPkt.last = macPkt.last;
    
    ulpInData.write(ethPkt);
}

void nic_handler_rx(RxPktStream& ulpOutData,
                    hls::stream<AxiX3RxData<0> >& ulp2nicData) {
    X3RxData ethPkt;
    AxiRxData nicPkt;
    
    ethPkt = ulpOutData.read();
    
    nicPkt.data = ethPkt.data;
    nicPkt.user = ethPkt.user;
    nicPkt.keep = ethPkt.keep;
    nicPkt.last = ethPkt.last;
    
    ulp2nicData.write(nicPkt);
}

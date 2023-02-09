/*
 * Copyright (C) 2022 Xilinx, Inc
 * Copyright (C) 2022-2023 Advanced Micro Devices, Inc.
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

#ifndef IPT_FILTER_RX_SC_HPP
#define IPT_FILTER_RX_SC_HPP

#include "vpp_acc.hpp"
#include "vpp_stream.hpp"

#include <iostream>
#include <string>
#include <stdint.h>
#include <ap_int.h>
#include "ap_axi_sdata.h"

#include "x3_specs.hpp"
#include "net_utils.hpp"
#include "packet_filter_sc.hpp"

#ifndef RULEWORD_BITS
#define RULEWORD_BITS 192
#endif

#ifndef IPRULE_TABLE_SIZE
#define IPRULE_TABLE_SIZE 64
#endif

class IPTablesRxSC : public VPP_ACC<IPTablesRxSC, 1>
{
public:

    ZERO_COPY(ruleIn);
    ZERO_COPY(ruleOut);
    
    SYS_PORT(ruleIn, HOST[0]);
    SYS_PORT(ruleOut, HOST[0]);
    
    SYS_CLOCK_ID(rxMacInterface, 3);
    SYS_CLOCK_ID(rxNicInterface, 3);
    
    // Config free running kernel
    FREE_RUNNING(rxMacInterface);
    FREE_RUNNING(ethernetParser);
    FREE_RUNNING(ipv4Parser);
    FREE_RUNNING(ruleManager);
    FREE_RUNNING(packetFilter);
    FREE_RUNNING(rxNicInterface);
    
    // X3 Hybrid platform connections
    SYS_PORT(macStream, MMacRx0);
    SYS_PORT(nicStream, SNicRx0);
    
    static void compute(ap_uint<192>* ruleIn, ap_uint<192>* ruleOut, uint32_t cmd);
    
    static void controlUnit(ap_uint<192> *ruleIn,
                            ap_uint<192> *ruleOut,
                            uint32_t cmd,
                            ApUintStream<8> &commandStream,
                            AxiWordStream<RULEWORD_BITS> &ruleUpdateStream,
                            AxiWordStream<RULEWORD_BITS> &ruleReadbackStream);
    
    static void rxMacInterface(RxDataPort &macStream,
                               RxPktStream &ulpInStream);
    
    static void ethernetParser(RxPktStream &ulpInStream,
                               ApDataStream &ipInDataStream,
                               ApUintStream<1> &ethValidStream,
                               RxPktStream &inPacketStream);
    
    static void ipv4Parser(ApDataStream &ipInDataStream,
                           ApUintStream<1> &ethValidStream,
                           ApDsStream<IPv4InfoPack_dt> &ipInfoStream);
    
    static void ruleManager(ApUintStream<8> &commandStream,
                            ApDsStream<IPv4InfoPack_dt> &ipInfoStream,
                            AxiWordStream<RULEWORD_BITS> &ruleUpdateStream,
                            AxiWordStream<RULEWORD_BITS> &ruleReadbackStream,
                            ApUintStream<4> &actionStream);
    
    static void packetFilter(RxPktStream &inPacketStream,
                             ApUintStream<4> &actionStream,
                             RxPktStream &ulpOutStream);
    
    static void rxNicInterface(RxPktStream &ulpOutStream,
                               RxDataPort &nicStream);
};
#endif // IPT_FILTER_RX_SC_HPP

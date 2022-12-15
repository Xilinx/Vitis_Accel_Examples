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
#ifndef IPT_FILTER_RX_SC_CPP
#define IPT_FILTER_RX_SC_CPP

#include "iptfilterRxsc.hpp"
#include "meta_handler_rx.cpp"
#include "iptfiltersc.cpp"
using namespace rxHandler;
using namespace IPTablesSC;

void IPTablesRxSC::compute(ap_uint<192>* ruleIn, ap_uint<192>* ruleOut, uint32_t cmd) {
    // Kernel external AXI Port streams
    RxDataPort macStream;
    RxDataPort nicStream;
    // Inter-kernel streams
    static RxPktStream ulpInStream("ulpInStream");
    static ApDataStream ipInDataStream("ipInDataStream");
    static RxPktStream inPacketStream("inPacketStream");
    static RxPktStream ulpOutStream("ulpOutStream");
    // host interaction and kernel configurations
    controlUnit(ruleIn, ruleOut, cmd, commandStream, ruleUpdateStream, ruleReadbackStream);
    // mac side boundary kernel, provides additional packet buffering
    rxMacInterface(macStream, ulpInStream);
    // ethernet frame parsing, extract only IPV4 packet, for rest set invalid
    ethernetParser(ulpInStream, ipInDataStream, ethValidStream, inPacketStream);
    // ip4 packet parsing to extract ip-port-protocol-valid information from packets
    ipv4Parser(ipInDataStream, ethValidStream, ipInfoStream);
    // set/clear configs, update/read-back rules, send statistics
    ruleManager(commandStream, ipInfoStream, ruleUpdateStream, ruleReadbackStream, actionStream);
    // perform filtering
    packetFilter(inPacketStream, actionStream, ulpOutStream);
    // nic side boundary kernel, provides synchronization for meta
    rxNicInterface(ulpOutStream, nicStream);
}

void IPTablesRxSC::rxMacInterface(RxDataPort &macStream,
                                  RxPktStream &ulpInStream) {
    rxHandler::mac_meta_handler_rx(macStream, ulpInStream);
}

void IPTablesRxSC::rxNicInterface(RxPktStream &ulpOutStream,
                                  RxDataPort &nicStream) {
    rxHandler::nic_meta_handler_rx(ulpOutStream, nicStream);
}

void IPTablesRxSC::packetFilter(RxPktStream &inPacketStream,
                                ApUintStream<4> &actionStream,
                                RxPktStream &ulpOutStream) {
    // perform filtering
    bool last = false;
    X3RxData pckVal;
    auto tgt = actionStream.read();
    ipTgtAction pktAction = static_cast<ipTgtAction>(int(tgt) & 0xF);
    
PERFORM_FILTERING:
    while (!last) {
#pragma HLS PIPELINE II = 1
        if (!last) pckVal = inPacketStream.read();
        last = pckVal.last;
#ifdef HW_EMU_TEST
        if (pktAction != IPT_ACCEPT) pckVal.data = -10;
        if (last) {
            ulpOutStream << pckVal;
        } else {
            ulpOutStream << pckVal;
        }
#else
        if (last) {
            if (pktAction == IPT_ACCEPT) ulpOutStream << pckVal;
        } else {
            if (pktAction == IPT_ACCEPT) ulpOutStream << pckVal;
        }
#endif
    }
}

void IPTablesRxSC::controlUnit(ap_uint<192> *ruleIn,
                               ap_uint<192> *ruleOut,
                               uint32_t cmd,
                               ApUintStream<8> &commandStream,
                               AxiWordStream<RULEWORD_BITS> &ruleUpdateStream,
                               AxiWordStream<RULEWORD_BITS> &ruleReadbackStream) {
	IPTablesSC::controlUnit(ruleIn, ruleOut, cmd, commandStream, ruleUpdateStream, ruleReadbackStream);
}

void IPTablesRxSC::ethernetParser(RxPktStream &ulpInStream,
                                  ApDataStream &ipInDataStream,
                                  ApUintStream<1> &ethValidStream,
                                  RxPktStream &inPacketStream) {
    IPTablesSC::ethernetParser<RxPktStream>(ulpInStream, ipInDataStream, ethValidStream, inPacketStream);
}

void IPTablesRxSC::ipv4Parser(ApDataStream &ipInDataStream,
                              ApUintStream<1> &ethValidStream,
                              ApDsStream<IPv4InfoPack_dt> &ipInfoStream) {
	IPTablesSC::ipv4Parser(ipInDataStream, ethValidStream, ipInfoStream);
}

void IPTablesRxSC::ruleManager(ApUintStream<8> &commandStream,
                               ApDsStream<IPv4InfoPack_dt> &ipInfoStream,
                               AxiWordStream<RULEWORD_BITS> &ruleUpdateStream,
                               AxiWordStream<RULEWORD_BITS> &ruleReadbackStream,
                               ApUintStream<4> &actionStream) {
    IPTablesSC::ruleManager(commandStream, ipInfoStream, ruleUpdateStream, ruleReadbackStream, actionStream);
}

#endif // IPT_FILTER_RX_SC_CPP

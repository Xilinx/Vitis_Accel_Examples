/*
 * (c) Copyright 2022 Xilinx, Inc. All rights reserved.
 * (c) Copyright 2022-2023 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __XFNETWORKING_PKT_FWD_UTILS__
#define __XFNETWORKING_PKT_FWD_UTILS__
#include <stdint.h>

#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

#include "general_def.hpp"

#if !defined(__SYNTHESIS__) && __LIBRARY_PKT_FWD_DEBUG__ > 1
#include <iostream>
#endif

#if defined(__LIBRARY_PKT_FWD_DEBUG__) && !defined(TEST_PKT_NUM)
#error "TEST_PKT_NUM must be defined to the number of packets pushed into inStrm by testbench in HLS test"
#endif

enum AnalyzeState { LoadHeader = 0, AnalyzeHeader, ReadRest };

/**
 * @brief Get an unsigned value from a long word with multi-bytes in network-order.
 *
 * @tparam WV value width in bits, must be multiple of 8 if greater than 8.
 * @tparam O offset.
 * @tparam WI input word width in bits.
 *
 * @param word input word.
 *
 * @return the unsigned value.
 */
template <int O, int WV, int WI>
ap_uint<WV> getUnsigned(ap_uint<WI>& word) {
#if __cplusplus >= 201103L
    static_assert(WV < 8 || WV % 8 == 0, "Unsigned width is not multiple of 8");
// static_assert(WV + O <= 352, "Unsigned data not fully in range");
#endif
    ap_uint<WV> r;
    if (WV < 8) {
        r = word.range(O + WV - 1, O);
    } else {
        for (int i = 0; i < WV / 8; ++i) {
#pragma HLS unroll
            r.range(8 * i + 7, 8 * i) = word.range(O + WV - 1 - 8 * i, O + WV - 8 - 8 * i);
        }
    }
    return r;
}

/**
 *
 * @brief Duplicates input AXI stream.
 *
 * @param inStrm input AXI stream.
 * @param dup1Strm output 1st duplicated stream.
 * @param dup2Strm output 2nd duplicated stream.
 * @param dup3Strm output 3rd duplicated stream.
 * @param keepStrm output keep stream for writePacket.
 *
 */
template <int DWIDTH, int UWIDTH>
static void dupStrm(hls::stream<ap_axiu<DWIDTH, UWIDTH, 0, 0> >& inStrm,
                    hls::stream<ap_uint<DWIDTH + 1> >& dup1Strm,
                    hls::stream<ap_uint<DWIDTH + 1> >& dup2Strm,
                    hls::stream<ap_uint<UWIDTH> >& userStrm) {
#ifdef __LIBRARY_PKT_FWD_DEBUG__
    int num_pkt = 0;
LOOP_DUP_STRM:
    while (num_pkt < TEST_PKT_NUM)
#else
LOOP_DUP_STRM:
    while (1)
#endif
    {
#pragma HLS pipeline II = 1
        ap_axiu<DWIDTH, UWIDTH, 0, 0> in;
        if (inStrm.read_nb(in)) {
            ap_uint<DWIDTH + 1> tmp;
            tmp.range(DWIDTH, 1) = in.data;
            bool last = in.last;
            tmp[0] = last;
            auto tuser = in.user;
            dup1Strm.write(tmp);
            dup2Strm.write(tmp);
            userStrm.write(tuser);
#ifdef __LIBRARY_PKT_FWD_DEBUG__
            num_pkt += in.last;
#endif
        }
    }
}

/**
 *
 * @brief Analyzes frame header and gives control information correspondingly.
 *
 * @param table input table-keys.
 * @param udpDestPort input UDP destination port.
 * @param inStrm input frame stream.
 * @param ctrlStrm output control information stream,  bit[19:4] table-key, bit[3] overflow, bit[2] insert, bit[1]
 * modify, bit[0] send.
 *
 */
template <int DWIDTH>
static void analyzeHeader(hls::stream<ap_uint<DWIDTH + 1> >& inStrm,
                          hls::stream<ap_uint<20> >& ctrlStrm,
                          hls::stream<ap_axiu<16, 0, 0, 0> >& table_in,
                          hls::stream<ap_axiu<16, 0, 0, 0> >& udp_dest) {
    // resetReg(table, udpDestPort);

    AnalyzeState currState = LoadHeader;
    AnalyzeState nextState;
    // registers for buffering header
    ap_uint<LEN_TOTAL_HEADER> currReg; // 14-Ethernet Header 20-IPV Header 4-UDP Header 2-CMD/TABLE
    bool send;
    bool modify;
    bool insert;
    bool transferOverflow;
    ap_uint<4> residualByte;
    ap_uint<20> ctrl;
    uint8_t actID, tableKey;
    ap_uint<4> cntr = 0;

    uint16_t table[256];
    uint16_t udpDestPort[4];
initTable:
    for (int i = 0; i < 256; i++) {
        table[i] = i;
    }
    udpDestPort[0] = 0x1234;

#ifdef __LIBRARY_PKT_FWD_DEBUG__
    int num_pkt = 0;
LOOP_ANALYZE_HEADER:
    while (num_pkt < TEST_PKT_NUM)
#else
LOOP_ANALYZE_HEADER:
    while (1)
#endif
    {

        ap_uint<DWIDTH + 1> in;
        if (!udp_dest.empty()) {
            udpDestPort[0] = udp_dest.read().data;
        } else if (!table_in.empty()) {
            bool last = false;
            uint32_t idx = 0;
        UpdateTableLoop:
            while (!last) {
#pragma HLS pipeline II = 1
                auto val = table_in.read();
                table[idx % 256] = val.data;
                last = val.last;
                idx++;
            }
        } else if (!inStrm.empty()) {
            bool last = false;
        ReadPacketLoop:
            while (!last) {
#pragma HLS pipeline II = 1
                in = inStrm.read();
                last = in[0];
                switch (currState) {
                    case LoadHeader:
                        currReg >>= DWIDTH;
                        currReg.range(351, 352 - DWIDTH) = in.range(DWIDTH, 1);

                        cntr++;
                        // reset internal state flags
                        send = false;
                        modify = false;
                        insert = false;
                        transferOverflow = false;
                        if (cntr == 11)
                            nextState = AnalyzeHeader;
                        else
                            nextState = LoadHeader;
                        break;
                    case AnalyzeHeader:
                        // restart loading header if last
                        if (last) {
                            nextState = LoadHeader;
                            cntr = 0;
#ifdef __LIBRARY_PKT_FWD_DEBUG__
                            num_pkt++;
#endif
                        } else {
                            nextState = ReadRest;
                        }
// valid packet?
#if !defined(__SYNTHESIS__) && __LIBRARY_PKT_FWD_DEBUG__ > 1
                        std::cout << "\nEther type = " << std::hex
                                  << getUnsigned<OFFSET_ETHER_TYPE, LEN_ETHER_TYPE, LEN_TOTAL_HEADER>(currReg)
                                  << std::dec << std::endl;
                        std::cout << "IP header length = "
                                  << getUnsigned<OFFSET_IP_VERSION, LEN_IP_VERSION, LEN_TOTAL_HEADER>(currReg)
                                  << std::endl;
                        std::cout << "IP version = "
                                  << getUnsigned<OFFSET_IP_HEADER_LEN, LEN_IP_HEADER_LEN, LEN_TOTAL_HEADER>(currReg)
                                  << std::endl;
                        std::cout << "IP protocol = "
                                  << getUnsigned<OFFSET_IP_PROTOCOL, LEN_IP_PROTOCOL, LEN_TOTAL_HEADER>(currReg)
                                  << std::endl;
                        std::cout << "UDP destination port = " << std::hex
                                  << getUnsigned<OFFSET_UDP_DEST_PORT, LEN_UDP_DEST_PORT, LEN_TOTAL_HEADER>(currReg)
                                  << std::dec << std::endl;
                        std::cout << "IP total len = " << std::dec
                                  << getUnsigned<OFFSET_IP_TOTAL_LEN, LEN_IP_TOTAL_LEN, LEN_TOTAL_HEADER>(currReg)
                                  << std::endl;
#endif
                        actID = getUnsigned<OFFSET_CMD_ACT_ID, LEN_CMD_ACT_ID, LEN_TOTAL_HEADER>(currReg);
                        tableKey = getUnsigned<OFFSET_CMD_TABLE_KEY, LEN_CMD_TABLE_KEY, LEN_TOTAL_HEADER>(currReg);
                        if (getUnsigned<OFFSET_ETHER_TYPE, LEN_ETHER_TYPE, LEN_TOTAL_HEADER>(currReg) ==
                                MATCH_ETHER_TYPE &&
                            getUnsigned<OFFSET_IP_VERSION, LEN_IP_VERSION, LEN_TOTAL_HEADER>(currReg) ==
                                MATCH_IP_HEADER_LEN &&
                            getUnsigned<OFFSET_IP_HEADER_LEN, LEN_IP_HEADER_LEN, LEN_TOTAL_HEADER>(currReg) ==
                                MATCH_IP_VERSION &&
                            getUnsigned<OFFSET_IP_PROTOCOL, LEN_IP_PROTOCOL, LEN_TOTAL_HEADER>(currReg) ==
                                MATCH_IP_PROTOCOL &&
                            getUnsigned<OFFSET_UDP_DEST_PORT, LEN_UDP_DEST_PORT, LEN_TOTAL_HEADER>(currReg) ==
                                udpDestPort[0]) {
#if !defined(__SYNTHESIS__) && __LIBRARY_PKT_FWD_DEBUG__ > 1
                            std::cout << ">>> Action ID = " << (unsigned int)actID << std::endl;
                            std::cout << ">>> Table Key = " << (unsigned int)tableKey << std::endl;
#endif
                            residualByte =
                                (getUnsigned<OFFSET_IP_TOTAL_LEN, LEN_IP_TOTAL_LEN, LEN_TOTAL_HEADER>(currReg) + 18) %
                                (KRNL_DATA_WIDTH / 8);
                            switch (actID) {
                                case ACT_ID_NOP:
                                    send = true;
                                    break;
                                case ACT_ID_DROP:
                                    send = false;
                                    break;
                                case ACT_ID_MODIFY_FROM_TABLE:
                                    send = true;
                                    modify = true;
                                    break;
                                case ACT_ID_INSERT_FROM_TABLE:
                                    send = true;
                                    insert = true;
                                    // IP Total Length + Ethernet Header + checksum
                                    // is frame overflows to a new packet?
                                    if (residualByte == 0 || residualByte == (DWIDTH / 8 - 1)) {
                                        transferOverflow = true;
                                    }
                                    break;
                                // default OP is NOP (transmit without modification)
                                default:
                                    send = true;
                            }
                        } else {
                            send = true;
                        }
#if !defined(__SYNTHESIS__) && __LIBRARY_PKT_FWD_DEBUG__ > 1
                        std::cout << "send: " << send << "\nmodify: " << modify << "\ninsert: " << insert
                                  << "\noverflow: " << transferOverflow << std::endl;
#endif
                        ctrl[0] = send;
                        ctrl[1] = modify;
                        ctrl[2] = insert;
                        ctrl[3] = transferOverflow;
                        ctrl.range(19, 4) = table[tableKey];
                        ctrlStrm.write(ctrl);
                        break;
                    case ReadRest:
                        if (last) {
                            nextState = LoadHeader;
                            cntr = 0;
#ifdef __LIBRARY_PKT_FWD_DEBUG__
                            num_pkt++;
#endif
                        }
                        break;
                    // in exception, reset the state machine
                    default:
                        nextState = LoadHeader;
                }
                // update state
                currState = nextState;
            }
        }
    }
}

ap_uint<8> byte_reverse(ap_uint<8> v) {
    ap_uint<8> val;
    val.range(3, 0) = v.range(7, 4);
    val.range(7, 4) = v.range(3, 0);
    return val;
}

template <int T>
ap_uint<T> net_order(ap_uint<T> v) {
    ap_uint<T> val;
net_order:
    for (int i = 0; i < T / 8; i++) {
#pragma HLS UNROLL
        val.range(((i + 1) * 8) - 1, i * 8) = v.range(T - 1 - (i * 8), T - ((i + 1) * 8));
    }
    return val;
}

void assemble_packet(uint64_t dest_mac,
                     uint64_t source_mac,
                     uint32_t source_ip,
                     uint32_t dest_ip,
                     uint32_t source_port,
                     uint32_t dest_port,
                     ap_uint<512>& header) {
    header.range(LEN_MAC_ADDRESS - 1, 0) = net_order<48>(dest_mac);
    header.range(OFFSET_ETHER_SOURCE_MAC + LEN_MAC_ADDRESS - 1, OFFSET_ETHER_SOURCE_MAC) = net_order<48>(source_mac);
    header.range(OFFSET_ETHER_TYPE + LEN_ETHER_TYPE - 1, OFFSET_ETHER_TYPE) = net_order<16>(ap_uint<16>(0x0800));
    header.range(OFFSET_IP_VERSION + LEN_IP_VERSION - 1, OFFSET_IP_VERSION) = 5;
    header.range(OFFSET_IP_HEADER_LEN + LEN_IP_HEADER_LEN - 1, OFFSET_IP_HEADER_LEN) = 4;
    // header.range(OFFSET_IP_VERSION + LEN_IP_VERSION - 1, OFFSET_IP_VERSION) = 0;
    header.range(OFFSET_IP_TOTAL_LEN + LEN_IP_TOTAL_LEN - 1, OFFSET_IP_TOTAL_LEN) = net_order<16>(ap_uint<16>(174));
    header.range(OFFSET_IP_IDENTIFICATION + LEN_IP_IDENTIFICATION - 1, OFFSET_IP_IDENTIFICATION) = 0;
    header.range(OFFSET_IP_FRAGMENT + LEN_IP_FRAGMENT - 1, OFFSET_IP_FRAGMENT) =
        net_order<16>(ap_uint<16>(0x6000)); // not fragmented
    header.range(OFFSET_IP_TTL + LEN_IP_TTL - 1, OFFSET_IP_TTL) = 10;
    header.range(OFFSET_IP_PROTOCOL + LEN_IP_PROTOCOL - 1, OFFSET_IP_PROTOCOL) = 0x11; // UDP
    header.range(OFFSET_IP_CHECKSUM + LEN_IP_CHECKSUM - 1, OFFSET_IP_CHECKSUM) = 0x0;  // need to be calculated
    header.range(OFFSET_IP_SOURCE_IP + LEN_IP_SOURCE_IP - 1, OFFSET_IP_SOURCE_IP) = net_order<32>(source_ip);
    header.range(OFFSET_IP_DEST_IP + LEN_IP_DEST_IP - 1, OFFSET_IP_DEST_IP) = net_order<32>(dest_ip);
    header.range(OFFSET_UDP + LEN_UDP_DEST_PORT - 1, OFFSET_UDP) = net_order<16>(ap_uint<16>(source_port));
    header.range(OFFSET_UDP_DEST_PORT + LEN_UDP_DEST_PORT - 1, OFFSET_UDP_DEST_PORT) =
        net_order<16>(ap_uint<16>(dest_port));
    header.range(OFFSET_UDP_LEN + LEN_UDP_LEN - 1, OFFSET_UDP_LEN) = 154;
    header.range(OFFSET_UDP_CHECKSUM + 15, OFFSET_UDP_CHECKSUM) = 0; // not mandatory
}
#endif //__XFNETWORKING_PKT_FWD_UTILS__

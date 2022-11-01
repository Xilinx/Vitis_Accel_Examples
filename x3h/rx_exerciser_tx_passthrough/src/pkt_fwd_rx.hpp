/*
 * (c) Copyright 2022 Xilinx, Inc. All rights reserved.
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
#ifndef _XFNETWORKING_PKT_FWD_HPP_
#define _XFNETWORKING_PKT_FWD_HPP_

/**
 * @file pkt_fwd.hpp
 * @brief Header for modules used in Packet Passthrough kernel.
 *
 * This file is part of Vitis Networking Library.
 */
#include <stdint.h>
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

#include "pkt_fwd_utils.hpp"

#if !defined(__SYNTHESIS__) && __LIBRARY_PKT_FWD_DEBUG__ > 1
#include <iostream>
#endif

#if defined(__LIBRARY_PKT_FWD_DEBUG__) && !defined(TEST_PKT_NUM)
#error "TEST_PKT_NUM must be defined to the number of packets pushed into inStrm by testbench in HLS test"
#endif

enum SendState { SendHeader = 0, ModifyPacket, SendPacket, SendOverflow };

/**
 *
 * @brief Latching the last AXI element.
 *
 * @param inStrm input AXI stream.
 * @param latch register array.
 *
 */
void storeLast(hls::stream<ap_uint<KRNL_DATA_WIDTH + 1> >& inStrm, uint32_t latch[WORD_COUNT_32]) {
#ifdef __LIBRARY_PKT_FWD_DEBUG__
    int num_pkt = 0;
    while (num_pkt < TEST_PKT_NUM)
#else
    while (1)
#endif
    {
#pragma HLS pipeline II = 1
        ap_uint<KRNL_DATA_WIDTH + 1> in;
        if (inStrm.read_nb(in)) {
            for (int i = 0; i < WORD_COUNT_32; i++) {
#pragma HLS unroll
                latch[i] = in.range(32 * i + 32, 32 * i + 1);
            }
            bool last = in[0];
#ifdef __LIBRARY_PKT_FWD_DEBUG__
            if (last) num_pkt += last;
#endif
        }
    }
}

/**
 * @brief Reset config registers.
 */
void resetReg(volatile uint16_t table[256], volatile uint16_t udpDestPort[4]) {
RESET_REG:
    for (int i = 0; i < 256; ++i) {
#pragma HLS pipeline II = 1
        table[i] = 0;
    }
    udpDestPort[0] = DEFAULT_DEST_IP_PORT;
}

/**
 * @brief Set an unsigned value in a long word with multi-bytes in network-order.
 *
 * @tparam O offset.
 * @tparam WV valud width in bits, must be multiple of 8.
 * @tparam WI input word width in bits.
 *
 * @param word word to be modified.
 * @param value value to be set.
 */
template <int O, int WI, int WV>
void setUnsigned(ap_uint<WI>& word, const ap_uint<WV>& value) {
#if __cplusplus >= 201103L
    static_assert(WV % 8 == 0, "Unsigned width is not multiple of 8");
    static_assert(WV + O <= WI, "Unsigned data not fully in range");
#endif
    for (int i = 0; i < WV / 8; ++i) {
#pragma HLS unroll
        word.range(O + WV - 1 - 8 * i, O + WV - 8 - 8 * i) = value.range(8 * i + 7, 8 * i);
    }
}

/**
 *
 * @brief Emits packet according to the control information given in ctrlStrm.
 *
 * @param inStrm input stream.
 * @param keepStrm keep stream from input AXI stream.
 * @param ctrlStrm input control information stream,
 *        bit[19:4] table-key, bit[3] overflow, bit[2] insert, bit[1] modify, bit[0] send.
 * @param outStrm output AXI stream.
 */
static void writePacket(hls::stream<ap_uint<X3_RX_DWIDTH + 1> >& inStrm,
                        hls::stream<ap_uint<X3_RX_UWIDTH> >& inUser,
                        hls::stream<ap_uint<20> >& ctrlStrm,
                        hls::stream<ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, 0, 0> >& outStrm) {
    bool exit;
    bool send;
    bool modify;
    bool insert;
    bool transferOverflow;
    // if ACT_ID_INSERT_FROM_TABLE happens and (total_len % KRNL_DATA_WIDTH / 8) equals to 0 or (KRNL_DATA_WIDTH / 8
    // -
    // 1),
    // the packet overflows to a new transfer, then the residual bytes should be emitted without reading in new
    // packet.
    bool readEnable;
    ap_uint<X3_RX_UWIDTH> userReg;
    SendState currState;
    SendState nextState;
    // additional 16 bit for ACT_ID_INSERT_FROM_TABLE
    ap_uint<X3_RX_DWIDTH + 16> workReg;
    uint16_t keyFromTable;
    ap_uint<4> lastBytes = 0;
#ifdef __LIBRARY_PKT_FWD_DEBUG__
    int num_pkt = 0;
    while (num_pkt < TEST_PKT_NUM)
#else
    while (1)
#endif
    {
#pragma HLS loop_flatten off
#pragma HLS pipeline off
        ap_uint<20> ctrl = ctrlStrm.read();
        send = ctrl[0];
        modify = ctrl[1];
        insert = ctrl[2];
        transferOverflow = ctrl[3];
        keyFromTable = ctrl.range(19, 4);
        exit = false;
        currState = SendHeader;
        nextState = SendHeader;
        workReg = 0;
        userReg = 0;
        readEnable = true;
        uint32_t cntr = 0;
        bool meta_eop = false;
        bool meta_eod = false;
        bool packet_status = true;
        ap_uint<8> lastBytes = 0;

    PACKET_EMIT:
        while (!exit) {
#pragma HLS pipeline II = 1
            if (readEnable) {
                ap_uint<X3_RX_DWIDTH + 1> in = inStrm.read();
                cntr += (X3_RX_DWIDTH / 8);
                workReg.range(X3_RX_DWIDTH - 1, 0) = in.range(X3_RX_DWIDTH, 1);
                userReg = inUser.read();
            }
            switch (currState) {
                case SendHeader:
                    // only ACT_ID_INSERT_FROM_TABLE changes 1st header packet
                    // updates length information
                    if ((cntr == 20) && insert) {
                        // network header is big-endian
                        uint16_t total_len =
                            getUnsigned<OFFSET_IP_TOTAL_LEN % 32, LEN_IP_TOTAL_LEN, KRNL_DATA_WIDTH + 16>(workReg) +
                            ap_uint<2>(2);
                        workReg.range(LEN_IP_TOTAL_LEN - 1 + (OFFSET_IP_TOTAL_LEN % 32), OFFSET_IP_TOTAL_LEN % 32) =
                            ((total_len << 8) | (total_len >> 8));
                    }else if (cntr == 40) {
                        if (insert) {
                            uint16_t total_len =
                                getUnsigned<OFFSET_UDP_LEN % 32, LEN_UDP_LEN, KRNL_DATA_WIDTH + 16>(workReg) +
                                ap_uint<2>(2);
                            workReg.range((OFFSET_UDP_LEN % 32) + LEN_UDP_LEN - 1, OFFSET_UDP_LEN % 32) =
                                ((total_len >> 8) | (total_len << 8));
                        }
                        nextState = ModifyPacket;
                    }
                    exit = false;
                    break;
                case ModifyPacket:
                    if (cntr >= 48){
                        if (insert) {
                            // shift Payload
                            ap_uint<16> unsentData = workReg.range(31,16);
                            workReg <<= 16;
                            workReg.range(15,0) = keyFromTable;

                        } else if (modify) {
                            // replaces the payload with the corresponding table key
                            workReg.range(15, 0) = keyFromTable;
                        }
                        nextState = SendPacket;
                        if (userReg.range(1, 1)) { // data eop
                            if (transferOverflow) {
                                nextState = SendOverflow;
                            }
                    }
                    }
                    break;
                case SendPacket:
                    if (insert) {
                        ap_uint<16> unsentData = workReg.range(X3_RX_DWIDTH + 15, X3_RX_DWIDTH);
                        workReg <<= 16;
                        workReg.range(15, 0) = unsentData;
                    }
                    if (userReg.range(1, 1)) { // data eop
                        if (transferOverflow) {
                            nextState = SendOverflow;
                        }
                    }
                    if (send == false && userReg.range(19, 19)) {
                        exit = true;
                    }
                    break;
                case SendOverflow:
                    // set the rest bytes of checksum in last packet to zero
                    {
                        ap_uint<16> unsentData = workReg.range(X3_RX_DWIDTH + 15, X3_RX_DWIDTH);
                        workReg = 0;
                        workReg.range(15, 0) = unsentData;
                    }
                    break;
                // in exception, reset the state machine
                default:
                    nextState = SendHeader;
            }
            if (send) {
                ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, 0, 0> out;
                out.data = workReg.range(X3_RX_DWIDTH - 1, 0);
                out.user = userReg;
                out.last = 0;

                if (out.user.range(1, 1) && insert) {
                    unsigned int lBytes = 32 - __builtin_clz(out.user.range(9, 2).to_uint());
                    lBytes = (lBytes + 2) % (X3_RX_DWIDTH / 8);
                    lBytes = (lBytes == 0) ? 4 : lBytes;
                    out.user.range(9, 2) = (1 << lBytes) - 1;
                }

                if (out.user.range(1, 1) && transferOverflow) { // eod but overflow
                    out.user.range(1, 1) = 0;                   // mark it as non end of data
                    lastBytes = out.user.range(9, 2);

                    meta_eod = true;
                    if (out.user.range(19, 19)) readEnable = false;
                }

                if (out.user.range(19, 19) &&
                    transferOverflow) { // eom received but can't be passed on as one iteration left
                    out.user.range(19, 19) = 0;
                    meta_eop = true;
                    packet_status = out.user.range(101, 101);
                }

                if (currState == SendOverflow) {
                    out.user.range(1, 1) = meta_eod; // mark eod
                    out.user.range(9, 2) = lastBytes;
                    meta_eod = false;
                    if (meta_eop) {
                        out.user.range(19, 19) = 1; // mark eom as already received earlier
                        out.user.range(101, 101) = packet_status;
                    }
                }

                if (out.user.range(19, 19)) { // received eom, mark exit
                    exit = true;
                    out.last = 1;

#ifdef __LIBRARY_PKT_FWD_DEBUG__
                    num_pkt++;
#endif
                }
                outStrm.write(out);
            }
            // Update state
            currState = nextState;
        }
    }
}
#endif // _XFNETWORKING_PKT_FWD_HPP_

/**
* Copyright (C) 2019-2021 Xilinx, Inc
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
#pragma once
#include "vpp_acc.hpp"
#include "vpp_stream.hpp"
#include "ap_int.h"
#include "config.hpp"

enum { cmd_drop, cmd_add, cmd_sample, cmd_sent };

const int ringSz = 10;

struct Pkt {
    int nr;
    int dt;
};

typedef ap_int<1> Bit;

typedef vpp::stream<Pkt, 32> PktStream;
typedef vpp::stream<int, 32> IntStream;
typedef vpp::stream<Bit, 32> BitStream;

class ETH : public VPP_ACC<ETH, 1> {
   public:
    ZERO_COPY(dIn);
    ZERO_COPY(dOut);

    SYS_PORT(dIn, MEM_BANK0);
    SYS_PORT(dOut, MEM_BANK1);

    FREE_RUNNING(eth_rx);
    FREE_RUNNING(fsk_mod);
    FREE_RUNNING(fsk_smp);
    FREE_RUNNING(eth_tx);

    static void compute(int cmd, Pkt* dIn, Pkt* dOut);

    static void control(int cmd,
                        Pkt* dIn,
                        Pkt* dOut,
                        IntStream& dropS,
                        IntStream& addS,
                        IntStream& reqS,
                        PktStream& smpS,
                        BitStream& getS,
                        IntStream& sntS);

    static void eth_rx(PktStream& Ax);

    static void fsk_mod(PktStream& Ax, PktStream& Bx, IntStream& dropS, IntStream& addS);

    static void fsk_smp(PktStream& Bx, PktStream& Cx, IntStream& reqS, PktStream& smpS);

    static void eth_tx(PktStream& Cx, BitStream& getS, IntStream& sntS);
};

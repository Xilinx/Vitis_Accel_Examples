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
#include "eth.hpp"

#if 1 || defined(__SYNTHESIS__)
#define dbg(...)
#else
#define dbg(...)                                         \
    do {                                                 \
        std::lock_guard<std::mutex> guard(g_cout_mutex); \
        printf(__VA_ARGS__);                             \
        fflush(0);                                       \
    } while (0)
#endif

void ETH::eth_rx(PktStream& Ax) {
#pragma HLS pipeline II = 1
    static int nr = 0;
    Pkt pkt = {nr, nr % 10};
    nr++;
    dbg("rx: [%d] %d\n", pkt.nr, pkt.dt);
    Ax.write(pkt);
}

void ETH::fsk_mod(PktStream& Ax, PktStream& Bx, IntStream& dropS, IntStream& addS) {
#pragma HLS pipeline II = 1
    static int drop = -1;
    static int add = 0;
    dropS.read_nb(drop);
    addS.read_nb(add);
    Pkt pkt = Ax.read();
    if (pkt.dt != drop) {
        dbg("mod: [%d] %d + %d\n", pkt.nr, pkt.dt, add);
        pkt.dt += add;
        Bx.write(pkt);
    }
}

void ETH::fsk_smp(PktStream& Bx, PktStream& Cx, IntStream& reqS, PktStream& smpS) {
#pragma HLS pipeline II = 1
    static int smp = 0;
    Pkt pkt = Bx.read();
    if (!smp) {
        reqS.read_nb(smp);
    } else {
        smpS.write(pkt);
        dbg("smp: [%d] %d\n", pkt.nr, pkt.dt);
        smp--;
    }
    Cx.write(pkt);
}

void ETH::eth_tx(PktStream& Cx, BitStream& getS, IntStream& sntS) {
#pragma HLS pipeline II = 1
    static int snt = 0;
    Pkt pkt = Cx.read();
    snt++;
    dbg("tx: [%d] %d\n", pkt.nr, pkt.dt);
    Bit get;
    if (getS.read_nb(get)) {
        sntS.write(snt);
    }
}

void ETH::control(int cmd,
                  Pkt* dIn,
                  Pkt* dOut,
                  IntStream& dropS,
                  IntStream& addS,
                  IntStream& reqS,
                  PktStream& smpS,
                  BitStream& getS,
                  IntStream& sntS) {
    Pkt pkt;
    switch (cmd) {
        case cmd_drop:
            pkt = dIn[0];
            dbg("ctl: dropping %d\n", pkt.dt);
            dropS.write(pkt.dt);
            break;
        case cmd_add:
            pkt = dIn[0];
            dbg("ctl: adding %d\n", pkt.dt);
            addS.write(pkt.dt);
            break;
        case cmd_sample:
            pkt = dIn[0];
            dbg("ctl: requesting %d samples\n", pkt.dt);
            reqS.write(pkt.dt);
            for (int i = 0; i < pkt.dt; i++) {
                dOut[i] = smpS.read();
            }
            break;
        case cmd_sent:
            dbg("ctl: sent count\n");
            getS.write(1);
            dOut[0].dt = sntS.read();
            break;
        default:
            dbg("ctl: unknown cmd %d\n", cmd);
    }
    dbg("ctl: return\n");
}

void ETH::compute(int cmd, Pkt* dIn, Pkt* dOut) {
    static IntStream dropS("drop");
    static IntStream addS("add");
    static IntStream reqS("req");
    static PktStream smpS("smp");
    static BitStream getS("get");
    static IntStream sntS("snt");
    static PktStream Ax("Ax", /*post_check=*/false);
    static PktStream Bx("Bx", /*post_check=*/false);
    static PktStream Cx("Cx", /*post_check=*/false);

    control(cmd, dIn, dOut, dropS, addS, reqS, smpS, getS, sntS);
    eth_rx(Ax);
    fsk_mod(Ax, Bx, dropS, addS);
    fsk_smp(Bx, Cx, reqS, smpS);
    eth_tx(Cx, getS, sntS);
}

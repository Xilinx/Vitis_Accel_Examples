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

#ifndef NET_UTILS_HPP
#define NET_UTILS_HPP

#include "x3_specs.hpp"

#include "vpp_acc.hpp"
#include "vpp_stream.hpp"

#include <stdint.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

typedef ap_axiu<X3_DWIDTH, X3_UWIDTH, 0, 0> AxiData;
typedef ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, 0, 0> AxiRxData;
typedef ap_axiu<X3_RX_PAUSE, 0, 0, 0> AxiRxPause;
typedef ap_axiu<X3_TX_DWIDTH, X3_TX_UWIDTH, 0, 0> AxiTxData;
typedef ap_axiu<X3_TX_META_RES_DWIDTH, 0, 0, 0> AxiTxRspMeta;
typedef ap_axiu<X3_TX_PAUSE, 0, 0, 0> AxiTxPause;

template <int SCH_WIDTH = 0>
using AxiX3RxData = ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, SCH_WIDTH, SCH_WIDTH>;

template <int SCH_WIDTH = 0>
using AxiX3TxData = ap_axiu<X3_TX_DWIDTH, X3_TX_UWIDTH, SCH_WIDTH, SCH_WIDTH>;

template <int SCH_WIDTH = 0>
using AxiX3TxMetaResp = ap_axiu<X3_TX_META_RES_DWIDTH, SCH_WIDTH, SCH_WIDTH, SCH_WIDTH>;

template <int SCH_WIDTH = 0>
using AxiX3RxPause = ap_axiu<X3_RX_PAUSE, SCH_WIDTH, SCH_WIDTH, SCH_WIDTH>;

//ap_axiu</*data-width=*/32,/*user-width=*/32, 0, 0>
using DataPort = hls::stream<AxiData>;
using RxDataPort = hls::stream<AxiRxData>;
using RxPausePort = hls::stream<AxiRxPause>;
using TxDataPort = hls::stream<AxiTxData>;
using TxMetaRspPort = hls::stream<AxiTxRspMeta>;
using TxPausePort = hls::stream<AxiTxPause>;

using X3Data = X3Data_dt<X3_DWIDTH, X3_UWIDTH>;
using X3RxData = X3RxData_dt<X3_RX_DWIDTH, X3_RX_UWIDTH>;
using X3TxData = X3TxData_dt<X3_TX_DWIDTH, X3_TX_UWIDTH>;
using X3TxMetaRsp = X3TxMeta_dt<X3_TX_META_RES_DWIDTH>;

using RxPktStream = vpp::stream<X3RxData, 32>;
using TxPktStream = vpp::stream<X3TxData, 512>;

template <int DWIDTH, int KEEPW = 0>
struct __attribute__((packed)) AXIWord_dt {
    ap_uint<DWIDTH> data;
    ap_uint<(KEEPW ? KEEPW: (DWIDTH / 8))> keep;
    bool last;
};

constexpr int maxBitsUsed(int maxVal) {
    return (32 - __builtin_clz(maxVal));
}

constexpr int getDataPortWidth(int maxVal) {
    return maxBitsUsed(maxVal);
}

template <int DWIDTH>
uint8_t countSetBits(ap_uint<DWIDTH> val) {
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < DWIDTH; ++i) {
#pragma HLS UNROLL
        cnt += val.range(i, i);
    }
    return cnt;
}

inline ap_uint<32> netOrder(ap_uint<32> inVal) {
#pragma HLS inline
    ap_uint<32> outVal;
    outVal.range(7, 0) = inVal.range(31, 24);
    outVal.range(15, 8) = inVal.range(23, 16);
    outVal.range(23, 16) = inVal.range(15, 8);
    outVal.range(31, 24) = inVal.range(7, 0);
    return outVal;
}

#endif // NET_UTILS_HPP

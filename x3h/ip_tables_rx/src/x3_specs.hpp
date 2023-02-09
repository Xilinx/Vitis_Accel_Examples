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
#ifndef X3_SPECS_HPP
#define X3_SPECS_HPP
#include "ap_int.h"

const auto X3_DWIDTH = 32;
const auto X3_UWIDTH = 8;

const auto X3_RX_DWIDTH = 32;
const auto X3_RX_UWIDTH = 128;
const auto X3_RX_PAUSE = 16;

const auto X3_TX_DWIDTH = 32;
const auto X3_TX_META_RES_DWIDTH = 128;
const auto X3_TX_UWIDTH = 64;
const auto X3_TX_PAUSE = 16;

const auto X3_TOD_DWIDTH = 64;

template <int DWIDTH, int UWIDTH>
struct __attribute__((packed)) X3_Rx_Pkt_Stream_dt {
    ap_uint<DWIDTH> tdata;
    ap_uint<UWIDTH> tuser;
    ap_uint<DWIDTH / 8> tkeep;
    bool tlast;
};

template <int DWIDTH>
struct __attribute__((packed)) X3_Rx_Pkt_Stream_Meta_dt {
    ap_uint<DWIDTH> tdata;
    ap_uint<DWIDTH / 8> tkeep;
    bool tlast;
};

template <int DWIDTH, int UWIDTH>
struct __attribute__((packed)) X3Data_dt {
    ap_uint<DWIDTH> data;
    ap_uint<UWIDTH> user;
    ap_uint<DWIDTH / 8> keep;
    bool last;
};

template <int DWIDTH, int UWIDTH>
struct __attribute__((packed)) X3RxData_dt {
    ap_uint<DWIDTH> data;
    ap_uint<UWIDTH> user;
    ap_uint<DWIDTH / 8> keep;
    bool last;
};

template <int DWIDTH>
struct __attribute__((packed)) X3RxMeta_dt {
    ap_uint<DWIDTH> data;
    ap_uint<DWIDTH / 8> keep;
    bool last;
};

template <int DWIDTH, int UWIDTH>
struct __attribute__((packed)) X3TxData_dt {
    ap_uint<DWIDTH> data;
    ap_uint<UWIDTH> user;
    ap_uint<DWIDTH / 8> keep;
    bool last;
};

template <int DWIDTH>
struct __attribute__((packed)) X3TxMeta_dt {
    ap_uint<DWIDTH> data;
    ap_uint<DWIDTH / 8> keep;
    bool last;
};
#endif // X3_SPECS_HPP

/*
 * Copyright 2022 Xilinx, Inc.
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
 */
#ifndef _XF_PLCTRL_UNTILS_HPP_
#define _XF_PLCTRL_UNTILS_HPP_
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "enums.hpp"
#include "hls_stream.h"
namespace xf {
namespace plctrl {

static void sleep(ap_uint<32> num_cycles) {
#ifndef __SYNTHESIS__
  int cnt = 0;
#endif
  for (int i = 0; i < num_cycles; ++i) {
#ifndef __SYNTHESIS__
    cnt++;
#else
    _ssdm_op_Wait();
#endif
  }
}
static void sync(hls::stream<ap_axiu<8, 0, 0, 0>> &sync_out,
                 hls::stream<ap_axiu<8, 0, 0, 0>> &sync_in) {

#pragma HLS inline
  ap_axiu<8, 0, 0, 0> req;
  req.data = SYNC_REQ;
  req.keep = -1;
  req.last = 1;
  sync_out.write(req);
// make sure the read after the write;
#ifdef __SYNTHESIS__
  _ssdm_op_Wait();
#endif
  ap_axiu<8, 0, 0, 0> ack = sync_in.read();
#ifndef __SYNTHESIS__
  assert(ack.data == SYNC_ACK);
#endif
}

/**
 *@param in_strm_0 fixed to control packet stream in
 *@param in_strm_1 data packet in
 *@param o_strm merged output stream
 */
template <int W>
static void merge_one(hls::stream<ap_axiu<32, 0, 0, 0>> &in_strm_0,
                      hls::stream<ap_axiu<W, 0, 0, 0>> &in_strm_1,
                      hls::stream<ap_axiu<W, 0, 0, 0>> &o_strm) {

  bool rd_0 = false;
  bool rd_1 = false;
  ap_axiu<32, 0, 0, 0> in_0;
  ap_axiu<W, 0, 0, 0> in_1;
  ap_uint<W> out = 0;
  ap_uint<W / 8> keep = 0;
  // W can be 32, 64, 128
  const int C_N = W / 32;
  ap_uint<8> c_n = 0;
  ;
  while (1) {
#pragma HLS pipeline II = 1
    if (rd_0 || (!rd_1 && !in_strm_0.empty())) {
      in_0 = in_strm_0.read();
      ap_uint<32> k_o = keep;
      bool w_e = false;
      if (rd_0) {
        // same transaction
        out((c_n + 1) * 32 - 1, c_n * 32) = in_0.data;
        k_o((c_n + 1) * 4 - 1, c_n * 4) = -1;
        if (c_n < C_N - 1) {
          keep = k_o;
          c_n++;
          w_e = false;
          // when reach the bound
        } else {
          keep = 0;
          c_n = 0;
          w_e = true;
        }
      } else {
        // new transaction
        out(31, 0) = in_0.data;
        k_o(3, 0) = -1;
        keep = k_o;
        if (C_N == 1) {
          c_n = 0;
          w_e = true;
        } else {
          c_n = 1;
          w_e = false;
        }
      }
      rd_0 = !in_0.last;
      if (in_0.last || w_e) {
        ap_axiu<W, 0, 0, 0> x_o;
        x_o.data = out;
        x_o.keep = k_o;
        x_o.last = in_0.last;
        o_strm.write(x_o);
      }
    } else if (rd_1 || !in_strm_1.empty()) {
      in_1 = in_strm_1.read();
      rd_1 = !in_1.last;
      o_strm.write(in_1);
    }
#ifndef __SYNTHESIS__
    else {
      break;
    }
#endif
  }
}
// the header from in_strm_0 to control the end of this function
template <int N, int W>
static void merge(hls::stream<ap_axiu<32, 0, 0, 0>> in_strm_0[N],
                  hls::stream<ap_axiu<W, 0, 0, 0>> in_strm_1[N],
                  hls::stream<ap_axiu<W, 0, 0, 0>> o_strm[N]) {
#pragma HLS dataflow
  for (int i = 0; i < N; ++i) {
#pragma HLS unroll
    merge_one<W>(in_strm_0[i], in_strm_1[i], o_strm[i]);
  }
}
} // end of namespace plctrl
} // end of namespace xf
#endif

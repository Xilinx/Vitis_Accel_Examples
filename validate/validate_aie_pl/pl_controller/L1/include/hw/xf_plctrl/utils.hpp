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

#ifndef _XF_PLCTRL_UNTILS_HPP_
#define _XF_PLCTRL_UNTILS_HPP_
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "xf_plctrl/enums.hpp"
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
static void sync(hls::stream<ap_axiu<32, 0, 0, 0> >& sync_out,
                 hls::stream<ap_axiu<32, 0, 0, 0> >& sync_in,
                 ap_uint<32> pld) {
#pragma HLS inline
    ap_axiu<32, 0, 0, 0> req;
    req.data = SYNC_REQ;
    req.keep = -1;
    req.last = 0;
    sync_out.write(req);
    req.data = pld;
    req.last = 1;
    sync_out.write(req);
// make sure the read after the write;
#ifdef __SYNTHESIS__
    _ssdm_op_Wait();
#endif
    ap_axiu<32, 0, 0, 0> ack = sync_in.read();
#ifndef __SYNTHESIS__
    assert(ack.data == SYNC_ACK);
#endif
}

// the header from in_strm_0 to control the end of this function
static void merge(hls::stream<ap_axiu<32, 0, 0, 0> >& in_strm_0,
                  hls::stream<ap_axiu<32, 0, 0, 0> >& in_strm_1,
                  hls::stream<ap_axiu<32, 0, 0, 0> >& o_strm) {
    bool e = false;
    bool rd_0 = false;
    bool rd_1 = false;
    ap_axiu<32, 0, 0, 0> in;
    while (!e) {
#pragma HLS pipeline II = 1
        if (rd_0 || (!rd_1 && !in_strm_0.empty())) {
            in = in_strm_0.read();
            // check header
            ap_uint<32> hd = in.data;
            // invalid header to indicate the end of stream
            if (hd == 0x7fffffff) {
                e = true;
            }
            rd_0 = !in.last;
            if (!e) o_strm.write(in);
        } else if (rd_1 || !in_strm_1.empty()) {
            in = in_strm_1.read();
            rd_1 = !in.last;
            o_strm.write(in);
        }
    }
}
} // end of namespace plctrl
} // end of namespace xf
#endif

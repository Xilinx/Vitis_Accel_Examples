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

#ifndef _XF_PLCTRL_GRAPH_CTRL_HPP_
#define _XF_PLCTRL_GRAPH_CTRL_HPP_

#include "ctrl_pkt_utils.hpp"
#include "drivers/aiengine/xaiengine/xaiegbl_params.h"

namespace xf {
namespace plctrl {

static void updateAIERTP(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                         ap_uint<32> ctrlPktID,
                         ap_int<32> rtpVal,
                         ap_uint<32> rtpAddr,
                         ap_uint<32> rtpSelAddr,
                         bool ping_pong) {
#pragma HLS inline
    controlPacketWrite(ctrlOut, ctrlPktID, rtpAddr, 1, rtpVal);
}
static void setAIEIterations(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                             ap_uint<32> ctrlPktID,
                             ap_uint<32> num_iter,
                             ap_uint<32> iter_mem_addr) {
#pragma HLS inline
    // set num_iter
    controlPacketWrite(ctrlOut, ctrlPktID, iter_mem_addr, 1, num_iter);
}

static void enableAIECores(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut, ap_uint<32> ctrlPktID) {
#pragma HLS inline
    // enable AIE cores
    controlPacketWrite(ctrlOut, ctrlPktID, XAIEGBL_CORE_CORECTRL, 1, 0x1);
}

static void disableAIECores(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut, ap_uint<32> ctrlPktID) {
#pragma HLS inline
    // set disable-event for AIE cores
    controlPacketWrite(ctrlOut, ctrlPktID, XAIEGBL_CORE_CORECTRL, 1, 0x0);
}

} // end of namespace plctrl
} // end of xf
#endif

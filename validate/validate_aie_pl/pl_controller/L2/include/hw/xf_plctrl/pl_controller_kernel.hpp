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

#ifndef _XF_PLCTRL_PL_CONTROLLER_KERNEL_HPP_
#define _XF_PLCTRL_PL_CONTROLLER_KERNEL_HPP_
#include "xf_plctrl/dma_ctrl.hpp"
#include "xf_plctrl/graph_ctrl.hpp"
#include "xf_plctrl/enums.hpp"
#include "xf_plctrl/utils.hpp"

/* pl controller kernel
 * @param ctrlOut control packet stream out to AIE
 * @param ctrlIn control packet stream in from AIE
 * @param ctrlPktID control packet ID of ctrlOut.
 * @param ctrlRspID control packet ID of ctrlIn
 * @param pc buffer storing microcode
 * @param metadata buffer storing metatdata
 * @param output buffer to stroing data read from AIE, such as RTP read.
 */
extern "C" void pl_controller_kernel(hls::stream<ap_axiu<32, 0, 0, 0> >& ctrlOut,
                                     hls::stream<ap_axiu<32, 0, 0, 0> >& syncOut,
                                     hls::stream<ap_axiu<32, 0, 0, 0> >& syncIn,
                                     ap_uint<32> ctrlPktID,
                                     ap_uint<32>* pm);
#endif

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
#ifndef _XF_PLCTRL_PL_CONTROLLER_KERNEL_HPP_
#define _XF_PLCTRL_PL_CONTROLLER_KERNEL_HPP_
#include "dma_ctrl.hpp"
#include "enums.hpp"
#include "graph_ctrl.hpp"
#include "utils.hpp"

namespace xf {
namespace plctrl {
/* pl controller kernel
 * @param ctrlOut control packet stream out to AIE
 * @param ctrlIn control packet stream in from AIE
 * @param ctrlPktID control packet ID of ctrlOut.
 * @param ctrlRspID control packet ID of ctrlIn
 * @param pc buffer storing microcode
 * @param metadata buffer storing metatdata
 * @param output buffer to stroing data read from AIE, such as RTP read.
 */
template <int CTRL_NM>
void pl_controller_kernel(hls::stream<ap_axiu<32, 0, 0, 0>> ctrlOut[CTRL_NM],
                          hls::stream<ap_axiu<8, 0, 0, 0>> &syncOut,
                          hls::stream<ap_axiu<8, 0, 0, 0>> &syncIn,
                          ap_uint<32> ctrlPktID, ap_uint<32> *pm) {
  printf("pl_ctrl started\n");
  ap_uint<32> pm_buf[1024];
  ap_uint<32> num = pm[0];
#ifndef __HLS_SYNTHESIS__
  if (num > 1024)
    printf("ERROR: num of micro-codes %d exceeds 1024\n", (int)num);
#endif
  for (int i = 0; i < num; ++i) {
#pragma HLS pipeline II = 1
    pm_buf[i] = pm[i + 1];
  }

  ap_uint<32> opcode;
  bool end_flag = false;
  ap_uint<32> loop_addr = 0;
  ap_uint<32> loop_count = 0;
  ap_uint<32> pc = 0;
  while (!end_flag) {
    opcode = pm_buf[pc++];
    printf("opcode = %d\n", (unsigned int)opcode);
    switch (opcode) {
    case xf::plctrl::SET_AIE_ITERATION: {
      ap_uint<32> num_iter = pm_buf[pc++];
      ap_uint<32> iter_mem_addr = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: SET_AIE_INTERATION, num_iter: %d\n",
             (unsigned int)num_iter);
#endif
      xf::plctrl::setAIEIterations(ctrlOut[strm_id], ctrlPktID, num_iter,
                                   iter_mem_addr);
      break;
    }
    case xf::plctrl::ENABLE_AIE_CORES: {
      ap_uint<32> strm_id = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: ENABLE_AIE_CORES\n");
#endif
      xf::plctrl::enableAIECores(ctrlOut[strm_id], ctrlPktID);
      break;
    }
    case xf::plctrl::UPDATE_AIE_RTP: {
#ifndef __HLS_SYNTHESIS__
      printf("opcode: UPDATE_AIE_RTP\n");
#endif
      ap_int<32> rtpVal = pm_buf[pc++];
      ap_uint<32> rtpAddr = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
      xf::plctrl::updateAIERTP(ctrlOut[strm_id], ctrlPktID, rtpVal, rtpAddr);
      break;
    }
    case xf::plctrl::DISABLE_AIE_CORES: {
      printf("opcode: DISABLE_AIE_CORES\n");
      ap_uint<32> strm_id = pm_buf[pc++];
      xf::plctrl::disableAIECores(ctrlOut[strm_id], ctrlPktID);
      break;
    }
    case xf::plctrl::SET_DMA_BD: {
      printf("opcode: SET_DMA_BD\n");
      ap_uint<32> dma_bd_number = pm_buf[pc++];
      ap_uint<32> dma_bd_value = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
      xf::plctrl::setDmaBd(ctrlOut[strm_id], ctrlPktID, dma_bd_number,
                           dma_bd_value);
      break;
    }
    case xf::plctrl::ENQUEUE_DMA_BD: {
      ap_uint<32> dma_bd_number = pm_buf[pc++];
      ap_uint<32> dma_channel = pm_buf[pc++];
      bool dma_s2mm = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: ENQUEUE_DMA_BD, dma_bd_number: %d, dma_channel: %d, "
             "dma_s2mm: %d\n",
             (unsigned int)dma_bd_number, (unsigned int)dma_channel,
             (unsigned int)dma_s2mm);
#endif
      xf::plctrl::enqueueDmaBd(ctrlOut[strm_id], ctrlPktID, dma_bd_number,
                               dma_channel, dma_s2mm);
      break;
    }
    case xf::plctrl::SLEEP: {
      ap_uint<32> num_cycles = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: SLEEP, length: %d\n", (unsigned int)num_cycles);
#endif
      xf::plctrl::sleep(num_cycles);
      break;
    }
    case xf::plctrl::HALT: {
      printf("opcode: HALT\n");
      end_flag = true;
      break;
    }
    case xf::plctrl::SYNC: {
      printf("opcode: SYNC\n");
      xf::plctrl::sync(syncOut, syncIn);
      break;
    }
    case xf::plctrl::LOOP_BEGIN: {
      printf("opcode: LOOP_BEGIN\n");
      // set address register and iteration regsiter
      loop_count = pm_buf[pc++] - 1;
      loop_addr = pc;
      break;
    }
    case xf::plctrl::WRITE: {
      ap_uint<32> reg_addr = pm_buf[pc++];
      ap_uint<32> reg_val = pm_buf[pc++];
      // ap_uint<32> hd = xf::plctrl::generateHeader(0, ctrlPktID);
      // ap_axiu<32,0,0,0> x;
      // x.keep = -1;
      // x.data = hd;
      // x.last = 0;
      // ctrlOut.write(x);
      // hd = xf::plctrl::generateControlHeader(reg_addr, 0, 0);
      // x.data = hd;
      // ctrlOut.write(x);
      // x.data = reg_val;
      // x.last = 1;
      // ctrlOut.write(x);
      break;
    }
    case xf::plctrl::LOOP_END: {
      printf("opcode: LOOP_END\n");
      // check iteration register
      // if 0, end of loop
      // if not 0, goto address register
      if (loop_count != 0) {
        loop_count--;
        pc = loop_addr;
      }
      break;
    }
    default: {
#ifndef __HLS_SYNTHESIS__
      printf("plControllerKernel: ERROR: not supported opcode by now%d\n",
             (unsigned int)opcode);
#endif
      break;
    }
    }
  }
}
// to workaround the HLS bug
static void pl_controller_kernel_1(hls::stream<ap_axiu<32, 0, 0, 0>> &ctrlOut,
                                   hls::stream<ap_axiu<8, 0, 0, 0>> &syncOut,
                                   hls::stream<ap_axiu<8, 0, 0, 0>> &syncIn,
                                   ap_uint<32> ctrlPktID, ap_uint<32> *pm) {
  printf("pl_ctrl started\n");
  ap_uint<32> pm_buf[1024];
  ap_uint<32> num = pm[0];
#ifndef __HLS_SYNTHESIS__
  if (num > 1024)
    printf("ERROR: num of micro-codes %d exceeds 1024\n", (int)num);
#endif
  for (int i = 0; i < num; ++i) {
#pragma HLS pipeline II = 1
    pm_buf[i] = pm[i + 1];
  }

  ap_uint<32> opcode;
  bool end_flag = false;
  ap_uint<32> loop_addr = 0;
  ap_uint<32> loop_count = 0;
  ap_uint<32> pc = 0;
  while (!end_flag) {
    opcode = pm_buf[pc++];
    printf("opcode = %d\n", (unsigned int)opcode);
    switch (opcode) {
    case xf::plctrl::SET_AIE_ITERATION: {
      ap_uint<32> num_iter = pm_buf[pc++];
      ap_uint<32> iter_mem_addr = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: SET_AIE_INTERATION, num_iter: %d\n",
             (unsigned int)num_iter);
#endif
      xf::plctrl::setAIEIterations(ctrlOut, ctrlPktID, num_iter, iter_mem_addr);
      break;
    }
    case xf::plctrl::ENABLE_AIE_CORES: {
      ap_uint<32> strm_id = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: ENABLE_AIE_CORES\n");
#endif
      xf::plctrl::enableAIECores(ctrlOut, ctrlPktID);
      break;
    }
    case xf::plctrl::UPDATE_AIE_RTP: {
#ifndef __HLS_SYNTHESIS__
      printf("opcode: UPDATE_AIE_RTP\n");
#endif
      ap_int<32> rtpVal = pm_buf[pc++];
      ap_uint<32> rtpAddr = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
      xf::plctrl::updateAIERTP(ctrlOut, ctrlPktID, rtpVal, rtpAddr);
      break;
    }
    case xf::plctrl::DISABLE_AIE_CORES: {
      printf("opcode: DISABLE_AIE_CORES\n");
      ap_uint<32> strm_id = pm_buf[pc++];
      xf::plctrl::disableAIECores(ctrlOut, ctrlPktID);
      break;
    }
    case xf::plctrl::SET_DMA_BD: {
      printf("opcode: SET_DMA_BD\n");
      ap_uint<32> dma_bd_number = pm_buf[pc++];
      ap_uint<32> dma_bd_value = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
      xf::plctrl::setDmaBd(ctrlOut, ctrlPktID, dma_bd_number, dma_bd_value);
      break;
    }
    case xf::plctrl::ENQUEUE_DMA_BD: {
      ap_uint<32> dma_bd_number = pm_buf[pc++];
      ap_uint<32> dma_channel = pm_buf[pc++];
      bool dma_s2mm = pm_buf[pc++];
      ap_uint<32> strm_id = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: ENQUEUE_DMA_BD, dma_bd_number: %d, dma_channel: %d, "
             "dma_s2mm: %d\n",
             (unsigned int)dma_bd_number, (unsigned int)dma_channel,
             (unsigned int)dma_s2mm);
#endif
      xf::plctrl::enqueueDmaBd(ctrlOut, ctrlPktID, dma_bd_number, dma_channel,
                               dma_s2mm);
      break;
    }
    case xf::plctrl::SLEEP: {
      ap_uint<32> num_cycles = pm_buf[pc++];
#ifndef __HLS_SYNTHESIS__
      printf("opcode: SLEEP, length: %d\n", (unsigned int)num_cycles);
#endif
      xf::plctrl::sleep(num_cycles);
      break;
    }
    case xf::plctrl::HALT: {
      printf("opcode: HALT\n");
      end_flag = true;
      break;
    }
    case xf::plctrl::SYNC: {
      printf("opcode: SYNC\n");
      xf::plctrl::sync(syncOut, syncIn);
      break;
    }
    case xf::plctrl::LOOP_BEGIN: {
      printf("opcode: LOOP_BEGIN\n");
      // set address register and iteration regsiter
      loop_count = pm_buf[pc++] - 1;
      loop_addr = pc;
      break;
    }
    case xf::plctrl::WRITE: {
      ap_uint<32> reg_addr = pm_buf[pc++];
      ap_uint<32> reg_val = pm_buf[pc++];
      // ap_uint<32> hd = xf::plctrl::generateHeader(0, ctrlPktID);
      // ap_axiu<32,0,0,0> x;
      // x.keep = -1;
      // x.data = hd;
      // x.last = 0;
      // ctrlOut.write(x);
      // hd = xf::plctrl::generateControlHeader(reg_addr, 0, 0);
      // x.data = hd;
      // ctrlOut.write(x);
      // x.data = reg_val;
      // x.last = 1;
      // ctrlOut.write(x);
      break;
    }
    case xf::plctrl::LOOP_END: {
      printf("opcode: LOOP_END\n");
      // check iteration register
      // if 0, end of loop
      // if not 0, goto address register
      if (loop_count != 0) {
        loop_count--;
        pc = loop_addr;
      }
      break;
    }
    default: {
#ifndef __HLS_SYNTHESIS__
      printf("plControllerKernel: ERROR: not supported opcode by now%d\n",
             (unsigned int)opcode);
#endif
      break;
    }
    }
  }
}
}
}

#endif

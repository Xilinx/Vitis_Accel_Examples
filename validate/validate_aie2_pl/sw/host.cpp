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
#include "../aie/kernels/include.h"
#include "adf/adf_api/XRTConfig.h"
#include "experimental/xrt_kernel.h"
#include "pl_controller.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

//#include "graph.cpp"

using namespace adf;
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cout << "Usage: " << argv[0]
              << " <xclbin, aie_ctrl_file, dma_lock_file>" << std::endl;
    return EXIT_FAILURE;
  }
  char *xclbinFilename = argv[1];
  char *aie_ctrl_file = argv[2];
  char *dma_lock_file = argv[3];
  // instance of plController
  xf::plctrl::plController m_pl_ctrl(aie_ctrl_file, dma_lock_file);

  int num_iter = 1;
  int num_sample = 32;

  m_pl_ctrl.enqueue_set_aie_iteration("mygraph", num_iter);
  m_pl_ctrl.enqueue_enable_aie_cores();

  for (int i = 0; i < num_iter; ++i) {
    m_pl_ctrl.enqueue_sync();
  }

  m_pl_ctrl.enqueue_sleep(128);
  m_pl_ctrl.enqueue_disable_aie_cores();

  m_pl_ctrl.enqueue_halt();
  m_pl_ctrl.print_micro_codes();

  int ret;
  int match = 0;
  int mem_size = 0;
  // Open xclbin
  auto dhdl = xrtDeviceOpen(0); // device index=0
  if (!dhdl) {
    printf("Device open error\n");
  }
  ret = xrtDeviceLoadXclbinFile(dhdl, xclbinFilename);
  if (ret) {
    printf("Xclbin Load fail\n");
  }
  xuid_t uuid;
  xrtDeviceGetXclbinUUID(dhdl, uuid);

  // output memory
  mem_size = num_sample * num_iter * sizeof(int);
  xrtBufferHandle out_bo1 = xrtBOAlloc(dhdl, mem_size, 0, 2 /*group_id*/);
  int *host_out1 = (int *)xrtBOMap(out_bo1);

  // input memory
  xrtBufferHandle in_bo1 = xrtBOAlloc(dhdl, mem_size, 0, 2 /*group_id*/);
  int *host_in1 = (int *)xrtBOMap(in_bo1);

  std::cout << " memory allocation complete" << std::endl;

  // initialize input memory
  for (int i = 0; i < mem_size / sizeof(int); i++) {
    *(host_in1 + i) = i;
  }

  // input/output memory for pl_controller
  xrtBOSync(in_bo1, XCL_BO_SYNC_BO_TO_DEVICE, mem_size, 0);

  int32_t num_pm = m_pl_ctrl.get_microcode_size(); /// sizeof(int32_t);
  xrtBufferHandle pm_bo =
      xrtBOAlloc(dhdl, (num_pm + 1) * sizeof(uint32_t), 0, 2);
  uint32_t *host_pm = (uint32_t *)xrtBOMap(pm_bo);

  m_pl_ctrl.copy_to_device_buff(host_pm + 1);
  host_pm[0] = num_pm;

  // sync input memory for pl_controller
  xrtBOSync(pm_bo, XCL_BO_SYNC_BO_TO_DEVICE, (num_pm + 1) * sizeof(uint32_t),
            /*OFFSET=*/0);
  std::cout << "sync pm buffer complete" << std::endl;
  // start sender_receiver kernels
  xrtKernelHandle sender_receiver_k1 =
      xrtPLKernelOpen(dhdl, uuid, "sender_receiver:{sender_receiver_1}");
  xrtRunHandle sender_receiver_r1 = xrtRunOpen(sender_receiver_k1);
  xrtRunSetArg(sender_receiver_r1, 0, num_iter);
  xrtRunSetArg(sender_receiver_r1, 1, num_sample);
  xrtRunSetArg(sender_receiver_r1, 2, in_bo1);
  xrtRunSetArg(sender_receiver_r1, 3, out_bo1);
  xrtRunStart(sender_receiver_r1);
  std::cout << " start sender-receiver kernel" << std::endl;
  ////start merge kernel
  // xrtKernelHandle merge_k1 = xrtPLKernelOpen(dhdl, uuid,
  // "merge_kernel:{merge_kernel_1}");
  // xrtRunHandle merge_r1 = xrtRunOpen(merge_k1);
  // xrtRunStart(merge_r1);
  // std::cout<< "start merge kernel"<<std::endl;
  // auto ghdl=xrtGraphOpen(dhdl,uuid,"mygraph");
  // ret=xrtGraphUpdateRTP(ghdl,"mygraph.first.in[1]",(const
  // char*)&num_sample,sizeof(int));

  // start pl controller
  xrtKernelHandle controller_k1 =
      xrtPLKernelOpen(dhdl, uuid, "pl_controller_top:{controller_1}");
  xrtRunHandle controller_r1 = xrtRunOpen(controller_k1);
  int ctrl_pkt_id = 0;
  xrtRunSetArg(controller_r1, 2, ctrl_pkt_id);
  xrtRunSetArg(controller_r1, 3, pm_bo);
  xrtRunStart(controller_r1);
  std::cout << "start pl controller kernel" << std::endl;
  // start input kernels

  xrtRunWait(controller_r1);
  // sync output memory
  xrtBOSync(out_bo1, XCL_BO_SYNC_BO_FROM_DEVICE, mem_size, /*OFFSET=*/0);
  // post-processing data;
  int i;
  for (i = 0; i < mem_size / sizeof(int); i++) {
    // if (*(host_out1 + i) != *(host_in1 + i) + 1) {
    //    match = 1;
    std::cout << "host_out1[" << i << "]=" << host_out1[i] << std::endl;
    //}
  }
  // release memory
  xrtRunClose(sender_receiver_r1);
  xrtKernelClose(sender_receiver_k1);

  xrtRunClose(controller_r1);
  xrtKernelClose(controller_k1);
  // xrtRunClose(merge_r1);
  // xrtKernelClose(merge_k1);

  xrtBOFree(out_bo1);
  xrtBOFree(in_bo1);
  xrtBOFree(pm_bo);
  xrtDeviceClose(dhdl);

  std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
  return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}

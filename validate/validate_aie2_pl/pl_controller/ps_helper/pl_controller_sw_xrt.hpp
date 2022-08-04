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
#ifndef PL_CONTORLLER_SW_XRT_HPP
#define PL_CONTORLLER_SW_XRT_HPP
#include <experimental/xrt_kernel.h>
#include <cstring>
#include <unordered_map>
#include "kernel/utils.hpp"

namespace pl_controller {
class pl_controller_sw_xrt {
   private:
    xrt::device device;
    // xrt::uuid uuid;
    xrt::kernel pl_ctrl_krnl;
    xrt::bo ucode_bo;
    xrt::bo cfg_buf_bo;
    xrt::bo out_buf_bo;
    uint32_t* ucode_bo_mapped;
    uint32_t* cfg_buf_bo_mapped;
    uint32_t* out_buf_bo_mapped;

    uint32_t ucode_size;
    uint32_t ucode_offset;
    uint32_t cfg_buf_size;
    uint32_t out_buf_size;

    std::unordered_map<std::string, int> ext_map;

    uint32_t pre_ucode_size;
    uint32_t* pre_ucode_buff;
    uint32_t core_control_size;
    uint32_t* core_control_buff;
    uint32_t core_iter_size;
    uint32_t* core_iter_buff;

    // get the size of ucode and runtime address
    int getSize(const std::string& ucodeFile, size_t& u_size, size_t& rt_size);

   public:
    // Open device
    // load xclbin
    // obtain object from xclbin
    pl_controller_sw_xrt(xrt::device device, const char* xclbinFilename);
    pl_controller_sw_xrt(xrt::device device, const xrt::uuid& uuid);
    pl_controller_sw_xrt(xrt::device device, const xrt::uuid& uuid, const std::string& ucodeFile);

    ~pl_controller_sw_xrt();

    int enqueueGraphRun(uint32_t iter);
    int enqueueRuntimeControl();
    int enqueueGraphEnd();

    int getGroupId();
    // allcoate buffer for uncode
    // allocate buffer for runtime address
    // load ucode to ucode buffer
    int loadMicroCode(const std::string& ucodeFile);

    // set runtime address
    int setAddress(std::string name, uint64_t addr);

    // transfer data
    // start kernel
    int run(uint64_t aieBase);

    // if timeout = 0, disable timeout
    // if timeout, return -1, otherwise return 0
    int wait(uint32_t timeout);
};
}
#endif

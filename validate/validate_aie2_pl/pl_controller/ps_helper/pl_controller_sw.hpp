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
#ifndef XF_PL_CONTORLLER_SW_HPP
#define XF_PL_CONTORLLER_SW_HPP
#include <adf.h>
extern "C" {
#include <xaiengine.h>
}
#include "adf/adf_api/AIEControlConfig.h"
#include "xil_io.h"
#include "drivers/aiengine/xaiengine/xaiemlgbl_params.h"
#include "../kernel/utils.hpp"
#include <cstring>
#include <unordered_map>
#include <unistd.h>

using namespace adf;
namespace pl_controller {

class pl_controller_sw {
   private:
    const uint32_t max_size = 1048576;
    const uint32_t max_cfg_sz = 256;

   public:
    uint32_t* buff;
    uint32_t* cfg_buf;
    uint32_t offset;
    uint32_t* out_buf;
    std::unordered_map<std::string, int> ext_map;
    pl_controller_sw();
    ~pl_controller_sw();

    void setAddress(std::string name, uint64_t addr);
    /**@brief Set arguments for PL controller kernel and start it
     * @param controllerBase base address of PL controller kernel.
     * @param ctrlOffset offset of control register
     */
    void run(uint64_t controllerBase, uint64_t ctrlOffset, uint64_t aieBase = 0x20000000000);
    /**@brief wait for PL controller done.
     * @param controllerBase base address of PL controller kernel.
     * @param ctrlOffset offset of control register
     */
    void wait(uint64_t controllerBase, uint64_t ctrlOffset);
};
}
#endif

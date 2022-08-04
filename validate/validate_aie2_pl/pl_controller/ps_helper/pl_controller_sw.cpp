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
#include "pl_controller_sw.hpp"

namespace pl_controller {
pl_controller_sw::pl_controller_sw() {
    buff = (uint32_t*)GMIO::malloc(max_size * sizeof(uint32_t));
    cfg_buf = new uint32_t[max_cfg_sz];
    memset(cfg_buf, 0, sizeof(uint32_t) * max_cfg_sz);
    offset = 0;
    // printf("create pl controller object\n");
}

pl_controller_sw::~pl_controller_sw() {
    GMIO::free(buff);
}

void pl_controller_sw::setAddress(std::string name, uint64_t addr) {
    try {
        int id = ext_map.at(name);
        std::cout << "setAddress: name=" << name << ", id=" << id << ", addr=" << addr << std::endl;
        cfg_buf[id * 2 + 2] = addr & 0xffffffff;
        cfg_buf[id * 2 + 3] = (addr >> 32) & 0xffffffff;
        cfg_buf[1] += 1;
    } catch (const std::out_of_range& e) {
        std::cout << "[ERROR] cannot find " << name << " in pl_controller_sw::ext_map\n";
        exit(-1);
    }
}

void pl_controller_sw::wait(uint64_t controllerBase, uint64_t ctrlOffset) {
    while (1) {
        uint32_t v = Xil_In32(controllerBase + ctrlOffset);
        if (v & 6) {
            break;
        }
        sleep(10);
    }
    std::cout << "out_buf=" << out_buf[0] << std::endl;
}

void pl_controller_sw::run(uint64_t controllerBase, uint64_t ctrlOffset, uint64_t aieBase) {
    std::cout << std::hex << "controllerBase=" << controllerBase << ", ctrlOffset=" << ctrlOffset
              << ", aieBase=" << aieBase << std::endl;
    uint64_t buff_addr = (uint64_t)buff;
    uint64_t cfg_buf_addr = (uint64_t)cfg_buf;
    out_buf = new uint32_t[32];
    uint64_t out_buf_addr = (uint64_t)out_buf;
    cfg_buf[0] = offset;
    for (int i = 0; i < cfg_buf[1] * 2 + 2; i++) std::cout << "cfg_buf[" << i << "]=" << cfg_buf[i] << std::endl;

    Xil_Out32(controllerBase + 0x10, (uint32_t)buff_addr);
    Xil_Out32(controllerBase + 0x14, (uint32_t)(buff_addr >> 32));
    Xil_Out32(controllerBase + 0x1C, (uint32_t)cfg_buf_addr);
    Xil_Out32(controllerBase + 0x20, (uint32_t)(cfg_buf_addr >> 32));
    Xil_Out32(controllerBase + 0x28, (uint32_t)out_buf_addr);
    Xil_Out32(controllerBase + 0x2C, (uint32_t)(out_buf_addr >> 32));
    Xil_Out32(controllerBase + 0x34, (uint32_t)aieBase);
    Xil_Out32(controllerBase + 0x38, (uint32_t)(aieBase >> 32));
    Xil_Out32(controllerBase + 0x40, (uint32_t)aieBase);
    Xil_Out32(controllerBase + 0x44, (uint32_t)(aieBase >> 32));
    Xil_Out32(controllerBase + ctrlOffset, 0x1);
}

} // end of namespace

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
#include "pl_controller_sw_xrt.hpp"
#include <fstream>
#include <unistd.h>

namespace pl_controller {
pl_controller_sw_xrt::pl_controller_sw_xrt(xrt::device m_device, const char* xclbinFilename) {
    //// Open device
    // unsigned int dev_index = 0;
    // device = xrt::device(dev_index); // device index=0
    device = m_device;
    // load xclbin
    auto uuid = device.load_xclbin(xclbinFilename);

    // obtain object from xclbin
    pl_ctrl_krnl = xrt::kernel(device, uuid, "pl_controller_hw", true); // cu_access_mode::exclusive
}
pl_controller_sw_xrt::pl_controller_sw_xrt(xrt::device m_device, const xrt::uuid& uuid) {
    //// Open device
    // unsigned int dev_index = 0;
    // device = xrt::device(dev_index); // device index=0
    device = m_device;
    // obtain object from xclbin
    pl_ctrl_krnl = xrt::kernel(device, uuid, "pl_controller_hw", true); // cu_access_mode::exclusive
}

pl_controller_sw_xrt::pl_controller_sw_xrt(xrt::device m_device, const xrt::uuid& uuid, const std::string& ucodeFile) {
    device = m_device;
    // obtain object from xclbin
    pl_ctrl_krnl = xrt::kernel(device, uuid, "pl_controller_hw", true); // cu_access_mode::exclusive
    // load micro code
    std::ifstream fin(ucodeFile, std::ios::binary);
    if (!fin) {
        std::cout << "[ERROR] pl_controller_sw_xrt: open file fail!\n";
    }
    uint32_t u_sz, rt_sz;
    fin.read((char*)&u_sz, sizeof(uint32_t));
    fin.read((char*)&rt_sz, sizeof(uint32_t));
    pre_ucode_size = u_sz / sizeof(uint32_t);
    pre_ucode_buff = new uint32_t[pre_ucode_size];
    for (uint32_t i = 0; i < pre_ucode_size; i++) {
        uint32_t val = 0;
        if (fin.read((char*)&val, sizeof(uint32_t))) {
            pre_ucode_buff[i] = val;
        } else {
            printf("UCODE number is not correct\n");
        }
    }
    uint32_t map_sz = 0;
    fin.read((char*)&map_sz, sizeof(int));
    for (int i = 0; i < map_sz; i++) {
        uint32_t str_sz;
        fin.read((char*)&str_sz, sizeof(int));
        char tmp[str_sz + 1];
        fin.read(tmp, str_sz);
        tmp[str_sz] = '\0';
        ext_map[tmp] = i;
        std::cout << "loadMicroCode: name=" << tmp << ", id=" << i << std::endl;
    }
    // iteration
    fin.read((char*)&core_iter_size, sizeof(uint32_t));
    std::cout << "core_iter_size=" << core_iter_size << std::endl;
    core_iter_buff = new uint32_t[core_iter_size];
    for (int i = 0; i < core_iter_size; i++) {
        uint32_t val;
        if (fin.read((char*)&val, sizeof(uint32_t))) {
            core_iter_buff[i] = val;
        } else {
            printf("UCODE number is not correct\n");
        }
    }
    // core control
    fin.read((char*)&core_control_size, sizeof(uint32_t));
    core_control_buff = new uint32_t[core_control_size];
    std::cout << "core_control_size=" << core_control_size << std::endl;
    for (int i = 0; i < core_control_size; i++) {
        uint32_t val;
        if (fin.read((char*)&val, sizeof(uint32_t))) {
            core_control_buff[i] = val;
        } else {
            printf("UCODE number is not correct\n");
        }
    }
    fin.close();

    // allcoate buffer for uncode
    ucode_size = (pre_ucode_size + core_iter_size * 3 + core_control_size * 3 * 2) * sizeof(uint32_t);
    ucode_offset = 0;
    ucode_bo = xrt::bo(device, ucode_size, pl_ctrl_krnl.group_id(0));
    ucode_bo_mapped = ucode_bo.map<uint32_t*>();

    // allocate buffer for runtime address
    cfg_buf_size = (map_sz + 2) * sizeof(uint64_t);
    cfg_buf_bo = xrt::bo(device, cfg_buf_size, pl_ctrl_krnl.group_id(1));
    cfg_buf_bo_mapped = cfg_buf_bo.map<uint32_t*>();
    cfg_buf_bo_mapped[1] = map_sz;
    std::cout << "ucode_size=" << ucode_size << ", cfg_buf_size=" << cfg_buf_size << std::endl;
}

pl_controller_sw_xrt::~pl_controller_sw_xrt() {}

int pl_controller_sw_xrt::getGroupId() {
    return pl_ctrl_krnl.group_id(0); // ucode buffer
}

int pl_controller_sw_xrt::enqueueGraphRun(uint32_t iter) {
    for (int i = 0; i < core_iter_size; i++) {
        ucode_bo_mapped[ucode_offset++] = WRITE_MM;
        ucode_bo_mapped[ucode_offset++] = core_iter_buff[i];
        ucode_bo_mapped[ucode_offset++] = iter;
    }
    for (int i = 0; i < core_control_size; i++) {
        ucode_bo_mapped[ucode_offset++] = WRITE_MM;
        ucode_bo_mapped[ucode_offset++] = core_control_buff[i];
        ucode_bo_mapped[ucode_offset++] = 1;
    }
    return 0;
}

int pl_controller_sw_xrt::enqueueRuntimeControl() {
    memcpy(&ucode_bo_mapped[ucode_offset], pre_ucode_buff, pre_ucode_size * sizeof(uint32_t));
    ucode_offset += pre_ucode_size - 1;
    return 0;
}

int pl_controller_sw_xrt::enqueueGraphEnd() {
    for (int i = 0; i < core_control_size; i++) {
        ucode_bo_mapped[ucode_offset++] = WRITE_MM;
        ucode_bo_mapped[ucode_offset++] = core_control_buff[i];
        ucode_bo_mapped[ucode_offset++] = 0;
    }
    ucode_bo_mapped[ucode_offset++] = 0; // halt
    return 0;
}

int pl_controller_sw_xrt::loadMicroCode(const std::string& ucodeFile) {
    std::ifstream fin(ucodeFile, std::ios::binary);
    if (!fin) {
        std::cout << "[ERROR] loadMicroCode: open file fail!\n";
    }
    // first 2 32-bits storing the ucode size and runtime address id size
    fin.read((char*)&ucode_size, sizeof(uint32_t));
    fin.read((char*)&cfg_buf_size, sizeof(uint32_t));
    cfg_buf_size += sizeof(uint32_t) * 2;
    std::cout << "ucode_size=" << ucode_size << ", cfg_buf_size=" << cfg_buf_size << std::endl;

    // allcoate buffer for uncode
    ucode_bo = xrt::bo(device, ucode_size, pl_ctrl_krnl.group_id(0));
    ucode_bo_mapped = ucode_bo.map<uint32_t*>();

    // allocate buffer for runtime address
    cfg_buf_bo = xrt::bo(device, cfg_buf_size, pl_ctrl_krnl.group_id(1));
    cfg_buf_bo_mapped = cfg_buf_bo.map<uint32_t*>();

    uint32_t val = 0;
    for (uint32_t i = 0; i < ucode_size / sizeof(uint32_t); i++) {
        if (fin.read((char*)&val, sizeof(uint32_t))) {
            ucode_bo_mapped[i] = val;
            // std::cout << "ucode[" << i << "]: " << val << std::endl;
        } else {
            printf("UCODE number is not correct\n");
        }
    }
    uint32_t map_sz = 0;
    fin.read((char*)&map_sz, sizeof(int));
    cfg_buf_bo_mapped[1] = map_sz;
    std::cout << "external buffer size: " << map_sz << std::endl;
    for (int i = 0; i < map_sz; i++) {
        uint32_t str_sz;
        fin.read((char*)&str_sz, sizeof(int));
        char tmp[str_sz + 1];
        fin.read(tmp, str_sz);
        tmp[str_sz] = '\0';
        ext_map[tmp] = i;
        std::cout << "loadMicroCode: name=" << tmp << ", id=" << i << std::endl;
    }
    fin.close();
    return 0;
}

int pl_controller_sw_xrt::setAddress(std::string name, uint64_t addr) {
    try {
        int id = ext_map.at(name);
        std::cout << "setAddress: name=" << name << ", id=" << id << ", addr=" << addr << std::endl;
        cfg_buf_bo_mapped[id * 2 + 2] = addr & 0xffffffff;
        cfg_buf_bo_mapped[id * 2 + 3] = (addr >> 32) & 0xffffffff;
    } catch (const std::out_of_range& e) {
        std::cout << "[ERROR] cannot find " << name << " in pl_controller_sw::ext_map\n";
        exit(-1);
    }
    return 0;
}

// if timeout,return -1;
// else return 0;
int pl_controller_sw_xrt::wait(uint32_t timeout) {
    std::cout << "Waiting PL controller done....." << std::endl;
    while (true) {
        unsigned int status;
        status = pl_ctrl_krnl.read_register(0x0);
        if (status == 6 || status == 4) {
            break;
        }
        sleep(2);
    }
    out_buf_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    std::cout << "out_buf=" << out_buf_bo_mapped[0] << std::endl;
    std::cout << "PL controller done!" << std::endl;
    return 0;
}

int pl_controller_sw_xrt::run(uint64_t aieBase) {
    uint32_t high_addr = aieBase >> 32;
    uint32_t low_addr = aieBase;
    printf("high_addr = %d, low_addr = %d\n", high_addr, low_addr);

    uint32_t ucode_num = ucode_size / sizeof(uint32_t);
    cfg_buf_bo_mapped[0] = ucode_num;
    // set argumentfor pl controller kernel
    printf("kernel start setting argument\n");
    uint64_t ucode_bo_addr = ucode_bo.address();
    pl_ctrl_krnl.write_register(0x10, (uint32_t)ucode_bo_addr);
    pl_ctrl_krnl.write_register(0x14, (uint32_t)(ucode_bo_addr >> 32));

    // allocate buffer for configure
    uint64_t cfg_buf_bo_addr = cfg_buf_bo.address();
    pl_ctrl_krnl.write_register(0x1C, (uint32_t)cfg_buf_bo_addr);
    pl_ctrl_krnl.write_register(0x20, (uint32_t)(cfg_buf_bo_addr >> 32));

    // allocate buffer for runtime address
    out_buf_size = 1024 * sizeof(uint32_t);
    out_buf_bo = xrt::bo(device, out_buf_size, pl_ctrl_krnl.group_id(2));
    out_buf_bo_mapped = cfg_buf_bo.map<uint32_t*>();
    uint64_t out_buf_bo_addr = out_buf_bo.address();
    pl_ctrl_krnl.write_register(0x28, (uint32_t)out_buf_bo_addr);
    pl_ctrl_krnl.write_register(0x2C, (uint32_t)(out_buf_bo_addr >> 32));

    // aieBase
    pl_ctrl_krnl.write_register(0x34, (uint32_t)aieBase);
    pl_ctrl_krnl.write_register(0x38, (uint32_t)(aieBase >> 32));
    pl_ctrl_krnl.write_register(0x40, (uint32_t)aieBase);
    pl_ctrl_krnl.write_register(0x44, (uint32_t)(aieBase >> 32));

    printf("kernel set argument done\n");
    // sync bo
    ucode_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE, ucode_size, /*OFFSET=*/0);
    cfg_buf_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE, cfg_buf_size, /*OFFSET=*/0);
    printf("bo sync to device done\n");

    // start pl controller kernel
    pl_ctrl_krnl.write_register(0x0, 0x1);
    printf("kernel started\n");
    return 0;
}
// get the size of ucode and runtime address
int pl_controller_sw_xrt::getSize(const std::string& ucodeFile, size_t& u_size, size_t& rt_size) {
    std::ifstream fin(ucodeFile, std::ios::binary);
    if (!fin) {
        std::cout << "[ERROR] getSize: open file fail!\n";
    }
    // first 2 32-bits storing the ucode size and runtime address id size
    fin.read((char*)&u_size, sizeof(int));
    fin.read((char*)&rt_size, sizeof(int));
    // if(rt_size < 16 * sizeof(int))
    rt_size = 16 * sizeof(int);
    // printf("u_size = %d, rt_size = %d\n", u_size, rt_size);
    fin.close();
    return 0;
}
} // end of namespace

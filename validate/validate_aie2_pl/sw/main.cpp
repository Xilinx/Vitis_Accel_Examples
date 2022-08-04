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
#include "ps_helper/pl_controller_sw_xrt.hpp"
#include <experimental/xrt_aie.h>
#include <fstream>
#include "golden.hpp"

// FlexmlLayer4 g;

int main(const int argc, char* argv[]) {
    char* xclbinFileName = argv[1];
    char* ucodeFile = argv[2];

    int nerr = 0;
    const int ifm_size = 8 * 64 * 8 * 52;
    const int wts_size = 78848;
    const int ofm_size = 8 * 32 * 16 * 28;

    const uint64_t aieBase = 0x20000000000;
    // open device
    unsigned int dev_index = 0;
    auto device = xrt::device(dev_index);
    // create instance of pl_controller_sw_xrt
    auto uuid = device.load_xclbin(xclbinFileName);
    pl_controller::pl_controller_sw_xrt pl_ctrl_sw(device, uuid, ucodeFile);

    pl_ctrl_sw.enqueueGraphRun(1);
    pl_ctrl_sw.enqueueRuntimeControl();
    pl_ctrl_sw.enqueueGraphEnd();

    int group_id = pl_ctrl_sw.getGroupId();
    std::cout << "group_id=" << group_id << std::endl;

    // FIXME use TinyYolo data
    auto ifm_bo = xrt::bo(device, ifm_size, group_id);
    auto ifm_bo_map = ifm_bo.map<uint32_t*>();
    for (int i = 0; i < ifm_size / 4; i++) ifm_bo_map[i] = i;
    pl_ctrl_sw.setAddress("g.ifm_ddr", ifm_bo.address());

    auto wts_bo = xrt::bo(device, wts_size, group_id);
    auto wts_bo_map = wts_bo.map<uint32_t*>();
    for (int i = 0; i < wts_size / 4; i++) wts_bo_map[i] = i;
    pl_ctrl_sw.setAddress("g.wts_ddr", wts_bo.address());

    auto ofm_bo = xrt::bo(device, ofm_size, group_id);
    auto ofm_bo_map = ofm_bo.map<uint32_t*>();
    pl_ctrl_sw.setAddress("g.ofm_ddr", ofm_bo.address());

    // data transfer to DDR
    ifm_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE, ifm_size, 0);
    wts_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE, wts_size, 0);

    // start pl controller kernel
    // which write
    int rc = pl_ctrl_sw.run(aieBase);
    // wait done;
    rc = pl_ctrl_sw.wait(0);

    std::cout << "Graph wait done" << std::endl;
    ofm_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    for (int i = 0; i < ofm_size / 4; i++) {
        if ((uint32_t)ofm_bo_map[i] != golden[i]) {
            std::cout << "ofm_bo_map[" << i << "]=" << (uint32_t)ofm_bo_map[i] << ", " << golden[i] << std::endl;
            nerr++;
        }
    }
    std::cout << "check: nerr=" << nerr << std::endl;
    if (nerr)
        std::cout << "Test failed\n";
    else
        std::cout << "Test passed\n";
    return nerr;
}

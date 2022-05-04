/**
* Copyright (C) 2022 Xilinx, Inc
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
#include "sk_types.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "adf/adf_api/XRTConfig.h"
#include "xrt/experimental/xrt_aie.h"
#include "xrt/experimental/xrt_kernel.h"
#include <xaiengine.h>

#define SAMPLES 256

#ifdef __cplusplus
extern "C" {
#endif

class xrtHandles : public pscontext {
   public:
    xrt::device dhdl;
    xrt::graph graphhdl;
    xrtHandles(xclDeviceHandle dhdl_in, const xuid_t xclbin_uuid) : dhdl(dhdl_in), graphhdl(dhdl, xclbin_uuid, "mm") {
        graphhdl.run();
    }
};

__attribute__((visibility("default"))) xrtHandles* aie_kernel_init(xclDeviceHandle dhdl, const xuid_t xclbin_uuid) {
    xrtHandles* handles = new xrtHandles(dhdl, xclbin_uuid);

    return handles;
}

__attribute__((visibility("default"))) int aie_kernel(
    float* in_boA, float* in_boB, float* out_bo, int input_size, int output_size, xrtHandles* handles) {
    auto out_bohdl = xrt::aie::bo(handles->dhdl, output_size, 0, 0);
    auto in_bohdlA = xrt::aie::bo(handles->dhdl, input_size, 0, 0);
    auto in_bohdlB = xrt::aie::bo(handles->dhdl, input_size, 0, 0);

    auto inA_bo_map = in_bohdlA.map<float*>();
    auto inB_bo_map = in_bohdlB.map<float*>();
    auto out_bo_map = out_bohdl.map<float*>();

    memcpy(inA_bo_map, in_boA, input_size);
    memcpy(inB_bo_map, in_boB, input_size);

    //    handles->graphhdl[0].run(1);
    std::string gmioportA, gmioportB, gmioportC;
    int gmio_id = 0;
    gmioportA = std::to_string(gmio_id++);
    gmioportB = std::to_string(gmio_id++);
    in_bohdlB.sync("in_source2", XCL_BO_SYNC_BO_GMIO_TO_AIE, input_size, 0);
    std::cout << "syncAIEBO B \n";
    in_bohdlA.sync("in_source1", XCL_BO_SYNC_BO_GMIO_TO_AIE, input_size, 0);
    std::cout << "syncAIEBO A \n";

    gmioportC = std::to_string(gmio_id++);
    out_bohdl.sync("out_sink", XCL_BO_SYNC_BO_AIE_TO_GMIO, output_size, 0);

    memcpy(out_bo, out_bo_map, output_size);

    return 0;
}

__attribute__((visibility("default"))) int aie_kernel_fini(xrtHandles* handles) {
    std::cout << "Releasing remaining XRT objects...\n";
    handles->graphhdl.end();
    delete handles;
    return 0;
}

#ifdef __cplusplus
}
#endif

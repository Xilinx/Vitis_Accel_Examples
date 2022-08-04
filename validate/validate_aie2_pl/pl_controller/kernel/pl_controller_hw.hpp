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
#ifndef PL_CONTROLLER_HW_HPP
#define PL_CONTROLLER_HW_HPP

#include "ap_int.h"
#include "hls_stream.h"
#include "maxi.hpp"

#ifndef __SYNTHESIS__
#include <iostream>
#endif

#include "ucode_process.hpp"
#include "access_axi_mb.hpp"

// ADDR_SHIFT: 0, 1, 2, 3
// W support: 32, 64, 128, 256

#define ADDR_SHIFT 0
#define W (32 << ADDR_SHIFT)

/**
 * @brief pl_controller_hw PL controller Kernel interface
 *
 * @param ucode_buf input program memory for PL controller
 * @param cfg_buf configure parameter, 0: ucode size, 1: write latency, 2: runtime address size, 3+: runtime address
 * @param out_buf output information (todo for debugging)
 * @param aie_ctrl aie AXI-MM interface port to read/write AIE
 * @param aie_ctrl2 aie AXI-MM interface port to read/write AIE
 */
extern "C" void pl_controller_hw(ap_uint<32>* ucode_buf,
                                 ap_uint<32>* cfg_buf,
                                 ap_uint<32>* out_buf,
                                 maxi<ap_uint<W> > aie_ctrl,
                                 maxi<ap_uint<W> > aie_ctrl2);

#endif

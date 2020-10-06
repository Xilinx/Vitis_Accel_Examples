/**
* Copyright (C) 2020 Xilinx, Inc
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
/**
 * @brief pathgenerator.h
 * @brief This file includes two mainstream mathematical model which desribes
 * the dynamics of financial derivation
 */

#ifndef _XF_FINTECH_PATH_GENERATOR_H_
#define _XF_FINTECH_PATH_GENERATOR_H_
#include "ap_int.h"
#include "hls_math.h"
#include "hls_stream.h"
#include "xf_fintech/bs_model.hpp"
#include "xf_fintech/enums.hpp"
#include "xf_fintech/rng.hpp"
#include "xf_fintech/utils.hpp"
namespace xf {
namespace fintech {
namespace internal {

using namespace fintech::enums;

template <typename DT, bool StepFirst, int SampNum, bool WithAntithetic>
class BSPathGenerator {
   public:
    static const int OutN = WithAntithetic ? 2 : 1;

    xf::fintech::BSModel<DT> BSInst;
    // Constructor
    BSPathGenerator() {
#pragma HLS inline
    }

    void NextPath(ap_uint<16> steps,
                  ap_uint<16> paths,
                  hls::stream<DT> randNumberStrmIn[1],
                  hls::stream<DT> pathStrmOut[OutN]) {
#pragma HLS inline off
#ifndef __SYNTHESIS__
        static int cnt = 0;
#endif
        if (StepFirst) {
            for (int i = 0; i < paths; ++i) {
#pragma HLS loop_tripcount min = SampNum max = SampNum
                for (int j = 0; j < steps; ++j) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = 8 max = 8
                    DT dw = randNumberStrmIn[0].read();
#ifndef __SYNTHESIS__
#ifdef HLS_DEBUG
                    if (cnt < 30) std::cout << "dw=" << dw << std::endl;
                    cnt++;
#endif
#endif
                    DT dLogS = BSInst.logEvolve(dw);
#ifndef __SYNTHESIS__
#ifdef HLS_DEBUG
                    if (cnt < 30) std::cout << "s=" << s << std::endl;
#endif
#endif
                    pathStrmOut[0].write(dLogS);
                    if (WithAntithetic) {
                        DT dLogS_1 = BSInst.logEvolve(-dw);
                        pathStrmOut[1].write(dLogS_1);
                    }
                }
            }
        } else {
            for (int i = 0; i < steps; ++i) {
#pragma HLS loop_tripcount min = 8 max = 8
                for (int j = 0; j < paths; ++j) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = SampNum max = SampNum
                    DT dw = randNumberStrmIn[0].read();
#ifndef __SYNTHESIS__
#ifdef HLS_DEBUG
                    if (cnt < 30) std::cout << "dw=" << dw << std::endl;
                    cnt++;
#endif
#endif
                    DT dLogS = BSInst.logEvolve(dw);
#ifndef __SYNTHESIS__
#ifdef HLS_DEBUG
                    if (cnt < 30) std::cout << "s=" << s << std::endl;
#endif
#endif
                    pathStrmOut[0].write(dLogS);
                    if (WithAntithetic) {
                        DT dLogS_1 = BSInst.logEvolve(-dw);
                        pathStrmOut[1].write(dLogS_1);
                    }
                }
            }
        }
    }
};

} // namespace internal
} // namespace fintech
} // namespace xf

#endif //#ifndef XF_FINTECH_PATH_GENERATOR_H

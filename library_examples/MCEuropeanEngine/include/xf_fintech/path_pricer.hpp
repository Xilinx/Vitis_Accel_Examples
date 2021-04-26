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
 * @file path_pricer.hpp
 * @brief This file contains the path pricer for different option
 *
 * Path pircer calculates the payoff based on the value of underling asset.
 */
#ifndef XF_FINTECH_PATH_PRICER_H
#define XF_FINTECH_PATH_PRICER_H

#include "ap_int.h"
#include "hls_stream.h"
#include "utils.hpp"
#include "xf_fintech/enums.hpp"
#include "xf_fintech/utils.hpp"
#ifndef __SYNTHESIS__
#include <assert.h>
#endif

namespace xf {
namespace fintech {
namespace internal {
using namespace fintech::enums;

template <OptionStyle style, typename DT, bool StepFirst, int SampNum, bool WithAntithetic, int MaxSteps = 1024>
class PathPricer {
   public:
    const static unsigned int InN = WithAntithetic ? 2 : 1;
    const static unsigned int OutN = InN;

    PathPricer() {
#pragma HLS inline
    }

    void Pricing(ap_uint<16> steps,
                 ap_uint<16> paths,
                 hls::stream<DT> pathStrmIn[InN],
                 hls::stream<DT> priceStrmOut[OutN]) {
#ifndef __SYNTHESIS__
        printf("Option Style is not supported now!\n");
#endif
    }
};

template <typename DT, bool StepFirst, int SampNum, bool WithAntithetic>
class PathPricer<European, DT, StepFirst, SampNum, WithAntithetic> {
   public:
    const static unsigned int InN = WithAntithetic ? 2 : 1;
    const static unsigned int OutN = InN;

    const static bool byPassGen = false;

    // configuration of the path pricer
    DT strike;
    DT underlying;
    DT drift;
    DT discount;

    bool optionType;

    PathPricer() {
#pragma HLS inline
    }

    void PE(ap_uint<16> steps, ap_uint<16> paths, hls::stream<DT>& pathStrmIn, hls::stream<DT>& priceStrmOut) {
#pragma HLS inline off
#ifndef __SYNTHESIS__
        static int cnt = 0;
#endif
        DT s = underlying;
        for (int i = 0; i < paths; ++i) {
            for (int t = 1; t <= steps; t++) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = SampNum max = SampNum
                DT logS = pathStrmIn.read();
                if (t == 1) s = underlying;
                DT s1 = FPExp(logS);
                s = FPTwoMul(s, s1);
                if (t == steps) {
                    DT op1 = 0;
                    DT op2 = 0;
                    if (optionType) {
                        op1 = strike;
                        op2 = s;
                    } else {
                        op1 = s;
                        op2 = strike;
                    }
                    DT p1 = FPTwoSub(op1, op2);
                    DT payoff = MAX(p1, 0); // discount * MAX(s1, 0);
                    DT price = FPTwoMul(discount, payoff);
#ifndef __SYNTHESIS__
#ifdef HLS_DEBUG
                    if (cnt < 1024) {
                        std::cout << "pIn = " << pIn << std::endl;
                        std::cout << "payoff = " << payoff << std::endl;
                    }
                    cnt++;
#endif
#endif
                    priceStrmOut.write(price);
                }
            }
        }
    } // PE()
    void Pricing(ap_uint<16> steps,
                 ap_uint<16> paths,
                 hls::stream<DT> pathStrmIn[InN],
                 hls::stream<DT> priceStrmOut[OutN]) {
        for (int i = 0; i < InN; ++i) {
#pragma HLS unroll
            PE(steps, paths, pathStrmIn[i], priceStrmOut[i]);
        }
    }
};
}
} // namespace fintech
} // namespace xf
#endif //#ifndef XF_FINTECH_PATH_PRICER_H

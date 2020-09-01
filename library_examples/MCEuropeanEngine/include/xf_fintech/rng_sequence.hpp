/*
 * Copyright 2019 Xilinx, Inc.
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
/**
 * @file rng_sequence.hpp
 * @brief This files contains random sequence generator
 */

#ifndef XF_FINTECH_RNG_SEQ_H
#define XF_FINTECH_RNG_SEQ_H
#include "ap_int.h"
#include "hls_stream.h"
#ifndef __SYNTHESIS__
#include <assert.h>
#endif
namespace xf {
namespace fintech {
namespace internal {

template <typename DT, typename RNG>
class RNGSequence {
   public:
    const static unsigned int OutN = 1;
    ap_uint<32> seed[1];
    // Constructor
    RNGSequence(){
#pragma HLS inline
    };

    void Init(RNG rngInst[1]) { rngInst[0].seedInitialization(seed[0]); }

    void NextSeq(ap_uint<16> steps, ap_uint<16> paths, RNG rngInst[1], hls::stream<DT> randNumberStrmOut[1]) {
#pragma HLS inline off
    RNG_LOOP:
        for (int i = 0; i < paths; ++i) {
#pragma HLS loop_tripcount min = 1024 max = 1024
            for (int j = 0; j < steps; ++j) {
#pragma HLS pipeline II = 1
#pragma HLS loop_tripcount min = 8 max = 8
                DT d = rngInst[0].next();
#ifndef __SYNTHESIS__
#ifdef HLS_DEBUG
                std::cout << "randNumber =" << d << std::endl;
#endif
#endif
                randNumberStrmOut[0].write(d);
            }
        }
    }
};

} // namespace internal
} // namespace fintech
} // namespace xf
#endif

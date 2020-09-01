/**********
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
 * **********/

#ifndef XF_BLAS_KERNEL_HPP
#define XF_BLAS_KERNEL_HPP

#include "ddr.hpp"
#include "ddrType.hpp"
#include "gemmKernel.hpp"

extern "C" {

// Compute engine types
#if BLAS_runGemm == 1
typedef xf::blas::GemmKernel<BLAS_dataType,
                             BLAS_dataEqIntType,
                             BLAS_XdataType,
                             BLAS_ddrWidth,
                             BLAS_XddrWidth,
                             BLAS_gemmKBlocks,
                             BLAS_gemmMBlocks,
                             BLAS_gemmNBlocks>
    GemmType;
#endif


typedef xf::blas::TimeStamp<BLAS_numInstr> TimeStampType;

void blasKernel(DdrIntType* p_DdrRd, DdrIntType* p_DdrWr);

} // extern C

#endif

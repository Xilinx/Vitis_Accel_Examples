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
#include "transp.hpp"

extern "C" {

typedef xf::blas::TimeStamp<BLAS_numInstr> TimeStampType;

typedef xf::blas::Transp<BLAS_dataType, BLAS_ddrWidth, BLAS_transpBlocks, BLAS_transpBlocks> TranspType;

void blasKernel(DdrIntType* p_DdrRd, DdrIntType* p_DdrWr);

} // extern C

#endif

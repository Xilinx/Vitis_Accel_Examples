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
#pragma once
#include "hls_burst_maxi.h"

#ifndef __SYNTHESIS__
template <typename T>
using maxi = hls::burst_maxi<T>;
#else
#include <stddef.h>
#include <stdint.h>
#include "ap_int.h"

// Same to synthesis model, but allows to init from pointer
template <typename T>
class maxi {
   public:
    maxi(T* p) : Ptr(p) {
#pragma HLS inline
    }

    void read_request(size_t offset, unsigned len) {
#pragma HLS inline
        __fpga_maxi_read_req(&Ptr[offset], len);
    }

    T read() {
#pragma HLS inline
        T Tmp;
        __fpga_maxi_read(Ptr, &Tmp);
        return Tmp;
    }

    void write_request(size_t offset, unsigned len) {
#pragma HLS inline
        __fpga_maxi_write_req(&Ptr[offset], len);
    }

    void write(const T& val, ap_int<sizeof(T)> byte_enable_mask = -1) {
#pragma HLS inline
        __fpga_maxi_write(Ptr, &val, &byte_enable_mask);
    }

    void write_response() {
#pragma HLS inline
        __fpga_maxi_write_resp(Ptr);
    }

    // private:
    volatile T* Ptr;
};
#endif

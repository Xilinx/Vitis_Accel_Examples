/**
* Copyright (C) 2019-2021 Xilinx, Inc
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

#include "conv_filter_acc_wrapper.hpp"
#include <cstring>

int conv_filter_execute_fpga(const char coeffs[FILTER_V_SIZE][FILTER_H_SIZE],
                             float factor,
                             short bias,
                             unsigned short width,
                             unsigned short height,
                             unsigned int numImages,
                             YUVImage srcImage,
                             YUVImage dstImage) {
    auto srcBufPool = conv_acc::create_bufpool(vpp::input);
    auto dstBufPool = conv_acc::create_bufpool(vpp::output);
    auto coeffsBufPool = conv_acc::create_bufpool(vpp::input);

    int run = 0;
    int dataSizePerChannel = width * height;
    // sending input
    conv_acc::send_while([&]() -> bool {
        conv_acc::set_handle(run);
        unsigned char* srcBuf = (unsigned char*)conv_acc::alloc_buf(srcBufPool, 3 * dataSizePerChannel);
        unsigned char* dstBuf = (unsigned char*)conv_acc::alloc_buf(dstBufPool, 3 * dataSizePerChannel);
        char* coeffsBuf = (char*)conv_acc::alloc_buf(coeffsBufPool, FILTER_V_SIZE * FILTER_H_SIZE);

        // initialize all input data before parallel computes
        unsigned char* srcChannel[3] = {srcImage.yChannel, srcImage.uChannel, srcImage.vChannel};
        for (int ch = 0; ch < 3; ch++) {
            std::memcpy(srcBuf + ch * dataSizePerChannel, srcChannel[ch], dataSizePerChannel);
        }
        std::memcpy(coeffsBuf, coeffs, 256);
        // execute conv_acc<NCU> parallel computes
        for (int ch = 0; ch < 3; ch++) {
            conv_acc::compute(coeffsBuf, factor, bias, width, height, srcBuf + ch * dataSizePerChannel,
                              dstBuf + ch * dataSizePerChannel);
        }
        return (++run < numImages);
    });

    // receive lambda function for receive thread
    conv_acc::receive_all_in_order([&]() {
        int run = conv_acc::get_handle();
        unsigned char* dstBuf = (unsigned char*)conv_acc::get_buf(dstBufPool);
        unsigned char* dstChannel[3] = {dstImage.yChannel, dstImage.uChannel, dstImage.vChannel};
        for (int ch = 0; ch < 3; ch++) {
            std::memcpy(dstChannel[ch], dstBuf + ch * dataSizePerChannel, dataSizePerChannel);
        }
    });
    // wait for both loops to finish
    conv_acc::join();

    return 0;
}

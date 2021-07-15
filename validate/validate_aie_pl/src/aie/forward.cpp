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

#include "forward.hpp"

/*
 * This function is a AIE design. It forwards the data from the input window
 * to the output window. The length of data to be fowarded is 2048 * 4 bytes.
 * And the data is processed 32bits by 32bits in a pipelined manner.
 */

void forward(input_window_int32* input, output_window_int32* output) {
    v8int32* restrict input_buffer = (v8int32*)input->ptr;
    v16int32* restrict output_buffer = (v16int32*)output->ptr;

    v16int32 in_buf;
    v16int32 out_buf;

    v8int32* restrict ptr_in = (v8int32*)input_buffer;
    v16int32* restrict ptr_out = (v16int32*)output_buffer;

    for (int i = 0; i < 2048; i = i + 16)
        chess_prepare_for_pipelining chess_loop_range(8, )

        {
            in_buf = upd_w(in_buf, 0, *(ptr_in++));
            in_buf = upd_w(in_buf, 1, *(ptr_in++));

            out_buf = in_buf;

            *(ptr_out++) = (v16int32)out_buf;
        }
}

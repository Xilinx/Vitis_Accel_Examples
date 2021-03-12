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

#include <stdint.h>

extern "C" {
void dot_product_1(const uint32_t* a, const uint32_t* b, uint64_t* res, const int size, const int reps) {
// In this case it is not known before kernel compilation how many times
// operations on the 64 bit bundle will take place, therefore
// the port width is set as the data width value of 64.

loop_reps:
    for (int i = 0; i < reps; i++) {
    dot_product:
        for (int j = 0; j < size; j++) {
            res[j] = a[j] * b[j];
        }
    }
}
}

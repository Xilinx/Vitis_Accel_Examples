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

#define BUFFER_SIZE 65536
extern "C" {
void krnl_mmult(const unsigned int* input1, const unsigned int* input2, unsigned int* out, unsigned int size) {
    unsigned int v1_buffer[BUFFER_SIZE]; // Local memory to store vector1

Loop1:
    for (int i = 0; i < BUFFER_SIZE; i++) v1_buffer[i] = input1[i];

Loop_Mult:
    for (int i = 0; i < size; i++) out[i] = v1_buffer[i] * input2[i];
}

} // End of extern "C"

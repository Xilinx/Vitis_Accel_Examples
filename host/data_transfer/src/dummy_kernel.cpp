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

extern "C" {
void dummy_kernel(unsigned int* buffer0, unsigned int* buffer1, unsigned int size) {
// Intentional empty kernel as this example doesn't require actual
// kernel to work.

// Auto-pipeline is going to apply pipeline to this loop
dummy:
    for (unsigned int i = 0; i < size; i++) {
        buffer0[i] = buffer1[i];
    }
}
}

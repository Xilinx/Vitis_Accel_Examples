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
extern "C" {
void copy_kernel(int* a, int* b, const int n_elements, const int direction) {
    if (direction == 0) {
    copy_h2d:
        for (int i = 0; i < n_elements; i += 1) {
            b[i] = a[i];
        }
    } else {
    copy_d2h:
        for (int i = 0; i < n_elements; i += 1) {
            a[i] = b[i];
        }
    }
}
}

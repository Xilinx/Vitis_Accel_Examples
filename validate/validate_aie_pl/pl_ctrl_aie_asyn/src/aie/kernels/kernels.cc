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

/* A simple kernel
 */
#include "include.h"
#include <adf.h>

void simple(input_window_int32* win, output_window_int32* wout, int num_samples) {
    int32 tmp;
    printf("simple: Entering...\n");
    printf("simple: Setting window size to %d\n", 4 * num_samples);
    set_window_size(win, 4 * num_samples);
    set_window_size(wout, 4 * num_samples);
    for (unsigned i = 0; i < num_samples; i++) {
        window_readincr(win, tmp);
        printf("simple: i=%d, win=%d, wout=%d\n", i, tmp, tmp + 1);
        tmp++;
        window_writeincr(wout, tmp);
    }
    printf("simple: Returning...\n");
}

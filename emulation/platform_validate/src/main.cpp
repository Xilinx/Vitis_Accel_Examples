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

#include <xil_printf.h>
#include <stdlib.h>
#include <stdio.h>
#include <xil_io.h>

int main(void) {
    xil_printf("\n::  A72-0 started... \n");
    Xil_Out32(0x800000000, 0xA);
    Xil_Out32(0x50000000000, 0XB);
    xil_printf("\n::  A72-0:: Data write address @0x800000000 = 0xA \n");
    xil_printf("\n::  A72-0:: Data write address @0x50000000000 = 0xB \n");
    unsigned int data1 = Xil_In32(0x800000000);
    unsigned int data2 = Xil_In32(0x50000000000);
    Xil_Out32(0x0, data1 + data2);
    unsigned int sum = Xil_In32(0x0);
    xil_printf("\n::  A72-0:: Data read address @0x0 = 0x%x\n", sum);
    if (sum == 0x15) {
        xil_printf("\n::  PS DONE and Test PASSED.. \n");
    } else {
        xil_printf("\n::  PS DONE and Test FAILED.. \n");
    }
    return 0;
}

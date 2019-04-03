/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*
 Memory Coalescing

 Coalesced memory access allows the FPGA to reduce the number of global memory accesses
 by processing multiple work items at once. This increase in throughput without
 communication/latency cost. If the addresses to the global items are not aligned, 
 or not completely sequential then each work item do a individual global access which 
 result in bad throughput as access to DDR have high latency.

 If thread i is reading from memory location n, then have thread i+1 read from location n+1,
 then these reads are coalesced into one transaction. Merge requests to elements from the
 same memory bank into a single request. This increase local storage and involve pseudo
 vectorization, which might increase control complexity.

 There are instances where this technique will have little to no effect. If the DDR accesses
 are properly pipelined then this memory coalescing technique have no benefit. 
 proper vectorization might actually improve computation/control ratio

   Memory -  |___|___|___|___|___|___|___|___|___|
               |   |   |   |   |   |   |   |   |
               |   |   |   |   |   |   |   |   |
   Thread -   |0| |1| |2| |3| |4| |5| |6| |7| |8|

                (a) Coalesced
                

   Memory -  |___|___|___|___|___|___|___|___|___|___|
                 \/      \/       /   /   /   /   /
                 /\      /\      /   /   /   /   /
   Thread -    |0| |1| |2| |3| |4| |5| |6| |7| |8|

                (a) non-Coalesced
                
 */

/*******************************************************************************
Description: 
    Example to demonstrate how memory coalescing issue can be handled.
*******************************************************************************/
#include <string.h>
            
#define LENGTH 8
#define ITERATION 64
extern "C" {
void dummy_op(int *din1,int *din2,int *din3,int *dout)
{

#pragma HLS INTERFACE m_axi port=din1 offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=din2 offset=slave  bundle=gmem1

#if DEADLOCK==0 
    // Adding another m_axi interface to avoid deadlock
    #pragma HLS INTERFACE m_axi port=din3 offset=slave  bundle=gmem2
#else
    #pragma HLS INTERFACE m_axi port=din3 offset=slave  bundle=gmem
#endif
#pragma HLS INTERFACE m_axi port=dout offset=slave  bundle=gmem
#pragma HLS INTERFACE s_axilite port=din1 bundle=control
#pragma HLS INTERFACE s_axilite port=din2 bundle=control
#pragma HLS INTERFACE s_axilite port=din3 bundle=control
#pragma HLS INTERFACE s_axilite port=dout bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    //Deadlock/Hang: Vivado HLS coalesce the burst happening in the outer 
    //loop through gmem m_axi interface for variable din1 and another 
    //access on the same gmem m_axi inside lower loop using "din3" and 
    //that is hanging/deadlocking in hw emulation as well as on board.
    //Solution: Making another m_axi interface to avoid deadlock.
    UPPER_LOOP:for (int i = 0; i < ITERATION; i++) {
        int din2_buf_0[LENGTH];
        int din1_buf_0[LENGTH];
        memcpy(din2_buf_0,&din2[i*LENGTH], LENGTH * sizeof (int));
        memcpy(din1_buf_0,&din1[i*LENGTH], LENGTH * sizeof (int));
        int sum = 0;
        LOWER_LOOP:for (int j = 0; j <  LENGTH; j++) {
        #pragma HLS PIPELINE II=1
          int Si = din1_buf_0[j] + din2_buf_0[j] + din3[j];
          sum += Si;
        }
        dout[i] = sum;
    }
}
}

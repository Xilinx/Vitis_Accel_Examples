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

/******************************************************************************
Description:
    This example demonstrates how vectorization helps in less number of
    reads/writes to memory. It provides a basis for calculating the bandwidth
    utilization when compiler looking to vectorize.

******************************************************************************/

#define WORK_GROUP 256    //Work group size
#define DATA_SIZE 8192

// Tripcount identifiers
__constant int c_size = DATA_SIZE/WORK_GROUP;

__kernel __attribute__ ((reqd_work_group_size(16, 16, 1))) __attribute__((vec_type_hint(int)))
void vec_add(
            __global const int *in1, 
            __global const int *in2, 
            __global int *out_r,
            const int size
            ) 
{
    
    //Calculate t_size to know the number of operations each work group item perform
    int t_size = size/WORK_GROUP;

    //Calculate the index of each operation
    int index = 0;

    //get_local_id(id) gives the id in the x or y dimension
    //get_local_id(0) gives x-dimension and get_local_id(1) y-dimension
    int tid0 = get_local_id(0);
    int tid1 = get_local_id(1);

    //get_local_size(id) gives the local size of x or y dimension
    //get_local_size(0) gives x local size and get_local_size(1) y local size
    int lsize0 = get_local_size(0);
    int lsize1 = get_local_size(1);

    //With vectorization, from each of the in1 and in2 vectors 16 data read happens
    //to perform addition operation.
    //Once the vector addition is complete, the result 16 data is burst written to out_r.
    //Please look at the profile summary report, the number of reads and writes will
    //be less with vectorization compared to without.
    //Work items in the work group access consecutive memory locations which 
    //leads to memory coalescing access from in1 and in2.

    //Each work group item performs the 'size' number of operations
    //In this case the size(t_size) is 32. Since 256 work group items are there,
    //multiply by 32 gives 8192 operations corresponds to the total array size.
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    for(int i = 0; i < t_size; i++) {
        index =  i*lsize0*lsize1 + tid1*lsize0 + tid0;
        out_r[index] = in1[index] + in2[index];
    }
}




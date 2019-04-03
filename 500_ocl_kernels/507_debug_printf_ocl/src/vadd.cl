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

/*******************************************************************************
Description:
    This Example demonstrate the printf usage inside OpenCL C Kernel. Xilinx 
    Support printf only for OpenCL C Kernel. It is not supported C/C++ HLS kernel.

    Note: It is recommended to use printf only for kernel code debugging purposes. 
    Avoid printf usage inside kernel when targeting for real application. User may 
    not get the optimized kernel implementation with printf as tool will try to 
    implement printf functionality while compiling kernel and as a result, it 
    could ignore some critical performance specific attributes like unrolling/
    pipelining.

*******************************************************************************/

#include "host.h"

__kernel void __attribute__ ((reqd_work_group_size(WORKGROUP_SIZE, 1, 1))) 
// WORKGROUP_SIZE is set to 16 in host.h
krnl_vadd(__global int* a, __global int* e)
{   
    // Get information about global and local workgroup sizes and ids
    int globalid = get_global_id(0);
    int globalsize = get_global_size(0);
    int localid = get_local_id(0);
    int localsize = get_local_size(0);

    // Add 4 consecutive locations from int array a and put result in array e
    e[globalid]=a[4*globalid]+a[4*globalid+1]+a[4*globalid+2]+a[4*globalid+3];

    // Use printf to print the information, work items and results
    // Disabled autoPipelining for entire kernel using compiler flag enableAutoPipelining=false
    // as pipeline is not feasible with printf statement in kernel. 
    // This will allow compiler to generate design quickly as it will not attempt to pipeline.
    printf("** Printing from inside krnl_vadd: globalid: %d of \
            global_work_size<%d,1,1> localid: %d of local_workgroup_size<%d,1,1>", 
            globalid,globalsize, localid,localsize); 
            
    printf(" - adding %d+%d+%d+%d to result %d **\n", 
    a[4*globalid],a[4*globalid+1],a[4*globalid+2],a[4*globalid+3], e[globalid]);
}

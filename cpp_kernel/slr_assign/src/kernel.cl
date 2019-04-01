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

#define BUFFER_SIZE 256
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void vmult(global const int* A,
                global const int* B,
                global int* C,
                const int n_elements)
{
    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];
    for (int i = 0 ; i < n_elements ; i += BUFFER_SIZE)
    {
        int size = BUFFER_SIZE;
        if (i + size > n_elements) size = n_elements - i;

        __attribute__((xcl_pipeline_loop(1)))
        readA: for (int j = 0 ; j < size ; j++) arrayA[j] = A[i+j];

        __attribute__((xcl_pipeline_loop(1)))
        readB: for (int j = 0 ; j < size ; j++) arrayB[j] = B[i+j];

        __attribute__((xcl_pipeline_loop(1)))
        vmult_writeC: for (int j = 0 ; j < size ; j++) C[i+j] = arrayA[j] * arrayB[j];
    }
}

kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void vadd(global const int* D,
                global const int* E,
                global int* F,
                const int n_elements)
{
    int arrayD[BUFFER_SIZE];
    int arrayE[BUFFER_SIZE];
    for (int i = 0 ; i < n_elements ; i += BUFFER_SIZE)
    {
        int size = BUFFER_SIZE;
        if (i + size > n_elements) size = n_elements - i;

        __attribute__((xcl_pipeline_loop(1)))
        readD: for (int j = 0 ; j < size ; j++) arrayD[j] = D[i+j];

        __attribute__((xcl_pipeline_loop(1)))
        readE: for (int j = 0 ; j < size ; j++) arrayE[j] = E[i+j];

        __attribute__((xcl_pipeline_loop(1)))
        vadd_writeF: for (int j = 0 ; j < size ; j++) F[i+j] = arrayD[j] + arrayE[j];
    }
}

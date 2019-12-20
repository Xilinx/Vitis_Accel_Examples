/**********
Copyright (c) 2019, Xilinx, Inc.
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
#ifndef __KRNL_MMULT__
#define __KRNL_MMULT__
#define DWIDTH 32
#define MAT_DIM 8
//typedef ap_axis<DWIDTH, 0, 0, 0> pkt;
typedef int pkt;
//Template to avoid signature conflict in sw_em
template <int DUMMY=0>
void mm2s(int *a,
          hls::stream<pkt> &strm_a,
          hls::stream<int> &strm_ctrl_trans1,
          hls::stream<int> &strm_ctrl_trans2) {

    int dim = strm_ctrl_trans1.read();
    int size = dim * dim;

mm2s:
    for (int i = 0; i < size; i++) {
       #pragma HLS PIPELINE II=1
        pkt p1;
        p1 = a[i];

        strm_a.write(p1);
    }
    strm_ctrl_trans2.write(dim);
}

//Template to avoid signature conflict in sw_em
template <int DUMMY=0>
void mmult(hls::stream<pkt> &strm_a,
           int *b,
           hls::stream<int> &strm_ctrl_trans2,
           hls::stream<pkt> &strm_out,
           hls::stream<int> &strm_ctrl_trans3) {

    int dim = strm_ctrl_trans2.read();
    int size = dim * dim;

    int buf_a[MAT_DIM][MAT_DIM];
    int buf_b[MAT_DIM][MAT_DIM];
    int buf_out[MAT_DIM][MAT_DIM];
    int temp_sum[MAT_DIM];
    int i, j, itr;

read_strm_in1:
    for (itr = 0, i = 0, j = 0; itr < size; itr++, j++) {
       #pragma HLS PIPELINE II=1
        if (j == dim) {
            j = 0;
            i++;
        }
        pkt temp = strm_a.read();
        buf_a[i][j] = temp;
    }

read_mm_in2:
    for (itr = 0, i = 0, j = 0; itr < size; itr++, j++) {
       #pragma HLS PIPELINE II=1
        if (j == dim) {
            j = 0;
            i++;
        }
        buf_b[i][j] = b[i * dim + j];
    }

mmult_strm_1:
    for (int row = 0; row < dim; row++) {
    mmult_strm_2:
        for (int col = 0; col < dim; col++) {
           #pragma HLS PIPELINE II=1
            int result = 0;
        mmult_strm_3:
            for (int l = 0; l < dim; l++) {
                result += buf_a[row][l] * buf_b[l][col];
            }
            buf_out[row][col] = result;
        }
    }

write_strm_out:
    for (itr = 0, i = 0, j = 0; itr < size; itr++, j++) {
       #pragma HLS PIPELINE II=1
        if (j == dim) {
            j = 0;
            i++;
        }
        pkt temp;
        temp = buf_out[i][j];

        strm_out.write(temp);
    }
    strm_ctrl_trans3.write(dim);
}

//Template to avoid signature conflict in sw_em
template <int DUMMY=0>
void s2mm(hls::stream<pkt> &strm_in,
          int *output,
          hls::stream<int> &strm_ctrl_trans5) {

    int dim = strm_ctrl_trans5.read();
    int size = dim * dim;

write_output:
    for (int i = 0; i < size; i++) {
       #pragma HLS PIPELINE II=1
        pkt temp = strm_in.read();
        output[i] = temp;
    }
}

#endif

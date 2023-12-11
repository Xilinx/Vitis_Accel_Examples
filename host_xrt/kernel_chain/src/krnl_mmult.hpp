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
#ifndef __KRNL_MMULT__
#define __KRNL_MMULT__
#define DWIDTH 32
#define MAT_DIM 32
// typedef ap_axis<DWIDTH, 0, 0, 0> pkt;
typedef int pkt;
// Template to avoid signature conflict in sw_em
template <int DUMMY = 0>
void mm2s(int* a, hls::stream<pkt>& strm_a, hls::stream<int>& strm_ctrl_trans1, hls::stream<int>& strm_ctrl_trans2) {
    int dim = strm_ctrl_trans1.read();
    strm_ctrl_trans2.write(dim);
    int size = dim * dim;

// Auto-pipeline is going to apply pipeline to this loop
mm2s:
    for (int i = 0; i < size; i++) {
        pkt p1;
        p1 = a[i];

        strm_a.write(p1);
    }
}

// Template to avoid signature conflict in sw_em
template <int DUMMY = 0>
void mmult(hls::stream<pkt>& strm_a,
           int* b,
           hls::stream<int>& strm_ctrl_trans2,
           hls::stream<pkt>& strm_out,
           hls::stream<int>& strm_ctrl_trans3) {
    int dim = strm_ctrl_trans2.read();
    strm_ctrl_trans3.write(dim);
    int size = dim * dim;

    int buf_a[MAT_DIM][MAT_DIM];
    int buf_b[MAT_DIM][MAT_DIM];
    int buf_out[MAT_DIM][MAT_DIM];
    int temp_sum[MAT_DIM];
    int i, j, itr;

// Auto-pipeline is going to apply pipeline to these loops
read_strm_in1:
    for (itr = 0, i = 0, j = 0; itr < size; itr++, j++) {
        if (j == dim) {
            j = 0;
            i++;
        }
        pkt temp = strm_a.read();
        buf_a[i][j] = temp;
    }

read_mm_in2:
    for (itr = 0, i = 0, j = 0; itr < size; itr++, j++) {
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
            int result = 0;
        mmult_strm_3:
            for (int l = 0; l < dim; l++) {
// As the outer loop is not a perfect loop
#pragma HLS loop_flatten off
                result += buf_a[row][l] * buf_b[l][col];
            }
            buf_out[row][col] = result;
        }
    }

write_strm_out:
    for (itr = 0, i = 0, j = 0; itr < size; itr++, j++) {
        if (j == dim) {
            j = 0;
            i++;
        }
        pkt temp;
        temp = buf_out[i][j];

        strm_out.write(temp);
    }
}

// Template to avoid signature conflict in sw_em
template <int DUMMY = 0>
void s2mm(hls::stream<pkt>& strm_in, int* output, hls::stream<int>& strm_ctrl_trans5) {
    int dim = strm_ctrl_trans5.read();
    int size = dim * dim;

write_output:
    for (int i = 0; i < size; i++) {
        pkt temp = strm_in.read();
        output[i] = temp;
    }
}

#endif

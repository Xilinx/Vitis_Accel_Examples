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

#ifndef _POC_AML_EDIT_DISTANCE_HPP_
#define _POC_AML_EDIT_DISTANCE_HPP_

#include "ap_int.h"
#include "hls_stream.h"

#ifndef __SYNTHESIS__
#include <iostream>

template <int N, int BITS>
void print(ap_uint<BITS> nw[N], ap_uint<BITS> n[N], ap_uint<BITS> w[N]) {
    std::cout << "x=";
    for (int i = 0; i <= N - 1; i++) {
        std::cout << nw[i] << " ";
    }
    std::cout << ", n=";
    for (int i = 0; i <= N - 1; i++) {
        std::cout << n[i] << " ";
    }
    std::cout << ", w=";
    for (int i = 0; i <= N - 1; i++) {
        std::cout << w[i] << " ";
    }
    std::cout << std::endl;
}

template <int N, int BITS>
void print2(ap_uint<BITS> nw[N], ap_uint<BITS> n[N], ap_uint<BITS> w[N]) {
    for (int i = N - 1; i >= 0; i--) {
        std::cout << nw[i] << " " << n[i] << " " << w[i] << "\t";
    }
    std::cout << std::endl;
}

template <int N>
void print_string(char t[N]) {
    for (int i = 0; i < N; i++) {
        // char a = t(8 * (N - i - 1) + 7, 8 * (N - i - 1));
        char a = t[N - i - 1];
        std::cout << a;
    }
    std::cout << std::endl;
}
#endif

template <int N, int BITS>
void compute_ed(char pattern_string[N], // must be shifted
                char input_string[N],

                ap_uint<BITS> northwest[N],
                ap_uint<BITS> north[N],
                ap_uint<BITS> west[N]) {
#pragma HLS inline

    // const unsigned short upper = (offset < N) ? (N - 1) : (2 * (N - 1) - offset);
    // const unsigned short lower = (offset < N) ? (N - offset - 1) : 0;
    for (int i = N - 1; i >= 0; i--) { // Remove the dependence
#pragma HLS unroll
        const char ichar = input_string[i];
        const char pchar = pattern_string[i];

        const ap_uint<BITS> nw_value =
            (pchar != ichar && (northwest[i] <= (ap_uint<BITS>)-2)) ? (ap_uint<BITS>)(northwest[i] + 1) : northwest[i];
        const ap_uint<BITS> n_value = (north[i] > (ap_uint<BITS>)-2) ? north[i] : (ap_uint<BITS>)(north[i] + 1);
        const ap_uint<BITS> w_value = (west[i] > (ap_uint<BITS>)-2) ? west[i] : (ap_uint<BITS>)(west[i] + 1);
        // const ap_uint<BITS> nw_value = (pchar != ichar) ? (ap_uint<BITS>)(northwest[i] + 1) : northwest[i];
        // const ap_uint<BITS> n_value = (ap_uint<BITS>)(north[i] + 1);
        // const ap_uint<BITS> w_value = (ap_uint<BITS>)(west[i] + 1);

        if (nw_value <= n_value && nw_value <= w_value) { // north west
            northwest[i] = north[i];
            west[i] = nw_value;
            north[i] = nw_value;
        } else if (n_value <= w_value) { // north
            northwest[i] = north[i];
            west[i] = n_value;
            north[i] = n_value;
        } else { // west
            northwest[i] = north[i];
            west[i] = w_value;
            north[i] = w_value;
        }
    }
}

template <int N>
void char_shift(unsigned short offset, char in_char, char str[N]) {
#pragma HLS inline
    // const unsigned short lower = (offset < N) ? (N - offset - 1) : 0;

    for (int i = 1; i < N; i++) {
#pragma HLS unroll
        str[i - 1] = str[i];
    }
    str[N - 1] = in_char;
}

template <int N, int BITS>
void left_shift(unsigned short offset, ap_uint<BITS> northwest[N], ap_uint<BITS> west[N]) {
#pragma HLS inline
    // ap_uint<8> nw0 = (offset > N - 1) ? (ap_uint<BITS>)(offset + 1) : northwest[0];
    // ap_uint<8> w0 = (offset > N - 2) ? (ap_uint<BITS>)(offset + 2) : west[0];
    ap_uint<8> nw0 = northwest[0];
    ap_uint<8> w0 = west[0];
    for (int i = 1; i < N; i++) {
#pragma HLS unroll
        northwest[i - 1] = northwest[i];
        west[i - 1] = west[i];
    }
    northwest[N - 1] = nw0;
    west[N - 1] = w0;
}

/**
 * @brief, Levenshtein distance implementation of two given strings
 *
 * @tparam N maximum length of query string
 * @tparam M maximum length of input stream string, N must be less than M
 * @tparam BITS data width of internal edit distance in bits
 *
 * @param query_strm
 * @param input_strm
 *
 */
template <int N, int M, int BITS>
void editDistance(hls::stream<ap_uint<BITS> >& len1_strm,
                  hls::stream<ap_uint<64> >& query_strm,
                  hls::stream<ap_uint<BITS> >& len2_strm,
                  hls::stream<ap_uint<64> >& input_strm,
                  hls::stream<ap_uint<BITS> >& max_ed_strm,
                  hls::stream<bool>& i_e_strm,

                  hls::stream<bool>& o_e_strm,
                  hls::stream<bool>& o_match_strm) {
    ap_uint<BITS> north[N];
#pragma HLS ARRAY_PARTITION variable = north complete dim = 1
    ap_uint<BITS> west[N];
#pragma HLS ARRAY_PARTITION variable = west complete dim = 1
    ap_uint<BITS> northwest[N];
#pragma HLS ARRAY_PARTITION variable = northwest complete dim = 1

    char str1[N], str3[N];
#pragma HLS ARRAY_PARTITION variable = str1 complete dim = 1
#pragma HLS ARRAY_PARTITION variable = str3 complete dim = 1
    char str2[M];
#pragma HLS ARRAY_PARTITION variable = str2 complete dim = 1

    const int fold_num = (8 * M + 63) / 64;

    const ap_uint<BITS> len1 = len1_strm.read();
    for (int i = 0; i < fold_num; i++) {
        ap_uint<64> str1_part = query_strm.read();
        for (int j = 0; j < 8; j++) {
#pragma HLS unroll
            if (i * 8 + j < N) str1[i * 8 + j] = str1_part(j * 8 + 7, j * 8);
        }
    }

    bool last = i_e_strm.read();
    while (!last) {
        northwest[N - 1] = 0;
        north[N - 1] = 1;
        west[N - 1] = 1;
        for (int i = 0; i < N - 1; i++) {
#pragma HLS unroll
            northwest[i] = (ap_uint<BITS>)-1;
            north[i] = (i == 0) ? (ap_uint<BITS>)1 : (ap_uint<BITS>)-1;
            west[i] = (i == N - 2) ? (ap_uint<BITS>)1 : (ap_uint<BITS>)-1;
        }
        // print<N>(northwest, north, west);
        for (int i = 0; i < N; i++) {
#pragma HLS unroll
            str3[i] = 255;
        }

        const ap_uint<BITS> len2 = len2_strm.read();
        ap_uint<BITS> med = max_ed_strm.read();
        for (int i = 0; i < fold_num; i++) {
            ap_uint<64> str2_part = input_strm.read();
            for (int j = 0; j < 8; j++) {
#pragma HLS unroll
                if (i * 8 + j < M) str2[i * 8 + j] = str2_part(j * 8 + 7, j * 8);
            }
        }

    ED_CORE_LOOP:
        for (unsigned short i = 0; i < len1 + len2 - 1; i++) {
#pragma HLS pipeline
            char_shift<N>(i, str2[M - 1 - i], str3);
            compute_ed<N, BITS>(str3, str1, northwest, north, west);
            left_shift<N, BITS>(i, northwest, west);
            // print2<N, BITS>(northwest, north, west);
        }

        // ap_uint<BITS> ed = (len2 <= len1) ? north[0] : north[N - len1]; // edit distance is at the last element
        ap_uint<BITS> ed = north[N - len1]; // edit distance is at the last element
#ifndef __SYNTHESIS__
// if (ed <= med) {
//    std::cout << "ed:" << ed << ", med:" << med << std::endl;
//    print_string<N>(str1);
//    print_string<M>(str2);
//}
#endif
        o_match_strm.write(ed <= med);
        o_e_strm.write(false);

        last = i_e_strm.read();
    }

    o_e_strm.write(true);
}

#endif

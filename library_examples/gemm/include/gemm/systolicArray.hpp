/**********
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * **********/
#ifndef XF_BLAS_SYSTOLICARRAY_HPP
#define XF_BLAS_SYSTOLICARRAY_HPP

#include <cassert>
#include "../helpers/utils/utils.hpp"

namespace xf {

namespace blas {

template <typename t_DataType,
          unsigned int t_KBufferDim,
          unsigned int t_ParEntriesM,
          unsigned int t_ParEntriesN = t_ParEntriesM,
          typename t_MacDataType = t_DataType>
class SystolicArray {
    typedef WideType<t_DataType, t_ParEntriesM> WideTypeM;
    typedef WideType<t_DataType, t_ParEntriesN> WideTypeN;
    typedef WideType<t_MacDataType, t_ParEntriesN> WideMacTypeN;

    typedef hls::stream<typename WideTypeM::t_TypeInt> EdgeStreamM;
    typedef hls::stream<typename WideTypeN::t_TypeInt> EdgeStreamN;
    typedef hls::stream<typename WideMacTypeN::t_TypeInt> EdgeMacStreamN;

    typedef TaggedWideType<t_DataType, t_ParEntriesM> TaggedWideTypeM;
    typedef TaggedWideType<t_DataType, t_ParEntriesN> TaggedWideTypeN;
    typedef hls::stream<typename TaggedWideTypeM::t_TypeInt> TaggedWideStreamM;
    typedef hls::stream<typename TaggedWideTypeN::t_TypeInt> TaggedWideStreamN;

    typedef TaggedFloat<t_MacDataType> TaggedMacType;

    typedef TaggedDualWideType<t_DataType, t_ParEntriesN> TaggedDualType;
    typedef hls::stream<typename TaggedDualType::t_TypeInt> TaggedDualTypeStream;

    typedef TaggedFloat<t_DataType> TaggedDataType;
    typedef typename TaggedDataType::t_TypeInt TaggedTypeInt;

    static const unsigned int t_FloatDelay = AdderDelay<t_DataType>::m_Delays;

   public:
    SystolicArray(unsigned int p_blocks = 0) : m_blocks(p_blocks) {}

    void process(EdgeStreamM& p_As, EdgeStreamN& p_Bs, EdgeMacStreamN& p_sum) {
        TaggedDualTypeStream p_Cs[t_ParEntriesM];
#pragma HLS ARRAY_PARTITION variable = p_Cs complete dim = 1
        EdgeMacStreamN l_sum[t_ParEntriesM];
#pragma HLS ARRAY_PARTITION variable = l_sum complete dim = 1
        TaggedWideStreamM l_tagA;
        EdgeStreamN l_tagB;
#pragma HLS DATAFLOW
        tagAB(p_As, p_Bs, l_tagA, l_tagB);
        systolicArray(l_tagA, l_tagB, p_Cs);

        for (unsigned int col = 0; col < t_ParEntriesN; ++col) {
#pragma HLS UNROLL
            macs(p_Cs[col], l_sum[col]);
        }
        mergeSum(l_sum, p_sum);
    }

    void mergeSum(EdgeMacStreamN p_sums[t_ParEntriesM], EdgeMacStreamN& p_sum) {
        for (unsigned int i = 0; i < m_blocks; i++) {
            for (int j = 0; j < t_ParEntriesM; j++) {
#pragma HLS PIPELINE
                p_sum.write(p_sums[j].read());
            }
        }
    }

    void muls(TaggedDualTypeStream& p_Cs, EdgeMacStreamN& p_mul) {
        ap_shift_reg<bool, t_ParEntriesN> l_flush;
        for (int i = 0; i < t_ParEntriesN; i++)
#pragma HLS PIPELINE
            l_flush.shift(false);

        TriangSrl<t_MacDataType, t_ParEntriesN> l_Tc;
        l_Tc.clear();

        uint32_t l_outCount = 0;
        bool l_exit;

        do {
#pragma HLS PIPELINE
            TaggedDualType l_c = p_Cs.read();

            WideMacTypeN l_mul;
            WideMacTypeN l_cVec;
            WideMacTypeN l_mVec;

            bool l_iflush = l_c.m_flush;
            l_exit = l_c.m_exit;

            bool l_oflush = l_flush.shift(l_iflush);
            if (l_oflush) {
                l_outCount++;
            }

            for (int i = 0; i < t_ParEntriesN; i++) {
                l_cVec[t_ParEntriesN - i - 1] = l_c.m_a[i] * l_c.m_b[i];
            }

            l_mVec = l_Tc.shift(l_cVec);

            for (int i = 0; i < t_ParEntriesN; i++) {
                l_mul[i] = l_mVec[t_ParEntriesN - i - 1];
            }

            if (l_outCount > 0 && l_outCount <= m_blocks) {
                p_mul.write(l_mul);
            }

        } while (!l_exit);
    }

    void adds(EdgeMacStreamN& p_mul, EdgeMacStreamN& p_sum) {
        WideType<t_MacDataType, t_ParEntriesN> l_sum;
        constexpr int l_kIter = t_KBufferDim / t_FloatDelay;

        for (unsigned int n = 0; n < m_blocks; n++) {
            for (int k = 0; k < l_kIter; k++) {
#pragma HLS PIPELINE II = t_FloatDelay
                if (k == 0) l_sum = 0;

                t_MacDataType l_pSum[t_ParEntriesN];
#pragma HLS ARRAY_PARTITION variable = l_pSum complete dim = 1
                WideMacTypeN l_mul = p_mul.read();

                for (int i = 0; i < t_ParEntriesN; i++) {
                    l_pSum[i] = l_mul[i];
                }

                for (int d = 1; d < t_FloatDelay; d++) {
                    WideMacTypeN l_mul = p_mul.read();
                    for (int i = 0; i < t_ParEntriesN; i++) {
                        l_pSum[i] += l_mul[i];
                    }
                }

                for (int i = 0; i < t_ParEntriesN; i++) {
                    l_sum[i] += l_pSum[i];
                }

                if (k == l_kIter - 1) {
                    p_sum.write(l_sum);
                }
            }
        }
    }

    void macs(TaggedDualTypeStream& p_Cs, EdgeMacStreamN& p_sum) {
#pragma HLS DATAFLOW
        EdgeMacStreamN l_mul;
        muls(p_Cs, l_mul);
        adds(l_mul, p_sum);
    }

    void systolicArray(TaggedWideStreamM& p_As, EdgeStreamN& p_Bs, TaggedDualTypeStream p_Cs[t_ParEntriesM]) {
        TriangSrl<t_DataType, t_ParEntriesM> l_Ta;
        TriangSrl<t_DataType, t_ParEntriesN> l_Tb;
        WideType<bool, t_ParEntriesN + 1> l_Tf = 0;

        WindowRm<t_DataType, t_ParEntriesM, t_ParEntriesN> l_awin;
        WindowRm<t_DataType, t_ParEntriesM, t_ParEntriesN> l_bwin;
        l_awin.clear();
        l_bwin.clear();
        l_Ta.clear();
        l_Tb.clear();

        bool l_exit;
        do {
#pragma HLS PIPELINE

            TaggedWideTypeM l_A = p_As.read();
            WideTypeN l_B = p_Bs.read();
            l_exit = l_A.getExit();

            WideTypeM l_avec1 = l_Ta.shift(l_A.getVal());
            WideTypeN l_bvec1 = l_Tb.shift(l_B);
            l_Tf.shift(l_A.getFlush());

            l_awin.shift_right(l_avec1);
            l_bwin.shift(l_bvec1);

            for (unsigned int row = 0; row < t_ParEntriesM; ++row) {
#pragma HLS UNROLL
                TaggedDualType l_C;
                l_C.m_a = l_awin[row];
                l_C.m_b = l_bwin[row];
                l_C.m_flush = l_Tf[row + 1];
                l_C.m_exit = l_exit;
                p_Cs[row].write(l_C);
            }
        } while (!l_exit);
    }

    void tagAB(EdgeStreamM& p_a, EdgeStreamN& p_b, TaggedWideStreamM& l_a, EdgeStreamN& l_b) {
        for (unsigned int i = 0; i < m_blocks; i++)
            for (int k = 0; k < t_KBufferDim; k++) {
#pragma HLS PIPELINE
                WideTypeM l_valA = p_a.read();
                typename WideTypeN::t_TypeInt l_valB = p_b.read();
                bool l_flush = (k == 0);
                bool l_exit = false;
                TaggedWideTypeM l_taggedValA(l_valA, l_flush, l_exit);
                l_a.write(l_taggedValA);
                l_b.write(l_valB);
            }
        const unsigned int l_flushLen = 1 + t_ParEntriesM + t_ParEntriesN;
        for (unsigned i = 0; i < l_flushLen; i++) {
#pragma HLS PIPELINE
            bool l_exit = (i == l_flushLen - 1);
            bool l_flush = (i == 0);
            TaggedWideTypeM l_taggedValA(0, l_flush, l_exit);
            typename WideTypeN::t_TypeInt l_valB = 0;
            l_a.write(l_taggedValA);
            l_b.write(l_valB);
        }
    }

   private:
    unsigned int m_blocks;
};
}
}

#endif

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
/**
 * @file rng.hpp
 * @brief This files includes normal random number generator and uniform random
 * number generator
 *
 */
#ifndef XF_FINTECH_PRNG_HPP
#define XF_FINTECH_PRNG_HPP

#include "ap_fixed.h"
#include "ap_int.h"
#include "hls_math.h"
#include "utils.hpp"
namespace xf {
namespace fintech {
namespace internal {

// This version of cumulative normal distribution function assume its output
// range is within [2^(-32), 1 - 2^(-32)];
// It's a specified version works with MT19937 since its output uniform
// distributed value is [2^(-32), 1 - 2^(-32)];
template <typename mType>
mType CumulativeNormal(mType input) {
    const mType a[9] = {2.2352520354606839287,
                        161.02823106855587881,
                        1067.6894854603709582,
                        18154.981253343561249,
                        0.065682337918207449113,
                        0,
                        0,
                        0,
                        0};
    const mType b[8] = {
        47.20258190468824187, 976.09855173777669322, 10260.932208618978205, 45507.789335026729956, 0, 0, 0, 0};
    const mType c[9] = {0.39894151208813466764, 8.8831497943883759412, 93.506656132177855979,
                        597.27027639480026226,  2494.5375852903726711, 6848.1904505362823326,
                        11602.651437647350124,  9842.7148383839780218, 1.0765576773720192317e-8};
    const mType d[8] = {22.266688044328115691, 235.38790178262499861, 1519.377599407554805,  6485.558298266760755,
                        18615.571640885098091, 34900.952721145977266, 38912.003286093271411, 19685.429676859990727};
#pragma HLS array_partition variable = a dim = 1 complete
#pragma HLS array_partition variable = b dim = 1 complete
#pragma HLS array_partition variable = c dim = 1 complete
#pragma HLS array_partition variable = d dim = 1 complete

    const mType boundary = 0.67448975;
    const mType eps = 1.11022302462515655e-16;

    mType x, xsq, xnum, xden, temp, del, x_poly;

    if (input > 0) {
        x = input;
    } else {
        x = -input;
    }

    bool within_boundary = x < boundary;

    mType para_num[9];
    mType para_den[8];
#pragma HLS array_partition variable = para_num dim = 1 complete
#pragma HLS array_partition variable = para_den dim = 1 complete

    for (int i = 0; i < 9; i++) {
        if (within_boundary) {
            para_num[i] = a[i];
        } else {
            para_num[i] = c[i];
        }
    }

    for (int i = 0; i < 8; i++) {
        if (within_boundary) {
            para_den[i] = b[i];
        } else {
            para_den[i] = d[i];
        }
    }

    mType x_num_add[8];
    mType x_num_mul[8];
    mType x_den_add[8];
    mType x_den_mul[8];
#pragma HLS array_partition variable = x_num_add dim = 1 complete
#pragma HLS array_partition variable = x_num_mul dim = 1 complete
#pragma HLS array_partition variable = x_den_add dim = 1 complete
#pragma HLS array_partition variable = x_den_mul dim = 1 complete

#ifdef DPRAGMA
#pragma HLS resource variable = xsq core = DMul_meddsp
#else
#pragma HLS resource variable = xsq core = FMul_fulldsp
#endif
    xsq = x * x;

    mType para_num_tmp;
    if (within_boundary) {
        para_num_tmp = para_num[4];
        x_poly = xsq;
    } else {
        para_num_tmp = para_num[8];
        x_poly = x;
    }

    mType tmp_x_num_mul_0;
#ifdef DPRAGMA
#pragma HLS resource variable = tmp_x_num_mul_0 core = DMul_meddsp
#else
#pragma HLS resource variable = tmp_x_num_mul_0 core = FMul_fulldsp
#endif
    tmp_x_num_mul_0 = para_num_tmp * x_poly;
    x_num_mul[0] = tmp_x_num_mul_0;
    x_den_mul[0] = x_poly;

    for (int i = 1; i <= 7; i++) {
        mType local_x_num_add, local_x_num_mul, local_x_den_add, local_x_den_mul;
#ifdef DPRAGMA
#pragma HLS resource variable = local_x_num_add core = DAddSub_nodsp
#else
#pragma HLS resource variable = local_x_num_add core = FAddSub_nodsp
#endif
        local_x_num_add = x_num_mul[i - 1] + para_num[i - 1];
        x_num_add[i] = local_x_num_add;
#ifdef DPRAGMA
#pragma HLS resource variable = local_x_num_mul core = DMul_meddsp
#else
#pragma HLS resource variable = local_x_num_mul core = FMul_fulldsp
#endif
        local_x_num_mul = x_num_add[i] * x_poly;
        x_num_mul[i] = local_x_num_mul;
#ifdef DPRAGMA
#pragma HLS resource variable = local_x_den_add core = DAddSub_nodsp
#else
#pragma HLS resource variable = local_x_den_add core = FAddSub_nodsp
#endif
        local_x_den_add = x_den_mul[i - 1] + para_den[i - 1];
        x_den_add[i] = local_x_den_add;
#ifdef DPRAGMA
#pragma HLS resource variable = local_x_den_mul core = DMul_meddsp
#else
#pragma HLS resource variable = local_x_den_mul core = FMul_fulldsp
#endif
        local_x_den_mul = x_den_add[i] * x_poly;
        x_den_mul[i] = local_x_den_mul;
    }

    if (x < eps) {
        x_num_mul[3] = 0;
        x_den_mul[3] = 0;
    }

    mType add_para_num_tmp, add_para_den_tmp;
    mType add_x_num_tmp, add_x_den_tmp;
    if (within_boundary) {
        add_x_num_tmp = x_num_mul[3];
        add_x_den_tmp = x_den_mul[3];
        add_para_num_tmp = para_num[3];
        add_para_den_tmp = para_den[3];
    } else {
        add_x_num_tmp = x_num_mul[7];
        add_x_den_tmp = x_den_mul[7];
        add_para_num_tmp = para_num[7];
        add_para_den_tmp = para_den[7];
    }

#ifdef DPRAGMA
#pragma HLS resource variable = xnum core = DAddSub_nodsp
#else
#pragma HLS resource variable = xnum core = FAddSub_nodsp
#endif
    xnum = add_x_num_tmp + add_para_num_tmp;
#ifdef DPRAGMA
#pragma HLS resource variable = xden core = DAddSub_nodsp
#else
#pragma HLS resource variable = xden core = FAddSub_nodsp
#endif
    xden = add_x_den_tmp + add_para_den_tmp;

    temp = xnum / xden;
    mType tmp_temp_mul;
    if (within_boundary) {
        tmp_temp_mul = x;
    } else {
        tmp_temp_mul = FPExp(-xsq / 2);
    }
#ifdef DPRAGMA
#pragma HLS resource variable = temp core = DMul_meddsp
#else
#pragma HLS resource variable = temp core = FMul_fulldsp
#endif
    temp *= tmp_temp_mul;

    mType result, res_1, res_2;

    if (within_boundary) {
        res_1 = 0.5;
        if (input > 0) {
            res_2 = temp;
        } else {
            res_2 = -temp;
        }
    } else {
        if (input > 0) {
            res_1 = 1;
            res_2 = -temp;
        } else {
            res_1 = 0;
            res_2 = temp;
        }
    }
#ifdef DPRAGMA
#pragma HLS resource variable = result core = DAddSub_nodsp
#else
#pragma HLS resource variable = result core = FAddSub_nodsp
#endif
    result = res_1 + res_2;

    return result;
}
} // namespace internal

/**
 * @brief Inverse Cumulative transform from random number to normal random number.
 *
 * Reference: Algorithm AS 241, The Percentage Points of the Normal Distribution
 * by  Michael J. Wichura.
 *
 * @tparam mType data type.
 * @param input random number input.
 * @return normal random number.
 */
template <typename mType>
mType inverseCumulativeNormalPPND7(mType input) {
    const mType a0 = 3.3871327179e+00;
    const mType a1 = 5.0434271938e+01;
    const mType a2 = 1.5929113202e+02;
    const mType a3 = 5.9109374720e+01;

    const mType b1 = 1.7895169469e+01;
    const mType b2 = 7.8757757664e+01;
    const mType b3 = 6.7187563600e+01;

    const mType c0 = 1.4234372777e+00;
    const mType c1 = 2.7568153900e+00;
    const mType c2 = 1.3067284816e+00;
    const mType c3 = 1.7023821103e-01;

    const mType d1 = 7.3700164250e-01;
    const mType d2 = 1.2021132975e-01;

    const mType e0 = 6.6579051150e+00;
    const mType e1 = 3.0812263860e+00;
    const mType e2 = 4.2868294337e-01;
    const mType e3 = 1.7337203997e-02;

    const mType f1 = 2.4197894225e-01;
    const mType f2 = 1.2258202635e-02;

    const mType x_low = 0.075e+00;
    const mType x_high = 0.925e+00;
    const mType split = 5.0e+00;
    const mType const1 = 0.180625e+00;
    const mType const2 = 1.6e+00;
    const mType half = 0.5e+00;
    const mType zero = 0;
    const mType one = 1;

    mType tmp, z, r, standard_value;
    mType p0, p1, p2, p3, q1, q2, q3, frac_a, frac_b;
    mType fa1, fa2, fa3, fa4, fa5, fa6, fa7;
    mType fb1, fb2, fb3, fb4, fb5, fb6;
    ap_uint<1> is_center, is_lower_tail, is_less_than_split;

    if (input < x_low || input > x_high) {
        is_center = 0;
        if (input < x_low) {
            z = input;
            is_lower_tail = 1;
        } else {
            z = internal::FPTwoSub(one, input);
            is_lower_tail = 0;
        }
        tmp = hls::sqrt(-hls::log(z));
        if (tmp < split) {
            is_less_than_split = 1;
            r = internal::FPTwoSub(tmp, const2);
            p0 = c0;
            p1 = c1;
            p2 = c2;
            p3 = c3;
            q1 = one;
            q2 = d1;
            q3 = d2;
        } else {
            is_less_than_split = 0;
            r = internal::FPTwoSub(tmp, split);
            p0 = e0;
            p1 = e1;
            p2 = e2;
            p3 = e3;
            q1 = one;
            q2 = f1;
            q3 = f2;
        }
    } else {
        is_center = 1;
        p0 = a0;
        p1 = a1;
        p2 = a2;
        p3 = a3;
        q1 = b1;
        q2 = b2;
        q3 = b3;
        z = internal::FPTwoSub(input, half);
        mType z_sq = internal::FPTwoMul(z, z);
        r = internal::FPTwoSub(const1, z_sq);
    }

    fa1 = p3 * r;
#pragma HLS resource variable = fa2 core = FAddSub_nodsp
    fa2 = fa1 + p2;
    fa3 = fa2 * r;
#pragma HLS resource variable = fa4 core = FAddSub_nodsp
    fa4 = fa3 + p1;
    fa5 = fa4 * r;
#pragma HLS resource variable = fa6 core = FAddSub_nodsp
    fa6 = fa5 + p0;

    fb1 = q3 * r;
#pragma HLS resource variable = fb2 core = FAddSub_nodsp
    fb2 = fb1 + q2;
    fb3 = fb2 * r;
#pragma HLS resource variable = fb4 core = FAddSub_nodsp
    fb4 = fb3 + q1;

    if (is_center) {
        fa7 = fa6 * z;
        fb5 = fb4 * r;
#pragma HLS resource variable = fb6 core = FAddSub_nodsp
        fb6 = fb5 + one;
    } else {
        fa7 = fa6;
        fb6 = fb4;
    }
    frac_a = fa7;
    frac_b = fb6;

    standard_value = frac_a / frac_b;
    if ((!is_center) && (is_lower_tail)) {
        standard_value = -standard_value;
    }

    return standard_value;
}

/**
 * @brief Inverse CumulativeNormal using Acklam's approximation to transform
 * uniform random number to normal random number.
 *
 * Reference: Acklam's approximation: by Peter J. Acklam, University of Oslo,
 * Statistics Division.
 *
 * @tparam mType data type.
 * @param input input uniform random number
 * @return normal random number
 */

template <typename mType>
mType inverseCumulativeNormalAcklam(mType input) {
    const mType a1 = -3.969683028665376e+01;
    const mType a2 = 2.209460984245205e+02;
    const mType a3 = -2.759285104469687e+02;
    const mType a4 = 1.383577518672690e+02;
    const mType a5 = -3.066479806614716e+01;
    const mType a6 = 2.506628277459239e+00;
    const mType b1 = -5.447609879822406e+01;
    const mType b2 = 1.615858368580409e+02;
    const mType b3 = -1.556989798598866e+02;
    const mType b4 = 6.680131188771972e+01;
    const mType b5 = -1.328068155288572e+01;
    const mType c1 = -7.784894002430293e-03;
    const mType c2 = -3.223964580411365e-01;
    const mType c3 = -2.400758277161838e+00;
    const mType c4 = -2.549732539343734e+00;
    const mType c5 = 4.374664141464968e+00;
    const mType c6 = 2.938163982698783e+00;
    const mType d1 = 7.784695709041462e-03;
    const mType d2 = 3.224671290700398e-01;
    const mType d3 = 2.445134137142996e+00;
    const mType d4 = 3.754408661907416e+00;
    const mType x_low = 0.02425;
    const mType x_high = 1.0 - x_low;

    mType standard_value, z, r, f1, f1_1, f2, tmp;
    mType p1, p2, p3, p4, p5, p6;
    mType q1, q2, q3, q4, q5;
    ap_uint<1> not_tail;
    ap_uint<1> upper_tail;

    mType t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
    mType t11, t12, t13, t14, t15, t16, t17, t18, t19;

    if (input < x_low || x_high < input) {
        if (input < x_low) {
            tmp = input;
            upper_tail = 0;
        } else {
#pragma HLS resource variable = tmp core = DAddSub_nodsp
            tmp = mType(1.0) - input;
            upper_tail = 1;
        }
#pragma HLS resource variable = t1 core = DLog_meddsp
        t1 = hls::log(tmp);
#pragma HLS resource variable = t2 core = DMul_meddsp
        t2 = t1 * 2;
        t3 = -t2;
        z = hls::sqrt(t3);
        r = z;
        p1 = c1;
        p2 = c2;
        p3 = c3;
        p4 = c4;
        p5 = c5;
        p6 = c6;
        q1 = d1;
        q2 = d2;
        q3 = d3;
        q4 = d4;
        not_tail = 0;
    } else {
#pragma HLS resource variable = z core = DAddSub_nodsp
        z = input - mType(0.5);
#pragma HLS resource variable = r core = DMul_meddsp
        r = z * z;
        p1 = a1;
        p2 = a2;
        p3 = a3;
        p4 = a4;
        p5 = a5;
        p6 = a6;
        q1 = b1;
        q2 = b2;
        q3 = b3;
        q4 = b4;
        q5 = b5;
        not_tail = 1;
    }

#pragma HLS resource variable = t4 core = DMul_meddsp
    t4 = p1 * r;
#pragma HLS resource variable = t5 core = DAddSub_nodsp
    t5 = t4 + p2;
#pragma HLS resource variable = t6 core = DMul_meddsp
    t6 = t5 * r;
#pragma HLS resource variable = t7 core = DAddSub_nodsp
    t7 = t6 + p3;
#pragma HLS resource variable = t8 core = DMul_meddsp
    t8 = t7 * r;
#pragma HLS resource variable = t9 core = DAddSub_nodsp
    t9 = t8 + p4;
#pragma HLS resource variable = t10 core = DMul_meddsp
    t10 = t9 * r;
#pragma HLS resource variable = t11 core = DAddSub_nodsp
    t11 = t10 + p5;
#pragma HLS resource variable = t12 core = DMul_meddsp
    t12 = t11 * r;
#pragma HLS resource variable = f1_1 core = DAddSub_nodsp
    f1_1 = t12 + p6;
    if (not_tail) {
#pragma HLS resource variable = f1 core = DMul_meddsp
        f1 = f1_1 * z;
    } else {
        f1 = f1_1;
    }

#pragma HLS resource variable = t13 core = DMul_meddsp
    t13 = q1 * r;
#pragma HLS resource variable = t14 core = DAddSub_nodsp
    t14 = t13 + q2;
#pragma HLS resource variable = t15 core = DMul_meddsp
    t15 = t14 * r;
#pragma HLS resource variable = t16 core = DAddSub_nodsp
    t16 = t15 + q3;
#pragma HLS resource variable = t17 core = DMul_meddsp
    t17 = t16 * r;
#pragma HLS resource variable = f2 core = DAddSub_nodsp
    f2 = t17 + q4;
    if (not_tail) {
#pragma HLS resource variable = t18 core = DMul_meddsp
        t18 = f2 * r;
#pragma HLS resource variable = f2 core = DAddSub_nodsp
        f2 = t18 + q5;
    }
#pragma HLS resource variable = t19 core = DMul_meddsp
    t19 = f2 * r;
#pragma HLS resource variable = f2 core = DAddSub_nodsp
    f2 = t19 + 1;

    standard_value = f1 / f2;
    if ((!not_tail) && (upper_tail)) {
        standard_value = -standard_value;
    }
    return standard_value;
}

/**
 * @brief Mersenne Twister to generate uniform random number.
 *
 * Reference:Mersenne Twister: A 623-Dimensionally Equidistributed Uniform
 * Pseudo-Random Number Generator
 */
class MT19937 {
   private:
    /// Bit width of element in state vector
    static const int W = 32;
    /// Number of elements in state vector
    static const int N = 624;
    /// Bias in state vector to calculate new state element
    static const int M = 397;
    /// Bitmask for lower R bits
    static const int R = 31;
    /// First right shift length
    static const int U = 11;
    /// First left shift length
    static const int S = 7;
    /// Second left shift length
    static const int T = 15;
    /// Second right shift length
    static const int L = 18;
    /// Right shift length in seed initilization
    static const int S_W = 30;
    /// Address width of state vector
    static const int A_W = 10;
    /// Use 1024 depth array to hold a 624 state vector
    static const int N_W = 1024;

    /// Array to store state vector
    ap_uint<W> mt_odd_0[N_W / 2];
    /// Duplicate of array mt to solve limited memory port issue
    ap_uint<W> mt_odd_1[N_W / 2];
    /// Array to store state vector
    ap_uint<W> mt_even_0[N_W / 2];
    /// Duplicate of array mt to solve limited memory port issue
    ap_uint<W> mt_even_1[N_W / 2];
    /// Element 0 of state vector
    ap_uint<W> x_k_p_0;
    /// Element 1 of state vector
    ap_uint<W> x_k_p_1;
    /// Element 2 of state vector
    ap_uint<W> x_k_p_2;
    /// Element m of state vector
    ap_uint<W> x_k_p_m;
    /// Address of head of state vector in array mt/mt_1
    ap_uint<A_W> addr_head;

   public:
    /**
     * @brief initialize mt and mt_1 using seed
     *
     * @param seed initialization seed
     */
    void seedInitialization(ap_uint<W> seed) {
#pragma HLS inline off

        const ap_uint<W> start = 0xFFFFFFFF;
        const ap_uint<W> factor = 0x6C078965;
        const ap_uint<W> factor2 = 0x00010DCD;
        const ap_uint<W> factor3 = 0xffffffff - factor2;
        ap_uint<W * 2> tmp;
        ap_uint<W> mt_reg = seed;
        if (seed > factor3) {
            mt_reg = seed;
        } else {
            mt_reg = factor2 + seed;
        }
        mt_even_0[0] = mt_reg;
        mt_even_1[0] = mt_reg;
    SEED_INIT_LOOP:
        for (ap_uint<A_W> i = 1; i < N; i++) {
#pragma HLS pipeline II = 3
            tmp = factor * (mt_reg ^ (mt_reg >> S_W)) + i;
            mt_reg = tmp;
            ap_uint<A_W - 1> idx = i >> 1;
            if (i[0] == 0) {
                mt_even_0[idx] = tmp(W - 1, 0);
                mt_even_1[idx] = tmp(W - 1, 0);
            } else {
                mt_odd_0[idx] = tmp(W - 1, 0);
                mt_odd_1[idx] = tmp(W - 1, 0);
            }
        }
        x_k_p_0 = mt_even_0[0];
        x_k_p_1 = mt_odd_0[0];
        x_k_p_2 = mt_even_0[1];
        x_k_p_m = mt_odd_0[M / 2];
        addr_head = 0;
    }

    /**
     * @brief Default constructor
     */
    MT19937() {
#pragma HLS inline
        //#pragma HLS RESOURCE variable = mt_even_0 core = RAM_T2P_BRAM
        //#pragma HLS RESOURCE variable = mt_even_1 core = RAM_T2P_BRAM
        //#pragma HLS RESOURCE variable = mt_odd_0 core = RAM_T2P_BRAM
        //#pragma HLS RESOURCE variable = mt_odd_1 core = RAM_T2P_BRAM
        //#pragma HLS ARRAY_PARTITION variable=mt cyclic factor=2
    }

    /**
     * @brief Constructor with seed
     *
     * @param seed initialization seed
     */
    MT19937(ap_uint<W> seed) {
#pragma HLS inline
        //#pragma HLS RESOURCE variable = mt_even_0 core = RAM_T2P_BRAM
        //#pragma HLS RESOURCE variable = mt_even_1 core = RAM_T2P_BRAM
        //#pragma HLS RESOURCE variable = mt_odd_0 core = RAM_T2P_BRAM
        //#pragma HLS RESOURCE variable = mt_odd_1 core = RAM_T2P_BRAM
        //#pragma HLS ARRAY_PARTITION variable=mt cyclic factor=2
        seedInitialization(seed);
    }

    /**
     * @brief Setup status
     *
     * @param data array to store the initialization data
     */
    void statusSetup(ap_uint<W> data[N]) {
        for (ap_uint<A_W> i = 0; i < N; i++) {
            ap_uint<A_W - 1> idx = i >> 1;
            if (i[0] == 0) {
                mt_even_0[idx] = data[i];
                mt_even_1[idx] = data[i];
            } else {
                mt_odd_0[idx] = data[i];
                mt_odd_1[idx] = data[i];
            }
        }
        // Initialize x_k_p_0, x_k_p_1 and head address of array mt
        x_k_p_0 = mt_even_0[0];
        x_k_p_1 = mt_odd_0[0];
        x_k_p_2 = mt_even_0[1];
        x_k_p_m = mt_odd_0[M / 2];
        addr_head = 0;
    }
    /**
     * @brief each call of next() generate a uniformly distributed random number
     *
     * @return a uniformly distributed random number
     */
    ap_ufixed<W, 0> next() {
#pragma HLS inline
#pragma HLS DEPENDENCE variable = mt_even_0 inter false
#pragma HLS DEPENDENCE variable = mt_even_1 inter false
#pragma HLS DEPENDENCE variable = mt_odd_0 inter false
#pragma HLS DEPENDENCE variable = mt_odd_1 inter false
        static const ap_uint<W> A = 0x9908B0DFUL;
        static const ap_uint<W> B = 0x9D2C5680UL;
        static const ap_uint<W> C = 0xEFC60000UL;
        ap_uint<W> tmp;
        ap_uint<1> tmp_0;
        ap_uint<A_W> addr_head_p_3;
        ap_uint<A_W> addr_head_p_m_p_1;
        ap_uint<A_W> addr_head_p_n;
        ap_uint<W> x_k_p_n;
        ap_uint<W> pre_result;
        ap_ufixed<W, 0> result;

        addr_head_p_3 = addr_head + 3;
        addr_head_p_m_p_1 = addr_head + M + 1;
        addr_head_p_n = addr_head + N;
        addr_head++;

        tmp(W - 1, R) = x_k_p_0(W - 1, R);
        tmp(R - 1, 0) = x_k_p_1(R - 1, 0);
        tmp_0 = tmp[0];

        tmp >>= 1;
        if (tmp_0) {
            tmp ^= A;
        }
        x_k_p_n = x_k_p_m ^ tmp;

        x_k_p_0 = x_k_p_1;
        x_k_p_1 = x_k_p_2;

        ap_uint<A_W - 1> rd_addr_0 = addr_head_p_3 >> 1;
        ap_uint<A_W - 1> rd_addr_1 = addr_head_p_m_p_1 >> 1;
        ap_uint<A_W - 1> wr_addr = addr_head_p_n >> 1;

        if (addr_head_p_3[0] == 0) {
            x_k_p_2 = mt_even_0[rd_addr_0];
            x_k_p_m = mt_odd_0[rd_addr_1];
            mt_odd_0[wr_addr] = x_k_p_n;
        } else {
            x_k_p_2 = mt_odd_0[rd_addr_0];
            x_k_p_m = mt_even_0[rd_addr_1];
            mt_even_0[wr_addr] = x_k_p_n;
        }

        pre_result = x_k_p_n;
        pre_result ^= (pre_result >> U);
        pre_result ^= (pre_result << S) & B;
        pre_result ^= (pre_result << T) & C;
        pre_result ^= (pre_result >> L);

        result(W - 1, 0) = pre_result(W - 1, 0);

        return result;
    }
    /**
     * @brief each call of nextTwo() generate two uniformly distributed random numbers
     * @param result_l first random number
     * @param result_r second random number
     */
    void nextTwo(ap_ufixed<W, 0>& result_l, ap_ufixed<W, 0>& result_r) {
#pragma HLS inline
#pragma HLS DEPENDENCE variable = mt_odd_0 inter false
#pragma HLS DEPENDENCE variable = mt_odd_1 inter false
#pragma HLS DEPENDENCE variable = mt_even_0 inter false
#pragma HLS DEPENDENCE variable = mt_even_1 inter false
        static const ap_uint<W> A = 0x9908B0DFUL;
        static const ap_uint<W> B = 0x9D2C5680UL;
        static const ap_uint<W> C = 0xEFC60000UL;

        ap_uint<W> tmp_l, tmp_r;
        ap_uint<1> tmp_0_l, tmp_0_r;
        ap_uint<A_W> addr_head_p_3_l, addr_head_p_3_r;
        ap_uint<A_W> addr_head_p_m_p_1_l, addr_head_p_m_p_1_r;
        ap_uint<A_W> addr_head_p_n_l, addr_head_p_n_r;
        ap_uint<W> x_k_p_n_l, x_k_p_n_r;
        ap_uint<W> pre_result_l, pre_result_r;
        ap_ufixed<W, 0> out_l, out_r;

        addr_head_p_3_l = addr_head + 3;
        addr_head_p_m_p_1_l = addr_head + M + 1;
        addr_head_p_n_l = addr_head + N;

        addr_head_p_3_r = addr_head + 4;
        addr_head_p_m_p_1_r = addr_head + M + 2;
        addr_head_p_n_r = addr_head + N + 1;
        addr_head += 2;

        ap_uint<A_W - 1> rd_addr_l_0 = addr_head_p_3_l >> 1;
        ap_uint<A_W - 1> rd_addr_l_1 = addr_head_p_m_p_1_l >> 1;

        ap_uint<W> x_k_p_2_l = mt_odd_0[rd_addr_l_0];
        ap_uint<W> x_k_p_m_l = mt_even_0[rd_addr_l_1];

        ap_uint<A_W - 1> rd_addr_r_0 = addr_head_p_3_r >> 1;
        ap_uint<A_W - 1> rd_addr_r_1 = addr_head_p_m_p_1_r >> 1;
        ap_uint<W> x_k_p_2_r = mt_even_1[rd_addr_r_0];
        ap_uint<W> x_k_p_m_r = mt_odd_1[rd_addr_r_1];

        tmp_l(W - 1, R) = x_k_p_0(W - 1, R);
        tmp_l(R - 1, 0) = x_k_p_1(R - 1, 0);
        tmp_0_l = tmp_l[0];

        tmp_l >>= 1;
        if (tmp_0_l) {
            tmp_l ^= A;
        }
        x_k_p_n_l = x_k_p_m ^ tmp_l;

        tmp_r(W - 1, R) = x_k_p_1(W - 1, R);
        tmp_r(R - 1, 0) = x_k_p_2(R - 1, 0);
        tmp_0_r = tmp_r[0];

        tmp_r >>= 1;
        if (tmp_0_r) {
            tmp_r ^= A;
        }
        x_k_p_n_r = x_k_p_m_l ^ tmp_r;

        x_k_p_0 = x_k_p_2;
        x_k_p_1 = x_k_p_2_l;
        x_k_p_2 = x_k_p_2_r;
        x_k_p_m = x_k_p_m_r;

        pre_result_l = x_k_p_n_l;
        pre_result_l ^= (pre_result_l >> U);
        pre_result_l ^= (pre_result_l << S) & B;
        pre_result_l ^= (pre_result_l << T) & C;
        pre_result_l ^= (pre_result_l >> L);

        out_l(W - 1, 0) = pre_result_l(W - 1, 0);
        result_l = out_l;

        pre_result_r = x_k_p_n_r;
        pre_result_r ^= (pre_result_r >> U);
        pre_result_r ^= (pre_result_r << S) & B;
        pre_result_r ^= (pre_result_r << T) & C;
        pre_result_r ^= (pre_result_r >> L);

        out_r(W - 1, 0) = pre_result_r(W - 1, 0);
        result_r = out_r;

        ap_uint<A_W - 1> wr_addr_l = addr_head_p_n_l >> 1;
        ap_uint<A_W - 1> wr_addr_r = addr_head_p_n_r >> 1;
        mt_even_0[wr_addr_l] = x_k_p_n_l;
        mt_even_1[wr_addr_l] = x_k_p_n_l;
        mt_odd_0[wr_addr_r] = x_k_p_n_r;
        mt_odd_1[wr_addr_r] = x_k_p_n_r;
    }
};

/**
 * @brief Normally distributed random number generator based on InverseCumulative
 * function
 *
 * @tparam mType data type supported including float and double
 */
template <typename mType>
class MT19937IcnRng {
   public:
    /**
     * @brief Default constructor
     *
     */
    MT19937IcnRng() {
#pragma HLS inline
    }

    /**
     * @brief Constructor with seed
     *
     * @param seed initialization seed
     */
    MT19937IcnRng(ap_uint<32> seed) {
#pragma HLS inline
    }

    /**
     * @brief Initialization using seed
     *
     * @param seed initialization seed
     */
    void seedInitialization(ap_uint<32> seed) {}

    /**
     * @brief Setup status
     *
     * @param data initialization data for setting up status
     */
    void statusSetup(ap_uint<32> data[624]) {}

    /**
     * @brief Get next normally distributed random number
     *
     * @return a normally distributed random number
     */
    mType next() {}

    /**
     * @brief Get next uniformly distributed random number
     *
     * @param uniformR return uniformly distributed random number
     */
    void next(mType& uniformR) {}

    /**
     * @brief Get next normally distributed random number and its corresponding
     * uniformly distributed random number
     *
     * @param gaussianR return normally distributed random number
     * @param uniformR return uniformly distributed random number that
     * corrresponding to gaussianR
     */
    void next(mType& uniformR, mType& gaussianR) {}

    /**
     * @brief Get next two normally distributed random numbers
     *
     * @param gaussR return first normally distributed random number.
     * @param gaussL return second normally distributed random number.
     */
    void nextTwo(mType& gaussR, mType& gaussL) {}
};

/**
 * @brief Normally distributed random number generator based on InverseCumulative
 * function, output datatype is double.
 */
template <>
class MT19937IcnRng<double> {
   public:
    MT19937 uniformRNG;

    MT19937IcnRng(ap_uint<32> seed) : uniformRNG(seed) {
#pragma HLS inline
    }

    MT19937IcnRng() {
#pragma HLS inline
    }

    /**
     * @brief Initialization using seed
     *
     * @param seed initialization seed
     */
    void seedInitialization(ap_uint<32> seed) { uniformRNG.seedInitialization(seed); }

    /**
     * @brief Setup status
     *
     * @param data initialization data for setting up status
     */
    void statusSetup(ap_uint<32> data[624]) { uniformRNG.statusSetup(data); }

    /**
     * @brief Get next normally distributed random number
     *
     * @return a normally distributed random number
     */
    double next() {
#pragma HLS inline
        double tmp_uniform, result;
        tmp_uniform = uniformRNG.next();
        result = inverseCumulativeNormalAcklam<double>(tmp_uniform);
        return result;
    }

    /**
     * @brief Get next normally distributed random number and its corresponding
     * uniformly distributed random number
     *
     * @param gaussianR return normally distributed random number.
     * @param uniformR return uniformly distributed random number that
     * corrresponding to gaussianR
     */
    void next(double& uniformR, double& gaussianR) {
#pragma HLS inline
        double tmp_uniform, result;
        tmp_uniform = uniformRNG.next();
        result = inverseCumulativeNormalAcklam<double>(tmp_uniform);
        uniformR = tmp_uniform;
        gaussianR = result;
    }

    /**
     * @brief Get next uniformly distributed random number
     *
     * @param uniformR return uniformly distributed random number
     */
    void next(double& uniformR) {
#pragma HLS inline
        uniformR = uniformRNG.next();
    }

    /**
     * @brief Get next two normally distributed random number
     *
     * @param gaussR return first normally distributed random number.
     * @param gaussL return second normally distributed random number.
     */
    void nextTwo(double& gaussR, double& gaussL) {
#pragma HLS inline
        ap_ufixed<32, 0> unifR, unifL;
        uniformRNG.nextTwo(unifR, unifL);
        gaussR = inverseCumulativeNormalAcklam<double>(unifR);
        gaussL = inverseCumulativeNormalAcklam<double>(unifL);
    }
};

/**
 * @brief Normally distributed random number generator based on InverseCumulative
 * function, output datatype is float.
 */

template <>
class MT19937IcnRng<float> {
   public:
    MT19937 uniformRNG;

    MT19937IcnRng(ap_uint<32> seed) : uniformRNG(seed) {
#pragma HLS inline
    }

    MT19937IcnRng() {
#pragma HLS inline
    }

    /**
     * @brief Initialization using seed
     *
     * @param seed initialization seed
     */
    void seedInitialization(ap_uint<32> seed) { uniformRNG.seedInitialization(seed); }

    /**
     * @brief Setup status
     *
     * @param data initialization data for setting up status
     */
    void statusSetup(ap_uint<32> data[624]) { uniformRNG.statusSetup(data); }

    /**
     * @brief Get next normally distributed random number
     *
     * @return a normally distributed random number
     */
    float next() {
#pragma HLS inline
        float tmp_uniform, result;
        tmp_uniform = uniformRNG.next();
        result = inverseCumulativeNormalPPND7<float>(tmp_uniform);
        return result;
    }

    /**
     * @brief Get a normally distributed random number and its corresponding
     * uniformly distributed random number
     *
     * @param gaussianR return normally distributed random number.
     * @param uniformR return uniformly distributed random number that
     * corrresponding to gaussianR
     */
    void next(float& uniformR, float& gaussianR) {
#pragma HLS inline
        float tmp_uniform, result;
        tmp_uniform = uniformRNG.next();
        result = inverseCumulativeNormalPPND7<float>(tmp_uniform);
        uniformR = tmp_uniform;
        gaussianR = result;
    }

    /**
     * @brief Get next uniformly distributed random number
     *
     * param uniformR return a uniformly distributed random number
     */
    void next(float& uniformR) {
#pragma HLS inline
        uniformR = uniformRNG.next();
    }

    /**
     * @brief Get next two normally distributed random number
     *
     * @param gaussR return first normally distributed random number.
     * @param gaussL return second normally distributed random number.
     */
    void nextTwo(float& gaussR, float& gaussL) {
#pragma HLS inline
        ap_ufixed<32, 0> unifR, unifL;
        uniformRNG.nextTwo(unifR, unifL);
        gaussR = inverseCumulativeNormalPPND7<float>(unifR);
        gaussL = inverseCumulativeNormalPPND7<float>(unifL);
    }
};

} // namespace fintech
} // namespace xf
#endif // ifndef XF_FINTECH_PRNG_H

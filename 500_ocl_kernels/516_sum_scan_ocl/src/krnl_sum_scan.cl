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
#define LOG2_B (4)
#define B (1<<(LOG2_B))
#define M(x) (((x)-1)/(B) + 1)

#if(B == 16)
typedef float16 bus_t;
#elif(B == 8)
typedef float8 bus_t;
#elif(B == 4)
typedef float4 bus_t;
#elif(B == 2)
typedef float2 bus_t;
#elif(B == 1)
typedef float bus_t;
#endif

#define LOG2_HIST_LENGTH 8
#define HIST_LENGTH ((1<<LOG2_HIST_LENGTH)/(B))

// Tripcount identifiers
__constant int c_b = B;
__constant int c_log2_b = LOG2_B;
__constant int c_m = M(1000);
__constant int c_log2_hlen = LOG2_HIST_LENGTH;
__constant int c_hlen = HIST_LENGTH;

typedef union {
    bus_t b;
    float f[B];
} bus_to_float_t;

__attribute__((always_inline)) bus_t array_to_bus(float *in_r) {
    bus_to_float_t out_r;
    __attribute__((xcl_loop_tripcount(c_b, c_b)))
    atb: for(uint i = 0; i < B; i++) {
        out_r.f[i] = in_r[i];
    }

    return out_r.b;
}

__attribute__((always_inline)) void bus_to_array(bus_t g_in, float *out_r) {
    bus_to_float_t in_r;

    in_r.b = g_in;
    __attribute__((xcl_loop_tripcount(c_b, c_b)))
    bta: for(uint i = 0; i < B; i++) {
        out_r[i] = in_r.f[i];
    }
}

__attribute__((always_inline)) bus_t sum_scan(float *sum, bus_t g_in[HIST_LENGTH+1], uint i) {
    float in_r[(HIST_LENGTH+1)*B] __attribute__((xcl_array_partition(complete, 0)));
    __attribute__((xcl_loop_tripcount(c_hlen, c_hlen)))
    scan1: for(uint j = 0; j < HIST_LENGTH+1; j++) {
        bus_t tmp;
        if(HIST_LENGTH - j > i)
            tmp = 0.0f;
        else
            tmp = g_in[j];

        bus_to_array(tmp, &in_r[B*j]);
    }

#if DEBUG
    __attribute__((xcl_loop_tripcount(c_hlen, c_hlen)))
    scan2: for(uint j = 0; j < (HIST_LENGTH+1)*B; j++) {
        printf("%7.3f ", in_r[B+j]);
    }
    printf("\n");
#endif

    /* Tree based sumation of history */
    __attribute__((xcl_loop_tripcount(c_log2_hlen, c_log2_hlen)))
    scan3: for(uint d = 0; d < LOG2_HIST_LENGTH; d++) {
        uint o1 = 1<<d;
        uint o2 = 1<<(d+1);

        for(uint k = 1; k <= (1<<(LOG2_HIST_LENGTH-1-d)); k++) {
            in_r[k*o2-1] = in_r[k*o2-1] + in_r[k*o2-o1-1];
        }
    }
    
    /* Sum Scan for incoming block */
    __attribute__((xcl_loop_tripcount(c_log2_b, c_log2_b)))
    scan4: for(uint d = 0; d < LOG2_B; d++) {
        uint o0 = B*HIST_LENGTH;
        uint o1 = 1<<d;
        uint o2 = 1<<(d+1);

        scan4_1: for(uint k = 1; k <= (1<<(LOG2_B-1-d)); k++) {
            scan4_2: for(uint j =  0; j < (1<<d); j++) {
                in_r[o0+k*o2-j-1] = in_r[o0+k*o2-j-1] + in_r[o0+k*o2-o1-1];
            }
        }
    }

    *sum += in_r[HIST_LENGTH*B-1];
    __attribute__((xcl_loop_tripcount(c_b, c_b)))
    scan5: for(uint j = 0; j < B; j++) {
        in_r[B*HIST_LENGTH+j] += *sum;
    }
#ifdef DEBUG
    __attribute__((xcl_loop_tripcount(c_b, c_b)))
    scan6: for(uint j = 0; j < B; j++) {
        printf("%7.3f ", in_r[B*HIST_LENGTH+j]);
    }

    printf("[%7.3f] (%d)", *sum, i % (HIST_LENGTH+1));
    printf("\n");
#endif
    return array_to_bus(&in_r[B*HIST_LENGTH]);
}

__kernel void __attribute__ ((reqd_work_group_size(1, 1, 1)))
krnl_sum_scan(
    __global bus_t *in_r,
    __global bus_t *out_r,
    uint length_r
) {
    float sums[HIST_LENGTH];
    bus_t hist[HIST_LENGTH + 1] __attribute__((xcl_array_partition(complete, 0)));

    uint n = M(length_r);

    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_m, c_m)))
    sum_scan1: for(uint i = 0; i < n; i++){
        float sum;

        if(i < HIST_LENGTH+1) {
            sum = 0.0f;
        } else {
            sum = sums[i%(HIST_LENGTH)];
        }
        
        __attribute__((xcl_loop_tripcount(c_hlen, c_hlen)))
        sum_scan2: for(uint j = 0; j < HIST_LENGTH; j++) {
            hist[j] = hist[j+1];
        }
        hist[HIST_LENGTH] = in_r[i];

        out_r[i] = sum_scan(&sum, hist, i);

        sums[i%(HIST_LENGTH)] = sum;
    }
}


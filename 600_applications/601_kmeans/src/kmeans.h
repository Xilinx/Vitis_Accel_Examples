/*******************************************************************************
IMPORTANT:  READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.         
By downloading, copying, installing or using the software you agree        
to this license.  If you do not agree to this license, do not download,    
install, copy or use the software.                                         
                                                                           
Copyright (c) 2005 Northwestern University                                 
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _H_FUZZY_KMEANS
#define _H_FUZZY_KMEANS

#ifndef FLT_MAX
#define FLT_MAX 3.40282347e+38
#endif
#include <string.h>
#include <time.h>

#include "xcl2.hpp"

#define FLOAT_DT    0
#define INT_DT      1

#ifndef USE_DATA_TYPE
#define USE_DATA_TYPE INT_DT
#endif

#if USE_DATA_TYPE == INT_DT
    #define DATA_TYPE unsigned int 
    #define INT_DATA_TYPE int
#else
    #define DATA_TYPE float
    #define INT_DATA_TYPE int
#endif

class FPGA_KMEANS
{
public:
        float **
        fpga_kmeans_clustering(
        float **feature, /* in: [npoints][nfeatures] */
        int nfeatures,
        int npoints,
        int nclusters,
        float threshold,
        int *membership /* out: [npoints] */
        );

        int fpga_kmeans_compute(
            float **feature,    /* in: [npoints][nfeatures] */
            int     n_features,
            int     n_points,
            int     n_clusters,
            int    *membership,
            float **clusters,
            int     *new_centers_len,
            float  **new_centers);

        int fpga_kmeans_setup(int global_size = 1);
        int fpga_kmeans_init(std::string &binaryFile);
        int fpga_kmeans_allocate(int n_points, int n_features, int n_clusters, float **feature);
        int fpga_kmeans_deallocateMemory();
        int fpga_kmeans_print_report();
private:
        cl::Context      g_context;
        cl::CommandQueue g_q;
        cl::Program      g_prog;
        INT_DATA_TYPE   *g_membership_OCL;
        cl::Kernel       g_kernel_kmeans;

        cl::Buffer d_feature;
        cl::Buffer d_cluster;
        cl::Buffer d_membership;

        int g_global_size = 1;
        int g_vector_size = 16;
        float g_scale_factor =  1.0;

        float g_t_exec;
        int   g_iteration;
};

/* rmse.c */
float   euclid_dist_2        (float*, float*, int);
int     find_nearest_point   (float* , int, float**, int);
float   rms_err(float**, int, int, float**, int);
int     cluster(FPGA_KMEANS* fpga, int, int, float**, int, int, float, int*, float***, float*, int, int, std::string &binaryFile, int &index, const char* goldenFile = NULL);
float** kmeans_clustering_cmodel(float **feature, int nfeatures, int npoints, int nclusters, float threshold, 
        int* iteration, int *membership); 

//return elapsed time in ms from t0 to t1
inline double time_elapsed(struct timespec t0, struct timespec t1){
  double result = ((double)t1.tv_sec - (double)t0.tv_sec) * 1.0E9 + ((double)t1.tv_nsec - (double)t0.tv_nsec);
  result = result / 1.0E6;
  return (result);
}
#endif

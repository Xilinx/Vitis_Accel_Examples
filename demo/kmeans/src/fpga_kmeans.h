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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#ifndef _H_FPGA_KMEANS_
#define _H_FPGA_KMEANS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kmeans.h"
#include "kmeans_config.h"
#include "krnl_kmeans.h"
#include "timer.h"
#include "xcl2.hpp"

class FPGA_KMEANS {
public:
  float **fpga_kmeans_clustering(float **feature, float threshold,
                                 int *membership);
  int fpga_kmeans_init(std::string &binaryFile);
  int fpga_kmeans_allocate(int n_points, int n_features, int n_clusters);
  int fpga_kmeans_deallocateMemory();
  int fpga_kmeans_print_report();

private:
  void fpga_kmeans_compute(float **feature, /* in: [npoints][nfeatures] */
                           float threshold, int *membership, float **clusters,
                           int *new_centers_len, unsigned int **new_centers);

  int *m_new_memberships;
  unsigned int *m_new_centers[NUM_CU];

  unsigned int m_nfeatures;
  unsigned int m_nclusters;
  unsigned int m_npoints;

  unsigned int m_buf_cluster_sz;
  unsigned int m_buf_feature_sz;
  unsigned int m_buf_members_sz;
  unsigned int m_buf_centers_sz;

  unsigned int m_num_points_per_cu;
  unsigned int m_num_cu_calls;

  unsigned int *m_scaled_feature;
  unsigned int *m_scaled_clusters;

#ifdef __USE_OPENCL__
  cl::Context m_context;
  cl::CommandQueue m_q;
  cl::Program m_prog;
  cl::Kernel m_kernel_kmeans[NUM_CU];

  cl::Buffer m_buf_feature;
  cl::Buffer m_buf_cluster;
  cl::Buffer m_buf_members;
  cl::Buffer m_buf_centers[NUM_CU];
#endif
};

void cluster(FPGA_KMEANS *fpga, int, int, float **, int, float, float ***, int,
             std::string &binaryFile, const char *goldenFile = NULL);

#endif // _H_FPGA_KMEANS_

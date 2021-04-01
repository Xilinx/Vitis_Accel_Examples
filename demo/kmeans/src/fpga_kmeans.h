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
    float** fpga_kmeans_clustering(float** feature, float threshold, int* membership);
    int fpga_kmeans_init(std::string& binaryFile);
    int fpga_kmeans_allocate(int n_points, int n_features, int n_clusters);
    int fpga_kmeans_deallocateMemory();
    int fpga_kmeans_print_report();

   private:
    void fpga_kmeans_compute(float** feature, /* in: [npoints][nfeatures] */
                             float threshold,
                             int* membership,
                             float** clusters,
                             int* new_centers_len,
                             unsigned int** new_centers);

    int* m_new_memberships;
    unsigned int* m_new_centers[NUM_CU];

    unsigned int m_nfeatures;
    unsigned int m_nclusters;
    unsigned int m_npoints;

    unsigned int m_buf_cluster_sz;
    unsigned int m_buf_feature_sz;
    unsigned int m_buf_members_sz;
    unsigned int m_buf_centers_sz;

    unsigned int m_num_points_per_cu;
    unsigned int m_num_cu_calls;

    unsigned int* m_scaled_feature;
    unsigned int* m_scaled_clusters;

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

void cluster(FPGA_KMEANS* fpga,
             int,
             int,
             float**,
             int,
             float,
             float***,
             int,
             std::string& binaryFile,
             const char* goldenFile = nullptr);

#endif // _H_FPGA_KMEANS_

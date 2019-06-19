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

#include "fpga_kmeans.h"
#include "kmeans.h"
#include "xcl2.hpp"
#include <iostream>

#define FLOAT_DT 0
#define INT_DT 1
#define WORK_GROUP 2

#if USE_DATA_TYPE == INT_DT
#define DATA_TYPE unsigned int
#define INT_DATA_TYPE int
#else
#define DATA_TYPE float
#define INT_DATA_TYPE int
#endif

//Global Variables
int g_global_size = 1;
int g_vector_size = 16;
float g_scale_factor = 1.0;
char *g_fileBuf;

#if USE_DATA_TYPE == INT_DT

static void calculate_scale_factor(float *mem, int size) {
    float min = mem[0];
    float max = mem[0];
    for (int i = 0; i < size; i++) {
        float value = mem[i];
        if (value < min)
            min = value;
        if (value > max)
            max = value;
    }
    float diff = max - min;
    g_scale_factor = diff / 0x00FFFFFF;
    printf("Float to Integer Scale Factor = %f MaxFloat=%f and MinFloat=%f \n",
           g_scale_factor,
           max,
           min);
}

static int scaled_float2int(float value) {
    int ret_value;
    float fv = value;
    float scaled_value = fv / g_scale_factor;
    ret_value = scaled_value;
    return ret_value;
}
#endif

static DATA_TYPE *re_align_clusters(float **clusters,
                                    int n_clusters,
                                    int N_Features,
                                    int n_features) {
    int next_cfeature = 0;
    DATA_TYPE *temp_clusters =
        (DATA_TYPE *)malloc(n_clusters * N_Features * sizeof(DATA_TYPE));
    if (temp_clusters == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for temp_clusters\n");
        exit(EXIT_FAILURE);
    }
    for (int cid = 0; cid < n_clusters; cid++) {
        for (int fid = 0; fid < N_Features; fid++) {
            if (fid < n_features) {
                float fValue = clusters[0][cid * n_features + fid];
                DATA_TYPE value;
#if USE_DATA_TYPE == INT_DT
                value = scaled_float2int(fValue);
#else
                value = fValue;
#endif
                temp_clusters[next_cfeature++] = value;
            } else {
                temp_clusters[next_cfeature++] = 0.0;
            }
        }
    }
    return temp_clusters;
}

static DATA_TYPE *re_align_features(float **feature,
                                    int N_Features,
                                    int NPoints,
                                    int n_features,
                                    int n_points,
                                    int size) {
    int next_feature = 0;
    DATA_TYPE *temp_feature =
        (DATA_TYPE *)malloc(NPoints * n_features * sizeof(DATA_TYPE));
    if (temp_feature == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for temp_feature\n");
        exit(EXIT_FAILURE);
    }
    for (int pid = 0; pid < NPoints; pid += size) {
        for (int fid = 0; fid < n_features; fid++) {
            for (int tpid = 0; tpid < size; tpid++) {
                if (pid + tpid < n_points) {
                    float fValue = feature[0][(pid + tpid) * n_features + fid];
                    DATA_TYPE value;
#if USE_DATA_TYPE == INT_DT
                    value = scaled_float2int(fValue);
#else
                    value = fValue;
#endif
                    temp_feature[next_feature++] = value;
                } else {
                    temp_feature[next_feature++] = 0.0;
                }
            }
        }
    }
    return temp_feature;
}

int FPGA_KMEANS::fpga_kmeans_compute(
    float **feature, /* in: [npoints][nfeatures] */
    int n_features,
    int n_points,
    int n_clusters,
    int *membership,
    float **clusters,
    int *new_centers_len,
    float **new_centers) {
    cl_int err;
    int delta = 0;
    int i, j;
    cl::Event wait_event;
    int N_Features = ((n_features - 1) / g_vector_size + 1) * g_vector_size;
    DATA_TYPE *temp_clusters =
        re_align_clusters(clusters, n_clusters, N_Features, n_features);

    int narg = 0;
    OCL_CHECK(err, err = g_kernel_kmeans.setArg(narg++, d_feature));
    OCL_CHECK(err, err = g_kernel_kmeans.setArg(narg++, d_cluster));
    OCL_CHECK(err, err = g_kernel_kmeans.setArg(narg++, d_membership));
    OCL_CHECK(err,
              err = g_kernel_kmeans.setArg(
                  narg++, sizeof(cl_int), (void *)&n_points));
    OCL_CHECK(err,
              err = g_kernel_kmeans.setArg(
                  narg++, sizeof(cl_int), (void *)&n_clusters));
    OCL_CHECK(err,
              err = g_kernel_kmeans.setArg(
                  narg++, sizeof(cl_int), (void *)&n_features));

    OCL_CHECK(err,
              err = g_q.enqueueWriteBuffer(d_cluster,
                                           CL_TRUE,
                                           0,
                                           n_clusters * N_Features *
                                               sizeof(DATA_TYPE),
                                           temp_clusters,
                                           NULL,
                                           NULL));
    g_q.finish();
    free(temp_clusters);

    OCL_CHECK(err,
              err = g_q.enqueueNDRangeKernel(g_kernel_kmeans,
                                             0,
                                             cl::NDRange(WORK_GROUP, 1, 1),
                                             1,
                                             NULL,
                                             &wait_event));
    g_q.finish();
    OCL_CHECK(err, err = wait_event.wait());

    unsigned long start, stop;

    OCL_CHECK(err,
              err = wait_event.getProfilingInfo<unsigned long>(
                  CL_PROFILING_COMMAND_START, &start));
    OCL_CHECK(err,
              err = wait_event.getProfilingInfo<unsigned long>(
                  CL_PROFILING_COMMAND_END, &stop));
    g_t_exec += (stop - start);
    g_iteration++;

    OCL_CHECK(err,
              err = g_q.enqueueReadBuffer(d_membership,
                                          CL_TRUE,
                                          0,
                                          n_points * sizeof(INT_DATA_TYPE),
                                          g_membership_OCL,
                                          NULL,
                                          NULL));
    g_q.finish();

    delta = 0;
    for (i = 0; i < n_points; i++) {
        int cluster_id = g_membership_OCL[i];
        new_centers_len[cluster_id]++;
        if (g_membership_OCL[i] != membership[i]) {
            delta++;
            membership[i] = g_membership_OCL[i];
        }
        for (j = 0; j < n_features; j++) {
            new_centers[cluster_id][j] += feature[i][j];
        }
    }

    return delta;
}

float **FPGA_KMEANS::fpga_kmeans_clustering(
    float **feature, /* in: [npoints][nfeatures] */
    int nfeatures,
    int npoints,
    int nclusters,
    float threshold,
    int *membership) /* out: [npoints] */
{
    int i, j, n = 0; /* counters */
    int loop = 0, temp;
    int *new_centers_len; /* [nclusters]: no. of points in each cluster */
    float delta;          /* if the point moved */
    float **clusters;     /* out: [nclusters][nfeatures] */
    float **new_centers;  /* [nclusters][nfeatures] */

    int *initial; /* used to hold the index of points not yet selected
                                   prevents the "birthday problem" of dual selection (?)
                                   considered holding initial cluster indices, but changed due to
                                   possible, though unlikely, infinite loops */
    int initial_points;
    int c = 0;

    /* nclusters should never be > npoints
       that would guarantee a cluster without points */
    if (nclusters > npoints)
        nclusters = npoints;

    /* allocate space for and initialize returning variable clusters[] */
    clusters = (float **)malloc(nclusters * sizeof(float *));
    if (clusters == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for clusters\n");
        exit(EXIT_FAILURE);
    }
    clusters[0] = (float *)malloc(nclusters * nfeatures * sizeof(float));
    if (clusters[0] == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for clusters[0]\n");
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < nclusters; i++)
        clusters[i] = clusters[i - 1] + nfeatures;

    /* initialize the random clusters */
    initial = (int *)malloc(npoints * sizeof(int));
    if (initial == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for initial\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < npoints; i++) {
        initial[i] = i;
    }
    initial_points = npoints;

    /* randomly pick cluster centers */
    for (i = 0; i < nclusters && initial_points >= 0; i++) {
        for (j = 0; j < nfeatures; j++)
            clusters[i][j] = feature[initial[n]][j]; // remapped

        /* swap the selected index to the end (not really necessary,
           could just move the end up) */
        temp = initial[n];
        initial[n] = initial[initial_points - 1];
        initial[initial_points - 1] = temp;
        initial_points--;
        n++;
    }

    /* initialize the membership to -1 for all */
    for (i = 0; i < npoints; i++)
        membership[i] = -1;

    /* allocate space for and initialize new_centers_len and new_centers */
    new_centers_len = (int *)calloc(nclusters, sizeof(int));
    if (new_centers_len == NULL) {
        fprintf(stderr,
                "Error: Failed to allocate memory for new_centers_len\n");
        exit(EXIT_FAILURE);
    }

    new_centers = (float **)malloc(nclusters * sizeof(float *));
    if (new_centers == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for new_centers\n");
        exit(EXIT_FAILURE);
    }
    new_centers[0] = (float *)calloc(nclusters * nfeatures, sizeof(float));
    if (new_centers[0] == NULL) {
        fprintf(stderr,
                "Error: Failed to allocate memory for new_centers[0]\n");
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < nclusters; i++)
        new_centers[i] = new_centers[i - 1] + nfeatures;

    /* iterate until convergence */
    printf("\nRunning Iterations : ");
    do {
        printf(" %d ", loop + 1);
        delta = 0.0;
        // CUDA
        delta = (float)fpga_kmeans_compute(
            feature,         /* in: [npoints][nfeatures] */
            nfeatures,       /* number of attributes for each point */
            npoints,         /* number of data points */
            nclusters,       /* number of clusters */
            membership,      /* which cluster the point belongs to */
            clusters,        /* out: [nclusters][nfeatures] */
            new_centers_len, /* out: number of points in each cluster */
            new_centers      /* sum of points in each cluster */
        );

        /* replace old cluster centers with new_centers */
        /* CPU side of reduction */
        for (i = 0; i < nclusters; i++) {
            for (j = 0; j < nfeatures; j++) {
                if (new_centers_len[i] > 0)
                    clusters[i][j] =
                        new_centers[i][j] /
                        new_centers_len[i]; /* take average i.e. sum/n */
                new_centers[i][j] = 0.0;    /* set back to 0 */
            }
            new_centers_len[i] = 0; /* set back to 0 */
        }
        c++;
    } while ((delta > threshold) &&
             (loop++ < 1000)); /* makes sure loop terminates */
    printf("\niterated %d times\n", c);
    free(new_centers[0]);
    free(new_centers);
    free(new_centers_len);

    return clusters;
}

int FPGA_KMEANS::fpga_kmeans_init(std::string &binaryFile) {
    cl_int err;
    unsigned fileBufSize;

    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, g_context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              g_q = cl::CommandQueue(
                  g_context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    g_fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{g_fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, g_prog = cl::Program(g_context, devices, bins, NULL, &err));
    OCL_CHECK(err, g_kernel_kmeans = cl::Kernel(g_prog, "kmeans", &err));

    return 0;
}

int FPGA_KMEANS::fpga_kmeans_allocate(int n_points,
                                      int n_features,
                                      int n_clusters,
                                      float **feature) {
    cl_int err;
    DATA_TYPE *temp_feature;
#if USE_DATA_TYPE == INT_DT
    calculate_scale_factor(feature[0], n_points * n_features);
#endif
    int N_Features = ((n_features - 1) / g_vector_size + 1) * g_vector_size;
    int NPoints = ((n_points - 1) / g_vector_size + 1) * g_vector_size;
    temp_feature = re_align_features(
        feature, N_Features, NPoints, n_features, n_points, g_vector_size);

    OCL_CHECK(err,
              d_feature = cl::Buffer(g_context,
                                     CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
                                     NPoints * n_features * sizeof(DATA_TYPE),
                                     temp_feature,
                                     &err));

    OCL_CHECK(err,
              d_cluster =
                  cl::Buffer(g_context,
                             CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE,
                             n_clusters * N_Features * sizeof(DATA_TYPE),
                             NULL,
                             &err));

    OCL_CHECK(err,
              d_membership =
                  cl::Buffer(g_context,
                             CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE,
                             NPoints * sizeof(INT_DATA_TYPE),
                             NULL,
                             &err));

    free(temp_feature);

    g_membership_OCL = (INT_DATA_TYPE *)malloc(n_points * sizeof(int));
    if (g_membership_OCL == NULL) {
        fprintf(stderr,
                "Error: Failed to allocate memory for g_membership_OCL\n");
        exit(EXIT_FAILURE);
    }
    return true;
}

int FPGA_KMEANS::fpga_kmeans_deallocateMemory() {
    free(g_membership_OCL);
    delete[] g_fileBuf;
    return true;
}

int FPGA_KMEANS::fpga_kmeans_print_report() {
    printf("*******************************************************\n");
    printf("\tK-means Execution Summary:\n");
    printf("*******************************************************\n");
    printf("\tGlobal Size                   : %d\n", g_global_size);
    printf("\tIteration                     : %d\n", g_iteration);
    //Deviding time by 1E6 to change ns(nano sec) to ms (mili sec)
    printf("\tKernel Execution Time(ms)     : %f\n", g_t_exec / 1E6);
    printf("*******************************************************\n");
    return 0;
}

int FPGA_KMEANS::fpga_kmeans_setup(int global_size) {
    g_global_size = global_size;
    return 0;
}

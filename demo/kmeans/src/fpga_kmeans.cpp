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

#include "fpga_kmeans.h"

TIMER_INIT(7);

static float calculate_scale_factor(float *mem, int size) {
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
  float scale_factor = diff / 0x00FFFFFF;

  printf("Float to integer scale factor = %f MaxFloat=%f and MinFloat=%f \n",
         scale_factor, max, min);

  return scale_factor;
}

void scale_clusters(unsigned int *scaled, float **clusters, int n,
                    float scale_factor) {
  for (int i = 0; i < n; i++) {
    scaled[i] = (unsigned int)((float)clusters[0][i] / (float)scale_factor);
  }
}

void scale_features(unsigned int *scaled, float **features, int n,
                    float scale_factor) {
  for (int i = 0; i < n; i++) {
    scaled[i] = (unsigned int)((float)features[0][i] / (float)scale_factor);
  }
}

void scale_and_remap_features(unsigned int *remapped, float **features,
                              int npoints, int nfeatures, float scale_factor) {
  unsigned ptr = 0;
  for (int p = 0; p < npoints; p += 16) {
    for (int f = 0; f < nfeatures; f++) {
      for (int i = 0; i < 16; i++) {
        if ((p + i) < npoints) {
          remapped[ptr++] = features[p + i][f] / scale_factor;
        } else {
          remapped[ptr++] = 0;
        }
      }
    }
  }
}

/****************************************************************

                      FPGA KMEANS COMPUTE()

 ***************************************************************/
void FPGA_KMEANS::fpga_kmeans_compute(
    float **feature, // in: [npoints][nfeatures]
    float threshold, int *membership, float **clusters, int *new_centers_len,
    unsigned int **new_centers) {
  int loop = 0;
  int delta = 0;

  int n_features = m_nfeatures;
  int n_points = m_npoints;
  int n_clusters = m_nclusters;

  float scale_factor =
      calculate_scale_factor(feature[0], n_points * n_features);
  scale_and_remap_features(m_scaled_feature, feature, n_points, n_features,
                           scale_factor);

#ifdef __USE_OPENCL__
  cl_int err;

  // Set kernel arguments up front - they do need to change during the rest of
  // the computation
  for (unsigned i = 0; i < m_num_cu_calls; i++) {
    unsigned start_point = i * m_num_points_per_cu;
    unsigned npoints = std::min(m_num_points_per_cu, n_points - start_point);
    unsigned nclusters = n_clusters;
    unsigned nfeatures = n_features;
    unsigned feature_offset = nfeatures * (start_point / 16);
    unsigned members_offset = start_point / 16;

    int narg = 0;
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, m_buf_feature));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, m_buf_cluster));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, m_buf_members));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, m_buf_centers[i]));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, sizeof(cl_int),
                                                   (void *)&npoints));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, sizeof(cl_int),
                                                   (void *)&nclusters));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, sizeof(cl_int),
                                                   (void *)&nfeatures));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, sizeof(cl_int),
                                                   (void *)&feature_offset));
    OCL_CHECK(err, err = m_kernel_kmeans[i].setArg(narg++, sizeof(cl_int),
                                                   (void *)&members_offset));
  }

  // Write features to the device
  OCL_CHECK(err, err = m_q.enqueueWriteBuffer(m_buf_feature, CL_TRUE, 0,
                                              m_buf_feature_sz,
                                              m_scaled_feature, NULL, NULL));

  OCL_CHECK(err,
            err = m_q.enqueueMigrateMemObjects({m_buf_feature}, 0, NULL, NULL));
  m_q.enqueueBarrier();
#endif

  /* iterate until convergence */
  printf("Running iteration : ");
  do {
    if ((loop++) % 16 == 0)
      printf("\n");
    printf(" %3d ", loop);
    fflush(stdout);

    scale_clusters(m_scaled_clusters, clusters, n_clusters * n_features,
                   scale_factor);

    TIMER_START(1);

#ifdef __USE_OPENCL__

    // Schedule the writing of updated clusters values to the device
    OCL_CHECK(err, err = m_q.enqueueWriteBuffer(m_buf_cluster, CL_TRUE, 0,
                                                m_buf_cluster_sz,
                                                m_scaled_clusters, NULL, NULL));

    // Schedule kernel execution
    for (unsigned i = 0; i < m_num_cu_calls; i++) {
      OCL_CHECK(err, err = m_q.enqueueTask(m_kernel_kmeans[i], NULL, NULL));
    }

    // Ensure enqueueReadBuffer happens after all the enqueueTask have completed
    m_q.enqueueBarrier();

    // Schedule the reading of new memberships values back to the host
    OCL_CHECK(err, err = m_q.enqueueReadBuffer(m_buf_members, CL_TRUE, 0,
                                               m_buf_members_sz,
                                               m_new_memberships, NULL, NULL));

    for (unsigned i = 0; i < m_num_cu_calls; i++) {
      OCL_CHECK(err, err = m_q.enqueueReadBuffer(m_buf_centers[i], CL_TRUE, 0,
                                                 m_buf_centers_sz,
                                                 m_new_centers[i], NULL, NULL));
    }

// enqueueReadBuffer is blocking (CL_TRUE), so no need for m_q.finish();

#else
    for (unsigned i = 0; i < m_num_cu_calls; i++) {
      unsigned start_point = i * m_num_points_per_cu;
      unsigned npoints = std::min(m_num_points_per_cu, n_points - start_point);
      unsigned nclusters = n_clusters;
      unsigned nfeatures = n_features;
      unsigned feature_offset = nfeatures * (start_point / 16);
      unsigned members_offset = start_point / 16;

      kmeans_kernel_wrapper(m_scaled_feature, m_scaled_clusters,
                            m_new_memberships, m_new_centers[i], npoints,
                            nclusters, nfeatures, feature_offset,
                            members_offset);
    }
#endif
    TIMER_STOP;

    TIMER_START(2);
    delta = 0;
    for (int p = 0; p < n_points; p++) {
      if (m_new_memberships[p] != membership[p]) {
        delta++;
        membership[p] = m_new_memberships[p];
      }
    }
    TIMER_STOP;

    TIMER_START(3);
    for (int p = 0; p < n_points; p++) {
      int index = m_new_memberships[p];
      new_centers_len[index]++;
    }
    // Sum the partial centers computed by each CU to form the actual new
    // centers
    for (int c = 0, i = 0; c < n_clusters; c++) {
      for (int f = 0; f < n_features; f++, i++) {
        for (unsigned cu = 0; cu < m_num_cu_calls; cu++) {
          new_centers[c][f] += m_new_centers[cu][i];
        }
      }
    }
    TIMER_STOP;

    TIMER_START(4);
    for (int c = 0; c < n_clusters; c++) {
      for (int f = 0; f < n_features; f++) {
        if (new_centers_len[c] > 0)
          clusters[c][f] = (new_centers[c][f] * scale_factor) /
                           new_centers_len[c]; /* take average i.e. sum/n */
        new_centers[c][f] = 0.0;               /* set back to 0 */
      }
      new_centers_len[c] = 0; /* set back to 0 */
    }
    TIMER_STOP;

  } while ((delta > threshold) &&
           (loop < 1000)); /* makes sure loop terminates */

  printf("\nNumber of iterations : %d \n", loop);
}
/****************************************************************

                   FPGA KMEANS CLUSTERING()

 ***************************************************************/
float **
FPGA_KMEANS::fpga_kmeans_clustering(float **feature, // in: [npoints][nfeatures]
                                    float threshold,
                                    int *membership) // out: [npoints]
{
  int nfeatures = m_nfeatures;
  int npoints = m_npoints;
  int nclusters = m_nclusters;

  int temp, i, j, n = 0;
  int *new_centers_len;       /* [nclusters]: no. of points in each cluster */
  float **clusters;           /* out: [nclusters][nfeatures] */
  unsigned int **new_centers; /* [nclusters][nfeatures] */

  int *initial; /* used to hold the index of points not yet selected
                                 prevents the "birthday problem" of dual
                   selection (?)
                                 considered holding initial cluster indices, but
                   changed due to
                                 possible, though unlikely, infinite loops */
  int initial_points;

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
    fprintf(stderr, "Error: Failed to allocate memory for new_centers_len\n");
    exit(EXIT_FAILURE);
  }

  new_centers = (unsigned int **)malloc(nclusters * sizeof(unsigned int *));
  if (new_centers == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for new_centers\n");
    exit(EXIT_FAILURE);
  }
  new_centers[0] =
      (unsigned int *)calloc(nclusters * nfeatures, sizeof(unsigned int));
  if (new_centers[0] == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for new_centers[0]\n");
    exit(EXIT_FAILURE);
  }
  for (i = 1; i < nclusters; i++)
    new_centers[i] = new_centers[i - 1] + nfeatures;

  /* iterate until convergence */
  TIMER_START(0);
  fpga_kmeans_compute(
      feature,               /* in: [npoints][nfeatures] */
      threshold, membership, /* which cluster the point belongs to */
      clusters,              /* out: [nclusters][nfeatures] */
      new_centers_len,       /* out: number of points in each cluster */
      new_centers            /* sum of points in each cluster */
      );
  TIMER_STOP_ID(0);

  free(new_centers[0]);
  free(new_centers);
  free(new_centers_len);

  return clusters;
}
/****************************************************************

                      FPGA KMEANS INIT()

 ***************************************************************/
int FPGA_KMEANS::fpga_kmeans_init(std::string &binaryFile) {
  TIMER_START(5);

#ifdef __USE_OPENCL__
  cl_int err;

  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();
  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, m_context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              m_q = cl::CommandQueue(m_context, device,
                                     CL_QUEUE_PROFILING_ENABLE |
                                         CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                     &err));

    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    m_prog = cl::Program(m_context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_CU; i++) {
    std::string cuname = "kmeans:{kmeans_" + std::to_string(i + 1) + "}";
    OCL_CHECK(err,
              m_kernel_kmeans[i] = cl::Kernel(m_prog, cuname.c_str(), &err));
  }
#endif

  printf("Application compiled with NUM_CU = %d\n", NUM_CU);

  TIMER_STOP(5);

  return 0;
}
/****************************************************************

                     FPGA KMEANS ALLOCATE()

 ***************************************************************/
int FPGA_KMEANS::fpga_kmeans_allocate(int n_points, int n_features,
                                      int n_clusters) {
  TIMER_START(6);

  m_npoints = n_points;
  m_nfeatures = n_features;
  m_nclusters = n_clusters;

  // There are NUM_CUS and CUs write 16 results in parallel
  // So to divide points evenly, the number of points must be a multiple of both
  // 16 and NUM_CU
  unsigned factor = 16 * NUM_CU;
  unsigned tmp = ((n_points + factor - 1) / factor) * factor;

  m_num_points_per_cu = tmp / NUM_CU;
  m_num_cu_calls = (n_points + m_num_points_per_cu - 1) / m_num_points_per_cu;

  m_buf_members_sz = sizeof(int) * ((n_points + 15) / 16) *
                     16; // Rounded up to nearest multiple of 16
  m_buf_cluster_sz = sizeof(unsigned int) *
                     ((n_clusters * n_features + 15) / 16) *
                     16; // Rounded up to nearest multiple of 16
  m_buf_centers_sz = m_buf_cluster_sz;
  m_buf_feature_sz = sizeof(unsigned int) * m_buf_members_sz * n_features;

  m_new_memberships = (int *)malloc(m_buf_members_sz);
  if (m_new_memberships == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for m_new_memberships\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_CU; i++) {
    m_new_centers[i] = (unsigned int *)malloc(m_buf_centers_sz);
    if (m_new_centers[i] == NULL) {
      fprintf(stderr, "Error: Failed to allocate memory for m_new_centers\n");
      exit(EXIT_FAILURE);
    }
  }

  m_scaled_clusters = (unsigned int *)malloc(m_buf_cluster_sz);
  if (m_scaled_clusters == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for m_scaled_clusters\n");
    exit(EXIT_FAILURE);
  }

  m_scaled_feature = (unsigned int *)malloc(m_buf_feature_sz);
  if (m_scaled_feature == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for m_scaled_feature\n");
    exit(EXIT_FAILURE);
  }

#ifdef __USE_OPENCL__
  cl_int err;
  OCL_CHECK(err, m_buf_feature = cl::Buffer(m_context, CL_MEM_ALLOC_HOST_PTR |
                                                           CL_MEM_READ_ONLY,
                                            m_buf_feature_sz, NULL, &err));
  OCL_CHECK(err, m_buf_cluster = cl::Buffer(m_context, CL_MEM_ALLOC_HOST_PTR |
                                                           CL_MEM_READ_ONLY,
                                            m_buf_cluster_sz, NULL, &err));
  OCL_CHECK(err, m_buf_members = cl::Buffer(m_context, CL_MEM_ALLOC_HOST_PTR |
                                                           CL_MEM_WRITE_ONLY,
                                            m_buf_members_sz, NULL, &err));

  for (int i = 0; i < NUM_CU; i++) {
    OCL_CHECK(err, m_buf_centers[i] = cl::Buffer(
                       m_context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,
                       m_buf_centers_sz, NULL, &err));
  }
#endif

  TIMER_STOP(6);

  return 0;
}
/****************************************************************

                    FPGA KMEANS DE-ALLOCATE()

 ***************************************************************/
int FPGA_KMEANS::fpga_kmeans_deallocateMemory() {
  free(m_scaled_feature);
  free(m_scaled_clusters);
  free(m_new_memberships);

  for (int i = 0; i < NUM_CU; i++) {
    free(m_new_centers[i]);
  }

  return 0;
}
/****************************************************************

                    FPGA KMEANS PRINT REPORT()

 ***************************************************************/
int FPGA_KMEANS::fpga_kmeans_print_report() {
#ifdef __USE_OPENCL__
  printf("------------------------------------------------------\n");
  printf("  Performance Summary                                 \n");
  printf("------------------------------------------------------\n");
  printf("  Device Initialization      : %12.4f ms\n", TIMER_REPORT_MS(5));
  printf("  Buffer Allocation          : %12.4f ms\n", TIMER_REPORT_MS(6));
  printf("------------------------------------------------------\n");
  printf("  Compute Memberships        : %12.4f ms\n", TIMER_REPORT_MS(1));
  printf("  Update Delta               : %12.4f ms\n", TIMER_REPORT_MS(2));
  printf("  Update Centers             : %12.4f ms\n", TIMER_REPORT_MS(3));
  printf("  Update Clusters            : %12.4f ms\n", TIMER_REPORT_MS(4));
  printf("  Total K-Means Compute Time : %12.4f ms\n", TIMER_REPORT_MS(0));
  printf("------------------------------------------------------\n");
#endif
  return 0;
}

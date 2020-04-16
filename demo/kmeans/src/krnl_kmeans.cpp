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

#include "krnl_kmeans.h"
#include "ap_int.h"
#include "hls_stream.h"

#define MAX_VALUE 0xFFFFFFFFFFFFFFFF

#define L_CLUSTERS_SZ ((CONST_NCLUSTERS * CONST_NFEATURES + 15) / 16) * 16

#ifndef PARALLEL_POINTS
#define PARALLEL_POINTS 96
#endif

#if (PARALLEL_POINTS % 16) != 0
#error PARALLEL_POINTS must be a multiple of 16
#endif

#define min(a, b) (a < b) ? a : b

// Tripcount Identifiers
const unsigned int c_nfeatures = CONST_NFEATURES;
const unsigned int c_nclusters = CONST_NCLUSTERS;
const unsigned int c_num_iter = CONST_NPOINTS / (PARALLEL_POINTS * NUM_CU);
const unsigned int c_ld_features = CONST_NFEATURES * PARALLEL_POINTS / 16;
const unsigned int c_ld_clusters = CONST_NFEATURES * CONST_NCLUSTERS / 16;
const unsigned int c_st_centers = CONST_NFEATURES * CONST_NCLUSTERS / 16;
const unsigned int c_st_members = PARALLEL_POINTS / 16;

/*
   This Application do K-means operations.
    operation:For each point find the minimum distance cluster and set the
   cluster id of each Point on membership.
    Arguments:
        feature     (input)     --> Memory location of all the points' features
        clusters    (input)     --> Memory location of all the clusters'
   features
        membership  (output)    --> Kernel execution will find the minimum
   clusters for each point and set the cluster id into this memory
        npoints     (input)     --> Total number of points to execute
        nclusters   (input)     --> Total number clusters
        nfeatures   (input)     --> Total number of features
*/

typedef ap_uint<7> index_t;

struct point_dist_t {
  index_t index;
  unsigned long min_dist;
  unsigned long dist;

  void init() {
    index = 0;
    min_dist = MAX_VALUE;
    dist = 0;
  }

  void update_dist(unsigned long point_value, unsigned int cluster_value) {
    long diff = (point_value - cluster_value);
    dist += diff * diff;
  }

  void update_index(index_t cluster_id) {
    if (dist < min_dist) {
      min_dist = dist;
      index = cluster_id;
    }
    dist = 0;
  }
};

void compute_memberships(
    hls::stream<unsigned int> index_str[PARALLEL_POINTS],
    hls::stream<unsigned int> feature_str[PARALLEL_POINTS],
    unsigned int features[PARALLEL_POINTS][CONST_NFEATURES],
    unsigned int clusters[L_CLUSTERS_SZ], int nclusters, int nfeatures) {
  point_dist_t pt[PARALLEL_POINTS];
#pragma HLS ARRAY_PARTITION variable = pt complete

  for (int p = 0; p < PARALLEL_POINTS; p++) {
#pragma HLS UNROLL
    pt[p].init();
  }

calc_indexes:
  for (int c = 0, offset = 0; c < nclusters; c++, offset += nfeatures) {
#pragma HLS LOOP_TRIPCOUNT min = c_nclusters max = c_nclusters

    for (int f = 0; f < nfeatures; f++) {
#pragma HLS LOOP_TRIPCOUNT min = c_nfeatures max = c_nfeatures
#pragma HLS PIPELINE

      for (int p = 0; p < PARALLEL_POINTS; p++) {
#pragma HLS UNROLL
        pt[p].update_dist(features[p][f], clusters[offset + f]);
        // Stream the features used in this iteration to the next process to
        // compute the new centers
        if (c == (nclusters - 1))
          feature_str[p].write(features[p][f]);
      }
    }

    for (int p = 0; p < PARALLEL_POINTS; p++) {
#pragma HLS UNROLL
      pt[p].update_index(c);
    }
  }
write_index_str:
  for (int p = 0; p < PARALLEL_POINTS; p++) {
#pragma HLS UNROLL
    // Instead of writing the indexes to the membership buffer in global memory,
    // stream them to the next process to compute the new centers
    // The next process will take care of writing the index to global memory
    // instead.
    index_str[p].write(pt[p].index);
  }
}

void compute_new_centers(hls::stream<unsigned int> index_str[PARALLEL_POINTS],
                         hls::stream<unsigned int> feature_str[PARALLEL_POINTS],
                         unsigned int l_new_centers[L_CLUSTERS_SZ],
                         index_t l_index[PARALLEL_POINTS], int npoints,
                         int nclusters, int nfeatures, unsigned &npoints_cnt) {
  index_t index;

calc_centers:
  for (int p = 0; p < PARALLEL_POINTS; p++, npoints_cnt++) {
    for (int f = 0; f < nfeatures; f++) {
#pragma HLS LOOP_TRIPCOUNT min = c_nfeatures max = c_nfeatures
#pragma HLS PIPELINE II = 1
#pragma HLS dependence variable = l_new_centers inter false

      if (f == 0) {
        index = index_str[p].read();
        l_index[p] = index;
      }

      unsigned int feature = feature_str[p].read();

      // Since we've increased the buffer size to be multiple of 16
      // Make sure we don't include unwanted values in the calculation
      if (npoints_cnt < npoints)
        l_new_centers[index * nfeatures + f] += feature;
    }
  }
}

void load_clusters(unsigned int l_clusters[L_CLUSTERS_SZ],
                   ap_int<512> *clusters, int nclusters, int nfeatures) {
  unsigned nreads = (nclusters * nfeatures + 15) / 16;

ld_clusters:
  for (int i = 0; i < nreads; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_ld_clusters max = c_ld_clusters
#pragma HLS PIPELINE

    ap_int<512> tmp = clusters[i];
    for (int j = 0; j < 16; j++) {
      l_clusters[i * 16 + j] = tmp.range(j * 32 + 31, j * 32);
    }
  }
}

void load_features(unsigned int l_features[PARALLEL_POINTS][CONST_NFEATURES],
                   ap_int<512> *feature, int nfeatures, unsigned nmem_reads,
                   unsigned &offset) {
  unsigned f = 0;
  unsigned p = 0;

ld_features:
  for (int i = 0; i < nmem_reads; i++, f++) {
#pragma HLS LOOP_TRIPCOUNT min = c_ld_features max = c_ld_features
#pragma HLS PIPELINE

    if (f == nfeatures) {
      f = 0;
      p++;
    }

    ap_int<512> tmp = feature[offset + i];
    for (int j = 0; j < 16; j++) {
      l_features[p * 16 + j][f] = tmp.range(j * 32 + 31, j * 32);
    }
  }

  offset += PARALLEL_POINTS * nfeatures / 16;
}

void store_memberships(ap_int<512> *membership, index_t index[PARALLEL_POINTS],
                       unsigned nmem_writes, unsigned &offset) {
st_members:
  for (int i = 0; i < nmem_writes; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_st_members max = c_st_members
#pragma HLS PIPELINE

    ap_int<512> tmp = 0;
    for (int j = 0; j < 16; j++) {
      tmp.range(j * 32 + 31, j * 32) = index[i * 16 + j];
    }

    membership[offset + i] = tmp;
  }

  offset += PARALLEL_POINTS / 16;
}

void store_centers(ap_int<512> *new_centers,
                   unsigned int l_new_centers[L_CLUSTERS_SZ], int nclusters,
                   int nfeatures) {
  unsigned nwrites = (nclusters * nfeatures + 15) / 16;

st_centers:
  for (int i = 0; i < nwrites; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_st_centers max = c_st_centers
#pragma HLS PIPELINE

    ap_int<512> tmp;
    for (int j = 0; j < 16; j++) {
      tmp.range(j * 32 + 31, j * 32) = l_new_centers[i * 16 + j];
    }
    new_centers[i] = tmp;
  }
}

void proc_memberships(hls::stream<unsigned int> index_str[PARALLEL_POINTS],
                      hls::stream<unsigned int> feature_str[PARALLEL_POINTS],
                      ap_int<512> *features, ap_int<512> *clusters, int npoints,
                      int nclusters, int nfeatures, unsigned feature_offset) {
#pragma HLS INLINE RECURSIVE

  unsigned max_nvalid_points = PARALLEL_POINTS;
  unsigned min_nvalid_points =
      npoints - (npoints / PARALLEL_POINTS) * PARALLEL_POINTS;
  unsigned max_rd_feature_count =
      nfeatures * ((max_nvalid_points - 1) / 16 + 1);
  unsigned min_rd_feature_count =
      nfeatures * ((min_nvalid_points - 1) / 16 + 1);
  unsigned rd_feature_offset = feature_offset;

  unsigned num_iterations = (npoints + PARALLEL_POINTS - 1) / PARALLEL_POINTS;

  unsigned int l_clusters[L_CLUSTERS_SZ];
#pragma HLS ARRAY_PARTITION variable = l_clusters cyclic factor = 16

  load_clusters(l_clusters, clusters, nclusters, nfeatures);

  for (unsigned int i = 0; i < num_iterations; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_num_iter max = c_num_iter

    unsigned int l_features[PARALLEL_POINTS][CONST_NFEATURES];
#pragma HLS ARRAY_PARTITION variable = l_features complete dim = 1

    unsigned rd_feature_count = (i == (num_iterations - 1))
                                    ? min_rd_feature_count
                                    : max_rd_feature_count;

    load_features(l_features, features, nfeatures, rd_feature_count,
                  rd_feature_offset);

    compute_memberships(index_str, feature_str, l_features, l_clusters,
                        nclusters, nfeatures);
  }
}

void proc_new_centers(ap_int<512> *membership, ap_int<512> *new_centers,
                      hls::stream<unsigned int> index_str[PARALLEL_POINTS],
                      hls::stream<unsigned int> feature_str[PARALLEL_POINTS],
                      int npoints, int nclusters, int nfeatures,
                      unsigned members_offset) {
#pragma HLS INLINE RECURSIVE

  unsigned max_nvalid_points = PARALLEL_POINTS;
  unsigned min_nvalid_points =
      npoints - (npoints / PARALLEL_POINTS) * PARALLEL_POINTS;
  unsigned max_wr_members_count = (max_nvalid_points - 1) / 16 + 1;
  unsigned min_wr_members_count = (min_nvalid_points - 1) / 16 + 1;
  unsigned wr_members_offset = members_offset;
  unsigned wr_centers_count = 0;

  unsigned num_iterations = (npoints + PARALLEL_POINTS - 1) / PARALLEL_POINTS;

  unsigned int l_new_centers[L_CLUSTERS_SZ];
#pragma HLS ARRAY_PARTITION variable = l_new_centers cyclic factor = 16

// Zero the new centers
init_centers:
  for (int i = 0; i < L_CLUSTERS_SZ; i++) {
#pragma HLS PIPELINE
    l_new_centers[i] = 0;
  }

  for (unsigned int i = 0; i < num_iterations; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_num_iter max = c_num_iter

    index_t l_index[PARALLEL_POINTS];
#pragma HLS ARRAY_PARTITION variable = l_index complete

    unsigned wr_members_count = (i == (num_iterations - 1))
                                    ? min_wr_members_count
                                    : max_wr_members_count;

    compute_new_centers(index_str, feature_str, l_new_centers, l_index, npoints,
                        nclusters, nfeatures, wr_centers_count);

    store_memberships(membership, l_index, wr_members_count, wr_members_offset);
  }

  store_centers(new_centers, l_new_centers, nclusters, nfeatures);
}

extern "C" {

void kmeans(ap_int<512> *features, ap_int<512> *clusters,
            ap_int<512> *membership, ap_int<512> *new_centers, int npoints,
            int nclusters, int nfeatures, unsigned feature_offset,
            unsigned members_offset) {
#pragma HLS INTERFACE m_axi port = features bundle = gmem0 offset = slave
#pragma HLS INTERFACE m_axi port = clusters bundle = gmem0 offset = slave
#pragma HLS INTERFACE m_axi port = membership bundle = gmem0 offset = slave
#pragma HLS INTERFACE m_axi port = new_centers bundle = gmem0 offset = slave
#pragma HLS INTERFACE s_axilite port = features bundle = control
#pragma HLS INTERFACE s_axilite port = clusters bundle = control
#pragma HLS INTERFACE s_axilite port = membership bundle = control
#pragma HLS INTERFACE s_axilite port = new_centers bundle = control
#pragma HLS INTERFACE s_axilite port = npoints bundle = control
#pragma HLS INTERFACE s_axilite port = nclusters bundle = control
#pragma HLS INTERFACE s_axilite port = nfeatures bundle = control
#pragma HLS INTERFACE s_axilite port = feature_offset bundle = control
#pragma HLS INTERFACE s_axilite port = members_offset bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

  static hls::stream<unsigned int> index_str[PARALLEL_POINTS];
  static hls::stream<unsigned int> feature_str[PARALLEL_POINTS];

#pragma HLS stream variable = index_str depth = 1
#pragma HLS stream variable = feature_str depth = c_nfeatures

// The DATAFLOW optimization creates a kernel where the two functions
// below run in a concurrent fashion (if allowed by the flow of data)
// While these two functions execute sequentially in software, they
// will execute in a pipelined fashion in the HW kernel

#pragma HLS DATAFLOW

  proc_memberships(index_str, feature_str, features, clusters, npoints,
                   nclusters, nfeatures, feature_offset);

  proc_new_centers(membership, new_centers, index_str, feature_str, npoints,
                   nclusters, nfeatures, members_offset);
}

} // end extern "C"

void kmeans_kernel_wrapper(unsigned int *features, unsigned int *clusters,
                           int *membership, unsigned int *new_centers,
                           int npoints, int nclusters, int nfeatures,
                           unsigned feature_offset, unsigned members_offset) {
  kmeans(reinterpret_cast<ap_int<512> *>(features),
         reinterpret_cast<ap_int<512> *>(clusters),
         reinterpret_cast<ap_int<512> *>(membership),
         reinterpret_cast<ap_int<512> *>(new_centers), npoints, nclusters,
         nfeatures, feature_offset, members_offset);
}

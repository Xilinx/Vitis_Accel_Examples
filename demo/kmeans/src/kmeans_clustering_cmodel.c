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
/*************************************************************************/
/**   File:         kmeans_clustering.c                                 **/
/**   Description:  Implementation of regular k-means clustering        **/
/**                 algorithm                                           **/
/**   Author:  Wei-keng Liao                                            **/
/**            ECE Department, Northwestern University                  **/
/**            email: wkliao@ece.northwestern.edu                       **/
/**                                                                     **/
/**   Edited by: Jay Pisharath                                          **/
/**              Northwestern University.                               **/
/**                                                                     **/
/**   ================================================================  **/
/**                                                                     **/
/**   Edited by: Sang-Ha  Lee                                           **/
/**              University of Virginia                                 **/
/**                                                                     **/
/**   Description:  No longer supports fuzzy c-means clustering;        **/
/**                 only regular k-means clustering.                    **/
/**                 Simplified for main functionality: regular k-means  **/
/**                 clustering.                                         **/
/**                                                                     **/
/**   Edited by: Heera Nand (heera.nand@xilinx.com)                     **/
/**              Xilinx                                                 **/
/**                                                                     **/
/**   Description:Customized code for Xilinx OpenCL Devices             **/
/*************************************************************************/

#include "kmeans.h"
#include <stdio.h>
#include <stdlib.h>

#define RANDOM_MAX 2147483647

#ifndef FLT_MAX
#define FLT_MAX 3.40282347e+38
#endif

extern double wtime(void);

int find_nearest_point(float *pt,                  /* [nfeatures] */
                       int nfeatures, float **pts, /* [npts][nfeatures] */
                       int npts) {
  int index, i;
  float max_dist = FLT_MAX;

  /* find the cluster center id with min distance to pt */
  for (i = 0; i < npts; i++) {
    float dist;
    dist = euclid_dist_2(pt, pts[i], nfeatures); /* no need square root */
    if (dist < max_dist) {
      max_dist = dist;
      index = i;
    }
  }
  return (index);
}

/*----< euclid_dist_2() >----------------------------------------------------*/
/* multi-dimensional spatial Euclid distance square */
__inline float euclid_dist_2(float *pt1, float *pt2, int numdims) {
  int i;
  float ans = 0.0;

  for (i = 0; i < numdims; i++)
    ans += (pt1[i] - pt2[i]) * (pt1[i] - pt2[i]);

  return (ans);
}

/*----< kmeans_clustering() >---------------------------------------------*/
float **kmeans_clustering_cmodel(float **feature, /* in: [npoints][nfeatures] */
                                 int nfeatures, int npoints, int nclusters,
                                 float threshold, int *iteration,
                                 int *membership) /* out: [npoints] */
{

  int i, j, n = 0, index;
  int *new_centers_len; /* [nclusters]: no. of points in each cluster */
  float delta;
  float **clusters;    /* out: [nclusters][nfeatures] */
  float **new_centers; /* [nclusters][nfeatures] */
  *iteration = 0;

  /* allocate space for returning variable clusters[] */
  clusters = (float **)malloc(nclusters * sizeof(float *));
  clusters[0] = (float *)malloc(nclusters * nfeatures * sizeof(float));
  for (i = 1; i < nclusters; i++)
    clusters[i] = clusters[i - 1] + nfeatures;

  /* randomly pick cluster centers */
  for (i = 0; i < nclusters; i++) {
    // n = (int)rand() % npoints;
    for (j = 0; j < nfeatures; j++)
      clusters[i][j] = feature[n][j];
    n++;
  }

  for (i = 0; i < npoints; i++)
    membership[i] = -1;

  /* need to initialize new_centers_len and new_centers[0] to all 0 */
  new_centers_len = (int *)calloc(nclusters, sizeof(int));

  new_centers = (float **)malloc(nclusters * sizeof(float *));
  new_centers[0] = (float *)calloc(nclusters * nfeatures, sizeof(float));
  for (i = 1; i < nclusters; i++)
    new_centers[i] = new_centers[i - 1] + nfeatures;

  do {

    delta = 0.0;

    for (i = 0; i < npoints; i++) {
      /* find the index of nestest cluster centers */
      index = find_nearest_point(feature[i], nfeatures, clusters, nclusters);
      /* if membership changes, increase delta by 1 */
      if (membership[i] != index)
        delta += 1.0;

      /* assign the membership to object i */
      membership[i] = index;

      /* update new cluster centers : sum of objects located within */
      new_centers_len[index]++;
      for (j = 0; j < nfeatures; j++)
        new_centers[index][j] += feature[i][j];
    }

    /* replace old cluster centers with new_centers */
    for (i = 0; i < nclusters; i++) {
      for (j = 0; j < nfeatures; j++) {
        if (new_centers_len[i] > 0)
          clusters[i][j] = new_centers[i][j] / new_centers_len[i];
        new_centers[i][j] = 0.0; /* set back to 0 */
      }
      new_centers_len[i] = 0; /* set back to 0 */
    }

    (*iteration)++;
  } while (delta > threshold);

  free(new_centers[0]);
  free(new_centers);
  free(new_centers_len);

  return clusters;
}

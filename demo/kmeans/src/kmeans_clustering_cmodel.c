/**
* IMPORTANT:  READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
* By downloading, copying, installing or using the software you agree
* to this license.  If you do not agree to this license, do not download,
* install, copy or use the software.
*
* Copyright (c) 2005 Northwestern University
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

int find_nearest_point(float* pt, /* [nfeatures] */
                       int nfeatures,
                       float** pts, /* [npts][nfeatures] */
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
__inline float euclid_dist_2(float* pt1, float* pt2, int numdims) {
    int i;
    float ans = 0.0;

    for (i = 0; i < numdims; i++) ans += (pt1[i] - pt2[i]) * (pt1[i] - pt2[i]);

    return (ans);
}

/*----< kmeans_clustering() >---------------------------------------------*/
float** kmeans_clustering_cmodel(float** feature, /* in: [npoints][nfeatures] */
                                 int nfeatures,
                                 int npoints,
                                 int nclusters,
                                 float threshold,
                                 int* iteration,
                                 int* membership) /* out: [npoints] */
{
    int i, j, n = 0, index;
    int* new_centers_len; /* [nclusters]: no. of points in each cluster */
    float delta;
    float** clusters;    /* out: [nclusters][nfeatures] */
    float** new_centers; /* [nclusters][nfeatures] */
    *iteration = 0;

    /* allocate space for returning variable clusters[] */
    clusters = (float**)malloc(nclusters * sizeof(float*));
    clusters[0] = (float*)malloc(nclusters * nfeatures * sizeof(float));
    for (i = 1; i < nclusters; i++) clusters[i] = clusters[i - 1] + nfeatures;

    /* randomly pick cluster centers */
    for (i = 0; i < nclusters; i++) {
        // n = (int)rand() % npoints;
        for (j = 0; j < nfeatures; j++) clusters[i][j] = feature[n][j];
        n++;
    }

    for (i = 0; i < npoints; i++) membership[i] = -1;

    /* need to initialize new_centers_len and new_centers[0] to all 0 */
    new_centers_len = (int*)calloc(nclusters, sizeof(int));

    new_centers = (float**)malloc(nclusters * sizeof(float*));
    new_centers[0] = (float*)calloc(nclusters * nfeatures, sizeof(float));
    for (i = 1; i < nclusters; i++) new_centers[i] = new_centers[i - 1] + nfeatures;

    do {
        delta = 0.0;

        for (i = 0; i < npoints; i++) {
            /* find the index of nestest cluster centers */
            index = find_nearest_point(feature[i], nfeatures, clusters, nclusters);
            /* if membership changes, increase delta by 1 */
            if (membership[i] != index) delta += 1.0;

            /* assign the membership to object i */
            membership[i] = index;

            /* update new cluster centers : sum of objects located within */
            new_centers_len[index]++;
            for (j = 0; j < nfeatures; j++) new_centers[index][j] += feature[i][j];
        }

        /* replace old cluster centers with new_centers */
        for (i = 0; i < nclusters; i++) {
            for (j = 0; j < nfeatures; j++) {
                if (new_centers_len[i] > 0) clusters[i][j] = new_centers[i][j] / new_centers_len[i];
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

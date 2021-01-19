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
#ifndef _H_FUZZY_KMEANS
#define _H_FUZZY_KMEANS

#ifndef FLT_MAX
#define FLT_MAX 3.40282347e+38
#endif

/* rmse.c */
float euclid_dist_2(float*, float*, int);
int find_nearest_point(float*, int, float**, int);
float rms_err(float**, int, int, float**, int);
float** kmeans_clustering_cmodel(
    float** feature, int nfeatures, int npoints, int nclusters, float threshold, int* iteration, int* membership);

#endif

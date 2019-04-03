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
/**   File:         rmse.c                                              **/
/**   Description:  calculate root mean squared error of particular     **/
/**                 clustering.                                         **/
/**   Author:  Sang-Ha Lee                                              **/
/**            University of Virginia.                                  **/
/**                                                                     **/
/**   Note: euclid_dist_2() and find_nearest_point() adopted from       **/
/**         Minebench code.                                             **/
/**                                                                     **/
/*************************************************************************/

#include <stdio.h>
#include <math.h>

#include "kmeans.h"

extern double wtime(void);

/*----< euclid_dist_2() >----------------------------------------------------*/
/* multi-dimensional spatial Euclid distance square */
__inline
float euclid_dist_2(float *pt1,
                    float *pt2,
                    int    numdims)
{
    int i;
    float ans=0.0;

    for (i=0; i<numdims; i++)
        ans += (pt1[i]-pt2[i]) * (pt1[i]-pt2[i]);

    return(ans);
}

/*----< find_nearest_point() >-----------------------------------------------*/
__inline
int find_nearest_point(float  *pt,          /* [nfeatures] */
                       int     nfeatures,
                       float  **pts,         /* [npts][nfeatures] */
                       int     npts)
{
    int index, i;
    float max_dist=FLT_MAX;

    /* find the cluster center id with min distance to pt */
    for (i=0; i<npts; i++) {
        float dist;
        dist = euclid_dist_2(pt, pts[i], nfeatures);  /* no need square root */
        if (dist < max_dist) {
            max_dist = dist;
            index    = i;
        }
    }
    return(index);
}

/*----< rms_err(): calculates RMSE of clustering >-------------------------------------*/
float rms_err   (float **feature,         /* [npoints][nfeatures] */
                 int     nfeatures,
                 int     npoints,
                 float **cluster_centres, /* [nclusters][nfeatures] */
                 int     nclusters)
{
    int    i;
    int    nearest_cluster_index;   /* cluster center id with min distance to pt */
    float  sum_euclid = 0.0;        /* sum of Euclidean distance squares */
    float  ret;                     /* return value */
    
    /* calculate and sum the sqaure of euclidean distance*/
    for (i=0; i<npoints; i++) {
        nearest_cluster_index = find_nearest_point(feature[i], 
                                                    nfeatures, 
                                                    cluster_centres, 
                                                    nclusters);

        sum_euclid += euclid_dist_2(feature[i],
                                    cluster_centres[nearest_cluster_index],
                                    nfeatures);
        
    }
    /* divide by n, then take sqrt */
    ret = sqrt(sum_euclid / npoints);

    return(ret);
}


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

//Default Setting
#ifndef CLUSTER_MEM_SIZE_IN_KB
#define CLUSTER_MEM_SIZE_IN_KB 32
#endif

#ifndef PARALLEL_POINTS 
#define PARALLEL_POINTS 4
#endif

#ifndef PARALLEL_FEATURES 
#define PARALLEL_FEATURES 2
#endif

#define FLOAT_DT 0
#define INT_DT   1

#ifndef USE_DATA_TYPE
#define USE_DATA_TYPE INT_DT
#endif

#define VECTOR_SIZE 16
#define MEMBERSHIP_TYPE int16
#if USE_DATA_TYPE == INT_DT
    #define DATA_TYPE   unsigned int
    #define VDATA_TYPE  uint16
    #define VMULT_TYPE  ulong16
    #define VECTOR_SIZE 16
    #define MAX_VALUE   0xFFFFFFFFFFFFFFFF
#else
    #define DATA_TYPE   float
    #define VDATA_TYPE  float16
    #define VMULT_TYPE  float16
    #define MAX_VALUE   3.40282347e+38
#endif


// Code is expecting that maximum allowed feature is 64.
// Incase kernel is used more than allowed limit, result will not be expected.  
#define MAX_FEATURES 64

#define MAX_CLUSTER_SIZE (CLUSTER_MEM_SIZE_IN_KB * 1024) // 32KB Reserve for Cluster

/*
   This Application do K-means operations. 
    operation:For each point find the minimum distance cluster and set the cluster id of each Point on membership.
    Arguments:
        feature     (input)     --> Memory location of all the points's features
        membership  (output)    --> Kernel execution will find the minimum clusters for each point and set the cluster id into this memory
        npoints     (input)     --> Total number of points to execute
        nclusters   (input)     --> Total number clusters
        nfeatures   (input)     --> Total number of features
   */
__kernel __attribute__ ((reqd_work_group_size(1, 1, 1))) 
void kmeans(
                __global VDATA_TYPE         * feature,   
                __global VDATA_TYPE         * clusters,
                __global MEMBERSHIP_TYPE    * membership,
                int     _npoints,
                int     nclusters,
                int     nfeatures
              ) 
{
#ifdef DEBUG
    printf("Kernel Cofiguration: CLUSTER_SIZE_IN_KB=%d, PARALLEL_POINTS=%d, PARALLEL_FEATURES=%d\n", CLUSTER_MEM_SIZE_IN_KB, PARALLEL_POINTS, PARALLEL_FEATURES);
#endif
    //Local memory to store centroid. This will help to read entire clusters into local memory so that 
    // cluster's features will be read only once.
    DATA_TYPE cluster_features[ MAX_CLUSTER_SIZE ];


    //local memory to perform burst read entire features of each point before starting the operation
    local VDATA_TYPE point_features[PARALLEL_POINTS][MAX_FEATURES] __attribute__((xcl_array_partition(complete, 1))); 

    int npoints   = (_npoints   -1) / VECTOR_SIZE + 1;

    int N_Features = (nfeatures -1) / VECTOR_SIZE + 1 ;

    int total_cluster_size = nclusters * N_Features;
    int clusterIdx = 0;
    int fIdx = 0;
    cluster_read_l1:for(int i = 0 ; i < total_cluster_size ; i++)
    {
        VDATA_TYPE vValue;
        vValue = clusters[i];
        __attribute__((xcl_pipeline_loop(1)))
        cluster_read_l2:for(int vid = 0 ; (vid < VECTOR_SIZE ) && (fIdx < nfeatures) ; vid ++, fIdx ++)
        {
            cluster_features[clusterIdx] = vValue[vid];
            clusterIdx ++ ;
        }
        if (fIdx >= nfeatures)
        {
            fIdx = 0;
        }
    }

    int global_size = get_global_size(0);
    int global_id   = get_global_id(0);
    int kernel_point_size  = ((npoints -1 ) / global_size + 1);
    int kernel_point_start = global_id * kernel_point_size;
    int kernel_point_end   = kernel_point_start + kernel_point_size;
    if (kernel_point_end > npoints) kernel_point_end = npoints;

#ifdef DEBUG
    printf("Global_Size=%d Global_ID=%d start=%d End=%d\n",global_size, global_id, kernel_point_start, kernel_point_end);
#endif

    // For loop to perform K-means for N points together. For each point, find a cluster which has minimum distance from point. 
    kmeans_itr:for(unsigned int start_point_id = kernel_point_start ; start_point_id < kernel_point_end ; start_point_id += PARALLEL_POINTS)
    {
        int total_points = PARALLEL_POINTS;
        if (start_point_id + total_points > kernel_point_end)
        {
            total_points = kernel_point_end  - start_point_id;
        }

        int fIdx = 0;
        int pIdx = 0;
        //Reading Features of point into local memory
        __attribute__((xcl_pipeline_loop(1)))
        read_features:for (int i = 0 ; i < total_points * nfeatures; i++, fIdx++)
        {
            VDATA_TYPE vValue = feature[start_point_id * nfeatures + i];
            if (fIdx == nfeatures)
            {
                fIdx = 0;
                pIdx++;
            }
            point_features[pIdx][fIdx] = vValue;
        }
        //Local Variable for Keeping Index and Minimum Distance for each point
        MEMBERSHIP_TYPE vIndex[PARALLEL_POINTS];
        VMULT_TYPE vMinDist[PARALLEL_POINTS];
        VMULT_TYPE vAcc[PARALLEL_POINTS];
        __attribute__((opencl_unroll_hint))
        init_loop:for (int i = 0 ; i < PARALLEL_POINTS ; i++)
        {
            vIndex[i]   = 0;
            vAcc[i]     = 0;
            vMinDist[i] = MAX_VALUE;
        }

        int cIdx = 0;
        int cfIdx = 0;
        fIdx = 0;
        int total_loop_count = ( (nfeatures -1) / PARALLEL_FEATURES + 1 )  * nclusters;
        __attribute__((xcl_pipeline_loop(1)))
        kmeans_ops:for (int i = 0; i < total_loop_count ; i ++) 
        {
            VDATA_TYPE vPoint[PARALLEL_FEATURES][PARALLEL_POINTS]__attribute__((xcl_array_partition(complete,0)));
            DATA_TYPE cf[PARALLEL_FEATURES];
            point_init:for ( int fl = 0 ; fl < PARALLEL_FEATURES ; fl ++)
            {
                if (fIdx + fl < nfeatures ) cf[fl]= cluster_features[ cfIdx ++];
                __attribute__((opencl_unroll_hint))
                for (int pid = 0 ; pid < PARALLEL_POINTS ; pid++)
                    vPoint[fl][pid]  = point_features[pid][fIdx + fl];
            }

            __attribute__((opencl_unroll_hint))
            square_and_add:for (int j = 0 ; j < PARALLEL_POINTS ; j++)
            {
               VMULT_TYPE dist[PARALLEL_FEATURES];
               diff_square:for (int fl = 0 ; fl < PARALLEL_FEATURES ; fl ++)
               {
                   VMULT_TYPE vDiff;
                   VDATA_TYPE vValue = vPoint[fl][j];
                  
                   __attribute__((opencl_unroll_hint))
                   for(int vid = 0 ; vid < VECTOR_SIZE; vid++)
                      vDiff[vid] = vValue[vid];

                   vDiff = vDiff - cf[fl];
                   dist[fl]  = vDiff * vDiff;
               }
               __attribute__((opencl_unroll_hint))
               calc_dist:for (int fl = 0 ; fl < PARALLEL_FEATURES ; fl ++ )
                   vAcc[j] += dist[fl];
            }
            if ( (fIdx + PARALLEL_FEATURES) < nfeatures) 
            {
                fIdx += PARALLEL_FEATURES ;
            }else
            {
                fIdx = 0;
                __attribute__((opencl_unroll_hint))
                upate_min:for (int l = 0 ; l < PARALLEL_POINTS; l ++)
                {
                    VMULT_TYPE t_vDist   = vAcc[l];
                    VMULT_TYPE t_vMinDist = vMinDist[l];
                    MEMBERSHIP_TYPE t_vIndex   = vIndex[l];
                     __attribute__((opencl_unroll_hint))
                     for (int vid = 0 ; vid < VECTOR_SIZE; vid ++)
                     {
                         //check if calculated distance is less than minimum
                         if (t_vDist[vid] < t_vMinDist[vid]) 
                         {
                             t_vMinDist[vid] = t_vDist[vid];
                             t_vIndex[vid]   = cIdx;
                         }
                         t_vDist[vid] = 0;
                     }
                     vAcc[l]       = t_vDist;
                     vMinDist[l]   = t_vMinDist;
                     vIndex[l]     = t_vIndex;
                 }
                 cIdx ++;
             }
        }

        //Writing membership 
        __attribute__((xcl_pipeline_loop(1)))
        membership_write:for ( int i = 0 ; i < total_points; i++)
        {
            membership[start_point_id + i] = vIndex[i];
        }
    }
    return;
}


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
/**   File:         cluster.c                                           **/
/**   Description:  Takes as input a file, containing 1 data point per  **/
/**                 per line, and performs a fuzzy c-means clustering   **/
/**                 on the data. Fuzzy clustering is performed using    **/
/**                 min to max clusters and the clustering that gets    **/
/**                 the best score according to a compactness and       **/
/**                 separation criterion are returned.                  **/
/**   Author:  Brendan McCane                                           **/
/**            James Cook University of North Queensland.               **/
/**            Australia. email: mccane@cs.jcu.edu.au                   **/
/**                                                                     **/
/**   Edited by: Jay Pisharath, Wei-keng Liao                           **/
/**              Northwestern University.                               **/
/**                                                                     **/
/**   ================================================================  **/
/**                                                                     **/
/**   Edited by: Shuai Che, David Tarjan, Sang-Ha Lee                   **/
/**              University of Virginia                                 **/
/**                                                                     **/
/**   Description:  No longer supports fuzzy c-means clustering;        **/
/**                 only regular k-means clustering.                    **/
/**                 No longer performs "validity" function to analyze   **/
/**                 compactness and separation crietria; instead        **/
/**                 calculate root mean squared error.                  **/
/**                                                                     **/
/**   Edited by: Heera Nand (heera.nand@xilinx.com)                     **/
/**              Xilinx                                                 **/
/**                                                                     **/
/**   Description:  Customized code for Xilinx OpenCL Devices           **/
/*************************************************************************/

#include <stdio.h>
#include <time.h>
#include "kmeans.h"
#include "fpga_kmeans.h"

float   min_rmse_ref = FLT_MAX;
/* reference min_rmse value */

/*---< cluster() >-----------------------------------------------------------*/
int cluster(
            FPGA_KMEANS* fpga,
            int     npoints,                /* number of data points */
            int     nfeatures,              /* number of attributes for each point */
            float   **features,             /* array: [npoints][nfeatures] */                  
            int     min_nclusters,          /* range of min to max number of clusters */
            int     max_nclusters,
            float   threshold,              /* loop terminating factor */
            int     *best_nclusters,        /* out: number between min and max with lowest RMSE */
            float   ***cluster_centres,     /* out: [best_nclusters][nfeatures] */
            float   *min_rmse,              /* out: minimum RMSE */
            int     isRMSE,                 /* calculate RMSE */
            int     nloops,                 /* number of iteration for each number of clusters */
            std::string &binaryFile,        /* Binary file string */
            const char*   goldenFile                  
            )
{    
    int     nclusters;          /* number of clusters k */
    int     index =0;           /* number of iteration to reach the best RMSE */
    float   rmse;               /* RMSE for each clustering */
    int    *membership;         /* which cluster a data point belongs to */
    int    *cmodel_membership;  /* which cluster a data point belongs to */
    float **tmp_cluster_centres;/* hold coordinates of cluster centers */
    int     i;
    
    struct timespec d_start,d_end;
    double d_time;
    
    /* allocate memory for membership */
    membership = (int*) malloc(npoints * sizeof(int));
    cmodel_membership = (int*) malloc(npoints * sizeof(int));
    if ((membership == NULL) | (cmodel_membership == NULL)){
        fprintf(stderr, "Error: Failed to run malloc\n");
        exit(1);
    }

     fpga->fpga_kmeans_init(binaryFile);

    /* sweep k from min to max_nclusters to find the best number of clusters */
    for(nclusters = min_nclusters; nclusters <= max_nclusters; nclusters++)
    {
        if (nclusters > npoints) break;    /* cannot have more clusters than points */
        clock_gettime(CLOCK_MONOTONIC,&d_start);
        clock_gettime(CLOCK_MONOTONIC,&d_end);
        d_time = time_elapsed(d_start,d_end);
        printf("Device Initialization Time %f ms\n",d_time);
        clock_gettime(CLOCK_MONOTONIC,&d_start);
        /* allocate device memory, (@ kmeans_cuda.cu) */
        fpga->fpga_kmeans_allocate(npoints, nfeatures, nclusters, features);
        clock_gettime(CLOCK_MONOTONIC,&d_end);
        d_time = time_elapsed(d_start,d_end);
        printf("Device Data Writing Time %f ms\n",d_time);
        /* iterate nloops times for each number of clusters */
        for(i = 0; i < nloops; i++)
        {
            printf("Running Device execution \n");
            clock_gettime(CLOCK_MONOTONIC,&d_start);
            /* initialize initial cluster centers, CUDA calls (@ kmeans_cuda.cu) */
            tmp_cluster_centres = fpga->fpga_kmeans_clustering(
                                                    features,
                                                    nfeatures,
                                                    npoints,
                                                    nclusters,
                                                    threshold,
                                                    membership);
            clock_gettime(CLOCK_MONOTONIC,&d_end);
            d_time = time_elapsed(d_start,d_end);
            printf("Device execution Time %f ms\n",d_time);
            printf("Running Host execution \n");

#ifdef VERIFY_USING_CMODEL
            struct timespec h_start,h_end;
            double h_time;
            int iteration;
            float **tmp_cmodel_cluster_centres;        /* hold coordinates of cluster centers of Cmodel */
            clock_gettime(CLOCK_MONOTONIC,&h_start);
            tmp_cmodel_cluster_centres = kmeans_clustering_cmodel(features,
                                                    nfeatures,
                                                    npoints,
                                                    nclusters,
                                                    threshold,
                                                    &iteration,
                                                    cmodel_membership);
            clock_gettime(CLOCK_MONOTONIC,&h_end);
            h_time = time_elapsed(h_start,h_end);
            printf("Host execution Time %f ms\n",h_time);
            printf("Host Iteration %d \n",iteration);
            int mismatch=0;
            int j;
            mismatch = 0;
            for(j = 0 ; j < npoints ; j++){
                if(cmodel_membership[j] != membership[j]){
                    mismatch++;
                }
            }
            float mismatch_rate = float( 100 * mismatch) / npoints;
            if (mismatch_rate> 10 ){
                printf("FAILED:Based on C-Model: Points Membership Mismatch %d Mismatch Rate %.3f \n",mismatch, mismatch_rate);
            }else{
                printf("PASSED:Based on C-Model: Points membership with Match Rate %.3f and mismatch %d with Cmodel. \n", 100.0 - mismatch_rate, mismatch);
            }
            if(tmp_cmodel_cluster_centres){
                free((tmp_cmodel_cluster_centres)[0]);
                free(tmp_cmodel_cluster_centres);
            }
#endif
            if(goldenFile){
                int mismatch = 0;
                //Compare the result with expected golden file
                FILE *inFile;
                if ( (inFile = fopen(goldenFile, "r") ) == NULL){
                   fprintf(stderr, "Error: no such file (%s)\n", goldenFile);
                   exit(1);
                }
                for (int j = 0 ; j < npoints ; j++){
                    int value;
                    fscanf(inFile,"%d",&value);
                    if (value != membership[j])
                        mismatch++;
                }
                float mismatch_rate = float (100 * mismatch) / npoints;
                if (mismatch_rate > 10){
                    printf("FAILED:Based on Golden File: Points Membership Mismatch %d Mismatch Rate %.4f \n",mismatch, mismatch_rate);
                }else{
                    printf("PASSED:Based on Golden File: Points membership with Match Rate %.4f and mismatches only %d compare to GoldenFile. \n", 100.0 - mismatch_rate, mismatch);
                }
            }
            FILE *outFile;
            outFile = fopen("membership.out", "w");
            for (int j = 0 ; j < npoints ; j++){
                fprintf(outFile,"%d\n",membership[j]);
            }
            fclose(outFile);

            if (*cluster_centres) {
                free((*cluster_centres)[0]);
                free(*cluster_centres);
            }
            *cluster_centres = tmp_cluster_centres;
            
                    
            /* find the number of clusters with the best RMSE */
            if(isRMSE)
            {
                rmse = rms_err(features,
                               nfeatures,
                               npoints,
                               tmp_cluster_centres,
                               nclusters);
                
                if(rmse < min_rmse_ref){
                    min_rmse_ref = rmse;            //update reference min RMSE
                    *min_rmse = min_rmse_ref;        //update return min RMSE
                    *best_nclusters = nclusters;    //update optimum number of clusters
                    index = i;                        //update number of iteration to reach best RMSE
                }
            }            
        }
        fpga->fpga_kmeans_deallocateMemory(); 
        fpga->fpga_kmeans_print_report();
    }

    free(membership);
    free(cmodel_membership);

    return index;
}

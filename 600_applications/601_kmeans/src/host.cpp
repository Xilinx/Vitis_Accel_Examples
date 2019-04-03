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
/**   Description:  Takes as input a file:                              **/
/**                 ascii  file: containing 1 data point per line       **/
/**                 binary file: first int is the number of objects     **/
/**                              2nd int is the no. of features of each **/
/**                              object                                 **/
/**                 This example performs a fuzzy c-means clustering    **/
/**                 on the data. Fuzzy clustering is performed using    **/
/**                 min to max clusters and the clustering that gets    **/
/**                 the best score according to a compactness and       **/
/**                 separation criterion are returned.                  **/
/**   Author:  Wei-keng Liao                                            **/
/**            ECE Department Northwestern University                   **/
/**            email: wkliao@ece.northwestern.edu                       **/
/**                                                                     **/
/**   Edited by: Jay Pisharath                                          **/
/**              Northwestern University.                               **/
/**                                                                     **/
/**   Edited by: Heera Nand (heera.nand@xilinx.com)                     **/
/**              Xilinx                                                 **/
/**                                                                     **/
/**   Description: Customized code for Xilinx OpenCL Devices            **/
/**   ==================================================================**/

#define _CRT_SECURE_NO_DEPRECATE 1

#ifndef FLT_MAX
#define FLT_MAX 3.40282347e+38
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "kmeans.h"
#include "fpga_kmeans.h"
#include "cmdlineparser.h"

using namespace sda::utils;

/*---< main() >-------------------------------------------------------------*/
int main(int argc, char **argv) {
    float  *buf;
    char    line[1024];
    int     isBinaryFile = 0;
    int     global_size = 1;
    float   threshold = 0.001;      
    int     max_nclusters=5;        
    int     min_nclusters=5;        
    int     best_nclusters = 0;
    int     nfeatures = 0;
    int     npoints = 0;
             
    float **features;
    float **cluster_centres=NULL;
    int     i, j, index;
    int     nloops = 1;/* default value */
            
    int     isRMSE = 0;
    float   rmse;
    
    int     isOutput = 0;

    //Command Line Parser
    CmdLineParser parser;

                   //"<Full Arg>",  "<Short Arg>", "<Description>",                "<Default>");
    parser.addSwitch("--xclbin_file",   "-x",    "input binary file string",           "");
    parser.addSwitch("--input_file",    "-i",    "input test data file",               "");
    parser.addSwitch("--compare_file",  "-c",    "Compare File to compare result",      "");
    parser.addSwitch("--max_nclusters", "-m",    "maximum number of clusters allowed", "5");
    parser.addSwitch("--min_nclusters", "-n",    "minimum number of clusters allowed", "5");
    parser.addSwitch("--threshold",     "-t",    "thresold value",                     "0.001");
    parser.addSwitch("--output",        "-o",    "output cluster center coordinates",  "0");
    parser.addSwitch("--global_size",   "-g",    "Specify Global Size",                "1");
    parser.parse(argc, argv);

    //read settings
    std::string binaryFile      = parser.value("xclbin_file");
    std::string filename        = parser.value("input_file");   
    std::string goldenfile      = parser.value("compare_file");
    
    max_nclusters   = parser.value_to_int("max_nclusters");
    min_nclusters   = parser.value_to_int("min_nclusters");
    threshold       = atof((parser.value("threshold")).c_str());    
    isOutput        = parser.value_to_int("output");       
    global_size     = parser.value_to_int("global_size");

    if (filename.empty() ){
        parser.printHelp();
        exit(EXIT_FAILURE);
    }

    FPGA_KMEANS* fpga = new FPGA_KMEANS();

    fpga->fpga_kmeans_setup(global_size);

    /* ============== I/O begin ==============*/
    /* get nfeatures and npoints */
    if (isBinaryFile) {//Binary file input
        int infile;
        if ((infile = open(filename.c_str(), O_RDONLY, "0600")) == -1) {
            fprintf(stderr, "Error: no such file (%s)\n", filename.c_str());
            exit(EXIT_FAILURE);
        }
        read(infile, &npoints,   sizeof(int));
        read(infile, &nfeatures, sizeof(int));        

        /* allocate space for features[][] and read attributes of all objects */
        buf         = (float*) malloc(npoints*nfeatures*sizeof(float));
        features    = (float**)malloc(npoints*          sizeof(float*));
        features[0] = (float*) malloc(npoints*nfeatures*sizeof(float));
        for (i=1; i<npoints; i++)
            features[i] = features[i-1] + nfeatures;

        read(infile, buf, npoints*nfeatures*sizeof(float));

        close(infile);
    }
    else {
        FILE *infile;
        if ((infile = fopen(filename.c_str(), "r")) == NULL) {
            fprintf(stderr, "Error: no such file (%s)\n", filename.c_str());
            exit(EXIT_FAILURE);
        }
        while (fgets(line, 1024, infile) != NULL)
        if (strtok(line, " \t\n") != 0)
                npoints++;
        rewind(infile);
        while (fgets(line, 1024, infile) != NULL) {
            if (strtok(line, " \t\n") != 0) {
                /* ignore the id (first attribute): nfeatures = 1; */
                while (strtok(NULL, " ,\t\n") != NULL) nfeatures++;
                break;
            }
        }        

        /* allocate space for features[] and read attributes of all objects */
        buf         = (float*) malloc(npoints*nfeatures*sizeof(float));
        features    = (float**)malloc(npoints*          sizeof(float*));
        features[0] = (float*) malloc(npoints*nfeatures*sizeof(float));
        for (i=1; i<npoints; i++)
            features[i] = features[i-1] + nfeatures;
        rewind(infile);
        i = 0;
        while (fgets(line, 1024, infile) != NULL) {
            if (strtok(line, " \t\n") == NULL) continue;            
            for (j=0; j<nfeatures; j++) {
                buf[i] = atof(strtok(NULL, " ,\t\n"));             
                i++;
            }            
        }
        fclose(infile);
    }
    
    printf("\nI/O completed\n");
    printf("\nfileName=%s\n",filename.c_str());
    printf("\nNumber of objects: %d\n", npoints);
    printf("Number of features: %d\n", nfeatures);
    printf("Number of min cluster: %d\n", min_nclusters);
    printf("Number of max cluster: %d\n", max_nclusters);
    printf("threshold: %f\n", threshold);
    /* ============== I/O end ==============*/

    // error check for clusters
    if (npoints < min_nclusters)
    {
        printf("Error: min_nclusters(%d) > npoints(%d) -- cannot proceed\n", min_nclusters, npoints);
        exit(EXIT_FAILURE);
    }
    
    srand(7);/* seed for future random number generator */
    memcpy(features[0], buf, npoints*nfeatures*sizeof(float)); /* now features holds 2-dimensional array of features */
    free(buf);
    
    /* ======================= core of the clustering ===================*/
    
    
    //FPGA Based cluster
    cluster_centres = NULL;
    index = cluster(
                    fpga,
                    npoints,            /* number of data points */
                    nfeatures,          /* number of features for each point */
                    features,           /* array: [npoints][nfeatures] */
                    min_nclusters,      /* range of min to max number of clusters */
                    max_nclusters,
                    threshold,          /* loop termination factor */
                    &best_nclusters,    /* return: number between min and max */
                    &cluster_centres,   /* return: [best_nclusters][nfeatures] */  
                    &rmse,              /* Root Mean Squared Error */
                    isRMSE,             /* calculate RMSE */
                    nloops,             /* number of iteration for each number of clusters */
                    binaryFile,         /* Binary file string */
                    goldenfile.c_str());        
    
    //cluster_timing = omp_get_wtime() - cluster_timing;
    
    printf("Number of min cluster: %d\n", min_nclusters);
    printf("Number of max cluster: %d\n", max_nclusters);
    printf("threshold: %f\n", threshold);
    printf("Number of best cluster: %d\n", best_nclusters);
    
    /* =============== Command Line Output =============== */
    
    /* cluster center coordinates
       :displayed only for when k=1*/
    if((min_nclusters == max_nclusters) && (isOutput == 1)) {
        printf("\n================= Centroid Coordinates =================\n");
        for(i = 0; i < max_nclusters; i++){
            printf("\n\n%d:", i);
            for(j = 0; j < nfeatures; j++){
                printf(" %.2f", cluster_centres[i][j]);
            }
            printf("\n\n");
        }
    }


    printf("Number of Iteration: %d\n", nloops);
    
    if(min_nclusters != max_nclusters){
        if(nloops != 1){//range of k, multiple iteration
            printf("Best number of clusters is %d\n", best_nclusters);
        }
        else{//range of k, single iteration
            printf("Best number of clusters is %d\n", best_nclusters);
        }
    }
    else{
        if(nloops != 1){// single k, multiple iteration
            if(isRMSE)// if calculated RMSE
               printf("Number of trials to approach the best RMSE of %.3f is %d\n", rmse, index + 1);
        }
        else{// single k, single iteration
            if(isRMSE)// if calculated RMSE
                printf("Root Mean Squared Error: %.3f\n", rmse);
        }
    }
    
    /* free up memory */
    delete fpga;
    free(features[0]);
    free(features);    
 
    return(0);
}


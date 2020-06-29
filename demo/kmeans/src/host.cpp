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
/**   Edited by: Rajat Sarkari (rsarkari@xilinx.com)                    **/
/**              Xilinx                                                 **/
/**                                                                     **/
/**   Description: Customized code for Xilinx OpenCL Devices            **/
/**   ==================================================================**/

#define _CRT_SECURE_NO_DEPRECATE 1

#ifndef FLT_MAX
#define FLT_MAX 3.40282347e+38
#endif

#include "cmdlineparser.h"
#include "fpga_kmeans.h"
#include "kmeans.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace sda::utils;

/**************************************************************************

                                 CLUSTER()

 *************************************************************************/
void cluster(FPGA_KMEANS *fpga, int npoints, /* number of data points */
             int nfeatures,    /* number of attributes for each point */
             float **features, /* array: [npoints][nfeatures] */
             int nclusters,    /* range of min to max number of clusters */
             // int max_nclusters,
             float threshold,          /* loop terminating factor */
             float ***cluster_centres, /* out: [best_nclusters][nfeatures] */
             int nloops, /* number of iteration for each number of clusters */
             std::string &binaryFile, /* Binary file string */
             const char *goldenFile) {

  int *membership;             /* which cluster a data point belongs to */
  int *cmodel_membership;      /* which cluster a data point belongs to */
  float **tmp_cluster_centres; /* hold coordinates of cluster centers */
  int i;

  // Allocate memory for membership
  membership = (int *)malloc(npoints * sizeof(int));
  cmodel_membership = (int *)malloc(npoints * sizeof(int));
  if ((membership == NULL) | (cmodel_membership == NULL)) {
    fprintf(stderr, "Error: Failed to run malloc\n");
    exit(1);
  }

  fpga->fpga_kmeans_init(binaryFile);

  // Allocate device memory
  fpga->fpga_kmeans_allocate(npoints, nfeatures, nclusters);

  // Iterate nloops times for each number of clusters
  for (i = 0; i < nloops; i++) {
    printf("Running device execution \n");
    // Initialize initial cluster centers
    tmp_cluster_centres =
        fpga->fpga_kmeans_clustering(features, threshold, membership);

#ifdef VERIFY_USING_CMODEL
    printf("Running host execution \n");
    int iteration;
    float **tmp_cmodel_cluster_centres; // Hold coordinates of cluster centers
                                        // of Cmodel
    auto h_start = std::chrono::high_resolution_clock::now();
    tmp_cmodel_cluster_centres =
        kmeans_clustering_cmodel(features, nfeatures, npoints, nclusters,
                                 threshold, &iteration, cmodel_membership);
    auto h_end = std::chrono::high_resolution_clock::now();
    auto h_time = std::chrono::duration<double, std::nano>(h_end - h_start);
    printf("Host iteration %d \n", iteration);
    printf("Host execution time %f ms\n", h_time.count() / 1E6);
    int mismatch = 0;
    int j;
    for (j = 0; j < npoints; j++) {
      if (cmodel_membership[j] != membership[j]) {
        mismatch++;
      }
    }
    float mismatch_rate = float(100 * mismatch) / npoints;
    if (mismatch_rate > 10) {
      printf("FAILED:Based on C-Model: Points Membership Mismatch %d "
             "Mismatch Rate %.3f \n",
             mismatch, mismatch_rate);
    } else {
      printf("PASSED:Based on C-Model: Points membership with Match "
             "Rate %.3f and mismatch %d with Cmodel. \n",
             100.0 - mismatch_rate, mismatch);
    }
    if (tmp_cmodel_cluster_centres) {
      free((tmp_cmodel_cluster_centres)[0]);
      free(tmp_cmodel_cluster_centres);
    }

    FILE *outFileC;
    outFileC = fopen("membership_cmodel.out", "w");
    for (int j = 0; j < npoints; j++) {
      fprintf(outFileC, "%d\n", cmodel_membership[j]);
    }
    fclose(outFileC);
#endif

    FILE *outFile;
    outFile = fopen("membership.out", "w");
    for (int j = 0; j < npoints; j++) {
      fprintf(outFile, "%d\n", membership[j]);
    }
    fclose(outFile);

    if (goldenFile && strcmp(goldenFile, "")) {
      int mismatch = 0;
      // Compare the result with expected golden file
      FILE *inFile;
      if ((inFile = fopen(goldenFile, "r")) == NULL) {
        fprintf(stderr, "Error: no such file (%s)\n", goldenFile);
        exit(1);
      }
      for (int j = 0; j < npoints; j++) {
        int value;
        fscanf(inFile, "%d", &value);
        if (value != membership[j])
          mismatch++;
      }
      float mismatch_rate = float(100 * mismatch) / npoints;
      if (mismatch_rate > 10) {
        printf("FAILED:Based on Golden File: Points Membership "
               "Mismatch %d Mismatch Rate %.4f \n",
               mismatch, mismatch_rate);
      } else {
        printf("PASSED:Based on Golden File: Points membership "
               "with Match Rate %.4f and mismatches only %d "
               "compare to GoldenFile. \n",
               100.0 - mismatch_rate, mismatch);
      }
    }

    if (*cluster_centres) {
      free((*cluster_centres)[0]);
      free(*cluster_centres);
    }
    *cluster_centres = tmp_cluster_centres;
  }
  fpga->fpga_kmeans_deallocateMemory();
  fpga->fpga_kmeans_print_report();

  free(membership);
  free(cmodel_membership);
}

/******************************************************************

                           MAIN() FUNCTION

 *****************************************************************/
int main(int argc, char **argv) {
  float *buf;
  char line[1024];
  int isBinaryFile = 0;
  float threshold = 0.001;
  int nclusters = 5;
  int nfeatures = 0;
  int npoints = 0;
  float **features;
  float **cluster_centres = NULL;
  int i, j;
  int nloops = 1; // Default value
  int isOutput = 0;

  // Command Line Parser
  CmdLineParser parser;

  // Switches
  //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
  parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
  parser.addSwitch("--input_file", "-i", "input test data flie", "");
  parser.addSwitch("--compare_file", "-c", "Compare File to compare result",
                   "");
  parser.addSwitch("--nclusters", "-n", "number of clusters", "5");
  parser.addSwitch("--threshold", "-t", "thresold value", "0.001");
  parser.addSwitch("--output", "-o", "output cluster center coordinates", "0");
  parser.parse(argc, argv);

  // Read settings
  std::string binaryFile = parser.value("xclbin_file");
  std::string filename = parser.value("input_file");
  std::string goldenfile = parser.value("compare_file");

  nclusters = parser.value_to_int("nclusters");
  threshold = atof((parser.value("threshold")).c_str());
  isOutput = parser.value_to_int("output");

  if (argc < 7) {
    parser.printHelp();
    return EXIT_FAILURE;
  }

  FPGA_KMEANS *fpga = new FPGA_KMEANS();
  /******************************************************************

                             I/O BEGINS

*****************************************************************/
  // Get nfeatures and npoints
  if (isBinaryFile) { // Binary file input
    int infile;
    if ((infile = open(filename.c_str(), O_RDONLY, "0600")) == -1) {
      fprintf(stderr, "Error: no such file (%s)\n", filename.c_str());
      exit(EXIT_FAILURE);
    }
    read(infile, &npoints, sizeof(int));
    read(infile, &nfeatures, sizeof(int));

    // Allocate space for features[][] and read attributes of all objects
    buf = (float *)malloc(npoints * nfeatures * sizeof(float));
    features = (float **)malloc(npoints * sizeof(float *));
    features[0] = (float *)malloc(npoints * nfeatures * sizeof(float));
    for (i = 1; i < npoints; i++)
      features[i] = features[i - 1] + nfeatures;

    read(infile, buf, npoints * nfeatures * sizeof(float));

    close(infile);
  } else {
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
        // Ignore the id (first attribute): nfeatures = 1;
        while (strtok(NULL, " ,\t\n") != NULL)
          nfeatures++;
        break;
      }
    }

    // Allocate space for features[] and read attributes of all objects
    buf = (float *)malloc(npoints * nfeatures * sizeof(float));
    features = (float **)malloc(npoints * sizeof(float *));
    features[0] = (float *)malloc(npoints * nfeatures * sizeof(float));
    for (i = 1; i < npoints; i++)
      features[i] = features[i - 1] + nfeatures;
    rewind(infile);
    i = 0;
    while (fgets(line, 1024, infile) != NULL) {
      if (strtok(line, " \t\n") == NULL)
        continue;
      for (j = 0; j < nfeatures; j++) {
        buf[i] = atof(strtok(NULL, " ,\t\n"));
        i++;
      }
    }
    fclose(infile);
  }

  printf("\nI/O completed\n");
  printf("\nfileName=%s\n", filename.c_str());
  printf("\n");
  printf("Number of objects       : %d\n", npoints);
  printf("Number of features      : %d\n", nfeatures);
  printf("Number of clusters      : %d\n", nclusters);
  printf("Threshold               : %f\n", threshold);

  /******************************************************************

                            I/O ENDS

*****************************************************************/
  // Error check for clusters
  if (npoints < nclusters) {
    printf("Error: nclusters(%d) > npoints(%d) -- cannot proceed\n", nclusters,
           npoints);
    exit(EXIT_FAILURE);
  }

  srand(7); // Seed for future random number generator
  memcpy(
      features[0], buf,
      npoints * nfeatures *
          sizeof(float)); // Now features holds 2-dimensional array of features
  free(buf);

  /******************************************************************

                   CORE OF THE CLUSTERING

*****************************************************************/
  // FPGA Based cluster
  cluster_centres = NULL;
  cluster(fpga, npoints,    /* number of data points */
          nfeatures,        /* number of features for each point */
          features,         /* array: [npoints][nfeatures] */
          nclusters,        /* number of clusters */
          threshold,        /* loop termination factor */
          &cluster_centres, /* return: [best_nclusters][nfeatures] */
          nloops,     /* number of iteration for each number of clusters */
          binaryFile, /* Binary File string */
          goldenfile.c_str());

  printf("Number of iteration(s)  : %d\n", nloops);

  /******************************************************************

                        COMMAND LINE OUTPUT

*****************************************************************/
  // Cluster center coordinates: displayed only for when k=1
  if (isOutput == 1) {
    printf("\n================= Centroid Coordinates =================\n");
    for (i = 0; i < nclusters; i++) {
      printf("\n\n%d:", i);
      for (j = 0; j < nfeatures; j++) {
        printf(" %.2f", cluster_centres[i][j]);
      }
      printf("\n\n");
    }
  }

  // Free up memory
  delete fpga;
  free(features[0]);
  free(features);

  return (0);
}

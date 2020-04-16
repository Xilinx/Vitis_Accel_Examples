#pragma once

#include "kmeans_config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define CONST_NPOINTS 494020
#define CONST_NCLUSTERS 100
#define CONST_NFEATURES 34

void kmeans_kernel_wrapper(unsigned int *feature, unsigned int *clusters,
                           int *membership, unsigned int *new_centers,
                           int npoints, int nclusters, int nfeatures,
                           unsigned feature_offset, unsigned members_offset);

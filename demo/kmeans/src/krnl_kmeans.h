#pragma once

#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef NUM_CU
#define NUM_CU 8
#endif

#define CONST_NPOINTS   494020
#define CONST_NCLUSTERS 100
#define CONST_NFEATURES 34

void kmeans_kernel_wrapper(
		unsigned int *feature,
		unsigned int *clusters,
		int          *membership,
		unsigned int *new_centers,
		int           npoints,
		int           nclusters,
		int           nfeatures,
		unsigned      feature_offset,
		unsigned      members_offset
);

// #define TRACK printf("line: %5d (function: %s)\n", __LINE__, __FUNCTION__);
#define TRACK


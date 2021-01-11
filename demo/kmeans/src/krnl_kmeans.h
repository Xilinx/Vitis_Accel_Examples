/**
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

#pragma once

#include "kmeans_config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define CONST_NPOINTS 494020
#define CONST_NCLUSTERS 100
#define CONST_NFEATURES 34

void kmeans_kernel_wrapper(unsigned int* feature,
                           unsigned int* clusters,
                           int* membership,
                           unsigned int* new_centers,
                           int npoints,
                           int nclusters,
                           int nfeatures,
                           unsigned feature_offset,
                           unsigned members_offset);

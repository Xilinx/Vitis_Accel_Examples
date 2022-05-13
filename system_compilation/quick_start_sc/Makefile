#
# Copyright (C) 2019-2021 Xilinx, Inc
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%system_compilation/quick_start_sc/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

# source files which have any accelerator code (ie. hls_kernel or vpp_acc code)
# must be specified here:

ACC_SRCS  := src/conv_filter_acc.cpp

# source files with only host code can go here: 

HOST_SRCS := $(XF_PROJ_ROOT)/common/includes/logger/logger.cpp $(XF_PROJ_ROOT)/common/includes/cmdparser/cmdlineparser.cpp src/host_randomized.cpp src/conv_filter_cpu.cpp src/conv_filter_acc_wrapper.cpp 

# potential additional compile options, eg. adding extra -I directories
# need Vivado/include for ap_int.h
#EXTRA_CFLAGS := -I $(XILINX_VIVADO)/include

ifeq ($(TARGET),hw)
HOST_ARGS := -n 60 -w 1920 -h 1080
else
HOST_ARGS := -n 1 -w 64 -h 64
endif

EXTRA_CFLAGS := -I$(XF_PROJ_ROOT)/common/includes/logger 
EXTRA_CFLAGS += -I$(XF_PROJ_ROOT)/common/includes/cmdparser
EXTRA_CFLAGS += -I./src/  

EXTRA_VPPFLAGS = -g

include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk
test: run

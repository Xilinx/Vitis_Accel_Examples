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

# source files which have any accelerator code - hardware code that
# includes PE functions for HLS, and VPP_ACC::compute body
ACC_SRCS  := ./src/acc_wrap.cpp ./src/acc_hls.cpp

# source files with only host code
HOST_SRCS := ./src/host.cpp

# user-defined platform config.hpp
ifeq ($(findstring u50, $(DEVICE)), u50)
  EXTRA_CFLAGS := -I include/u50
else ifeq ($(findstring u280, $(DEVICE)), u280)
  EXTRA_CFLAGS := -I include/u50
else
  EXTRA_CFLAGS := -I include/u200
endif

# HW emulation and profiling debug (for use with xrt.ini)
EXTRA_VPPFLAGS := -g

# HW profiling (set the flag used in profile.hpp)
ifeq (hw,$(TARGET))
  EXTRA_VPPFLAGS += -D ENABLE_HW_PROFILING=1
endif

# template makefile
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk

test: run

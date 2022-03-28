#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, softwarie
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.3
#

# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%hello_world/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

HOST_ARCH := x86
PLATFORM := xilinx_u250_gen3x16_xdma_3_1_202020_1
DEV_ARCH := $(shell platforminfo -p $(PLATFORM) | grep 'FPGA Family' | sed 's/.*://' | sed '/ai_engine/d' | sed 's/^[[:space:]]*//')

ifeq ($(DEV_ARCH), virtexuplus)
include makefile_us_alveo.mk
else
ifeq ($(DEV_ARCH), virtexuplusHBM)
include makefile_us_alveo.mk
else
ifeq ($(DEV_ARCH), zynquplus)
include makefile_zynqmp.mk
else
ifeq ($(DEV_ARCH), zynq)
include makefile_zynq7000.mk
else
ifeq ($(DEV_ARCH), versal)
ifeq ($(HOST_ARCH), x86)
include makefile_versal_alveo.mk
else
include makefile_versal_ps.mk
endif
endif
endif
endif
endif
endif

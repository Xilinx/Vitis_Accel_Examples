#
# Copyright 2019-2021 Xilinx, Inc.
# Copyright 2022-2023 Advanced Micro Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.4
#

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%x3h/ip_tables_rx/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))


DEVICE ?=xilinx_x3522p_gen4x8_hn10g_1_202220_1
PLATFORM_PATH ?= $(PLATFORM_REPO_PATHS)

ACC_SRCS  := $(CUR_DIR)/src/kernel/iptfilterRxsc.cpp
HOST_SRCS := $(CUR_DIR)/src/host.cpp
HOST_SRCS += $(XF_PROJ_ROOT)/common/includes/logger/logger.cpp
HOST_SRCS += $(XF_PROJ_ROOT)/common/includes/cmdparser/cmdlineparser.cpp

EXTRA_CFLAGS := -I$(XF_PROJ_ROOT)/common/includes/cmdparser
EXTRA_CFLAGS += -I$(XF_PROJ_ROOT)/common/includes/logger
EXTRA_CFLAGS += -I$(CUR_DIR)/src/
EXTRA_CFLAGS += -I$(CUR_DIR)/src/kernel/

EXTRA_VPPFLAGS += -I$(CUR_DIR)/src/
EXTRA_VPPFLAGS += -I$(CUR_DIR)/src/kernel/
EXTRA_VPPFLAGS += --advanced.param=compiler.worstNegativeSlack=-0.002

HOST_ARGS := -n $(CUR_DIR)/src/x3host.py -p $(CUR_DIR)/test.pcap -f 0 -r $(CUR_DIR)/iptable.rules

ifeq ($(TARGET),hw_emu)
EXTRA_VPPFLAGS += -D HW_EMU_TEST
EXTRA_CFLAGS += -D HW_EMU_TEST
endif
ifeq ($(TARGET),sw_emu)
EXTRA_VPPFLAGS += -D SW_EMU_TEST
EXTRA_CFLAGS += -D SW_EMU_TEST
endif

#debug := yes
#for centos7
EXTRA_CFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
VPP_LIB      := $(XILINX_VITIS)/system_compiler/lib/centos7

#EXTRA_CFLAGS := -std=c++11
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk

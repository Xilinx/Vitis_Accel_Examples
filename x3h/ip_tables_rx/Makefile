MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%x3h/ip_tables_rx/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

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

HOST_ARGS := $(CUR_DIR)/iptable.rules

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

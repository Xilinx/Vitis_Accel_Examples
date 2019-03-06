.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Device."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make check TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make run_nimbix DEVICE=<FPGA platform>"
	$(ECHO) "      Command to run application on Nimbix Cloud."
	$(ECHO) ""
	$(ECHO) "  make aws_build DEVICE=<FPGA platform>"
	$(ECHO) "      Command to build AWS xclbin application on AWS Cloud."
	$(ECHO) ""

# Points to Utility Directory
COMMON_REPO = ../../../
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))

TARGETS := hw
TARGET := $(TARGETS)
DEVICE := $(DEVICES)
XCLBIN := ./xclbin

include ./utils.mk

DSA := $(call device2sandsa, $(DEVICE))
BUILD_DIR := ./_x.$(TARGET).$(DSA)

BUILD_DIR_nearest_neighbor = $(BUILD_DIR)/nearest_neighbor

CXX := $(XILINX_SDX)/bin/xcpp
XOCC := $(XILINX_SDX)/bin/xocc

#Include Libraries
include $(ABS_COMMON_REPO)/libs/opencl/opencl.mk
include $(ABS_COMMON_REPO)/libs/xcl2/xcl2.mk
CXXFLAGS += $(xcl2_CXXFLAGS)
LDFLAGS += $(xcl2_LDFLAGS)
HOST_SRCS += $(xcl2_SRCS)
CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++14
LDFLAGS += $(opencl_LDFLAGS)

HOST_SRCS += src/host.cpp

# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

# Kernel compiler global settings
CLFLAGS += -t $(TARGET) --platform $(DEVICE) --save-temps 


EXECUTABLE = loop_fusion
CMD_ARGS = $(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xclbin

EMCONFIG_DIR = $(XCLBIN)/$(DSA)

BINARY_CONTAINERS += $(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xclbin
BINARY_CONTAINER_nearest_neighbor_OBJS += $(XCLBIN)/nearest_neighbor_loop_fusion.$(TARGET).$(DSA).xo
BINARY_CONTAINER_nearest_neighbor_OBJS += $(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xo

CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

# Building kernel
$(XCLBIN)/nearest_neighbor_loop_fusion.$(TARGET).$(DSA).xo: src/nearest_neighbor.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) --temp_dir $(BUILD_DIR_nearest_neighbor) -c -k nearest_neighbor_loop_fusion -I'$(<D)' -o'$@' '$<'
$(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xo: src/nearest_neighbor_bad.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) --temp_dir $(BUILD_DIR_nearest_neighbor) -c -k nearest_neighbor -I'$(<D)' -o'$@' '$<'
$(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xclbin: $(BINARY_CONTAINER_nearest_neighbor_OBJS)
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) --temp_dir $(BUILD_DIR_nearest_neighbor) -l $(LDCLFLAGS) --nk nearest_neighbor_loop_fusion:1 --nk nearest_neighbor:1 -o'$@' $(+)

# Building Host
$(EXECUTABLE): $(HOST_SRCS) $(HOST_HDRS)
	mkdir -p $(XCLBIN)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

check: all
ifeq ($(findstring xilinx_kcu1500_dynamic, $(DEVICE)), xilinx_kcu1500_dynamic)
$(error Nothing to be done for make)
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xclbin
else
	 ./$(EXECUTABLE) $(XCLBIN)/nearest_neighbor.$(TARGET).$(DSA).xclbin
endif
	sdx_analyze profile -i sdaccel_profile_summary.csv -f html

run_nimbix: all
	$(COMMON_REPO)/utility/nimbix/run_nimbix.py $(EXECUTABLE) $(CMD_ARGS) $(DSA)

aws_build: check-aws_repo $(BINARY_CONTAINERS)
	$(COMMON_REPO)/utility/aws/run_aws.py $(BINARY_CONTAINERS)

# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) sdaccel_* TempConfig system_estimate.xtxt *.rpt
	-$(RMDIR) src/*.ll _xocc_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) $(XCLBIN)
	-$(RMDIR) _x.*


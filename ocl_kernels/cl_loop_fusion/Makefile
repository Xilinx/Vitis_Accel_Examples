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
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make run_nimbix DEVICE=<FPGA platform>"
	$(ECHO) "      Command to run application on Nimbix Cloud."
	$(ECHO) ""
	$(ECHO) "  make aws_build DEVICE=<FPGA platform>"
	$(ECHO) "      Command to build AWS xclbin application on AWS Cloud."
	$(ECHO) ""

# Points to Utility Directory
COMMON_REPO = ../../common/
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))

TARGET := hw

include ./utils.mk

DSA := $(call device2dsa, $(DEVICE))
TEMP_DIR := ./_x.$(TARGET).$(DSA)
BUILD_DIR := ./build_dir.$(TARGET).$(DSA)

CXX := $(XILINX_SCOUT)/bin/xcpp
XOCC := v++

#Include Libraries
include $(ABS_COMMON_REPO)/includes/opencl/opencl.mk
include $(ABS_COMMON_REPO)/includes/xcl2/xcl2.mk
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
CLFLAGS += -t $(TARGET) --platform $(DEVICE) --save-temps --report_dir $(BUILD_DIR)/reports


EXECUTABLE = loop_fusion
CMD_ARGS = $(BUILD_DIR)/nearest_neighbor.xclbin

EMCONFIG_DIR = $(TEMP_DIR)

BINARY_CONTAINERS += $(BUILD_DIR)/nearest_neighbor.xclbin
BINARY_CONTAINER_nearest_neighbor_OBJS += $(TEMP_DIR)/nearest_neighbor.xo
BINARY_CONTAINER_nearest_neighbor_OBJS += $(TEMP_DIR)/nearest_neighbor_loop_fusion.xo

CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

.PHONY: build
build: $(BINARY_CONTAINERS)

# Building kernel
$(TEMP_DIR)/nearest_neighbor.xo: src/nearest_neighbor.cl
	mkdir -p $(TEMP_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(TEMP_DIR) -c -k nearest_neighbor -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/nearest_neighbor_loop_fusion.xo: src/nearest_neighbor.cl
	mkdir -p $(TEMP_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(TEMP_DIR) -c -k nearest_neighbor_loop_fusion -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/nearest_neighbor.xclbin: $(BINARY_CONTAINER_nearest_neighbor_OBJS)
	mkdir -p $(BUILD_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(BUILD_DIR) -l $(LDCLFLAGS) --nk nearest_neighbor:1 --nk nearest_neighbor_loop_fusion:1 -o'$@' $(+)

# Building Host
$(EXECUTABLE): check-xrt $(HOST_SRCS) $(HOST_HDRS)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

check: all
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(BUILD_DIR)/nearest_neighbor.xclbin
else
	 ./$(EXECUTABLE) $(BUILD_DIR)/nearest_neighbor.xclbin
endif
	sdx_analyze profile -i profile_summary.csv -f html

run_nimbix: all
	$(COMMON_REPO)/utility/nimbix/run_nimbix.py $(EXECUTABLE) $(CMD_ARGS) $(DSA)

aws_build: check-aws_repo $(BINARY_CONTAINERS)
	$(COMMON_REPO)/utility/aws/run_aws.py $(BINARY_CONTAINERS)

# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll _xocc_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir*
	-$(RMDIR) _x.*


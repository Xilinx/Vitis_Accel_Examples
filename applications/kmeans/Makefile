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
COMMON_REPO = ../../
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))

TARGET := hw

include ./utils.mk

DSA := $(call device2dsa, $(DEVICE))
TEMP_DIR := ./_x.$(TARGET).$(DSA)
BUILD_DIR := ./build_dir.$(TARGET).$(DSA)

CXX := $(XILINX_SCOUT)/bin/xcpp
XOCC := v++

#Include Libraries
include $(ABS_COMMON_REPO)/common/includes/opencl/opencl.mk
include $(ABS_COMMON_REPO)/common/includes/logger/logger.mk
include $(ABS_COMMON_REPO)/common/includes/cmdparser/cmdparser.mk
include $(ABS_COMMON_REPO)/common/includes/xcl2/xcl2.mk
CXXFLAGS += $(logger_CXXFLAGS) $(cmdparser_CXXFLAGS) $(xcl2_CXXFLAGS)
LDFLAGS += $(logger_LDFLAGS) $(cmdparser_LDFLAGS) $(xcl2_LDFLAGS)
HOST_SRCS += $(logger_SRCS) $(cmdparser_SRCS) $(xcl2_SRCS)
include config.mk

CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++14
LDFLAGS += $(opencl_LDFLAGS)

HOST_SRCS += ./src/host.cpp ./src/cluster.c ./src/rmse.c ./src/fpga_kmeans.cpp ./src/kmeans_clustering_cmodel.c 

# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

# Kernel compiler global settings
CLFLAGS += -t $(TARGET) --platform $(DEVICE) --save-temps 


EXECUTABLE = host_kmeans
CMD_ARGS = -x $(BUILD_DIR)/kmeans.xclbin -i ./data/100 -c ./data/100.gold_c5 -m 5 -n 5 -g 2

EMCONFIG_DIR = $(TEMP_DIR)

BINARY_CONTAINERS += $(BUILD_DIR)/kmeans.xclbin
BINARY_CONTAINER_kmeans_OBJS += $(TEMP_DIR)/kmeans.xo

CP = cp -rf
DATA = ./data

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

.PHONY: build
build: $(BINARY_CONTAINERS)

# Building kernel
$(TEMP_DIR)/kmeans.xo: src/kmeans.cl
	mkdir -p $(TEMP_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(TEMP_DIR) -c -k kmeans -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/kmeans.xclbin: $(BINARY_CONTAINER_kmeans_OBJS)
	mkdir -p $(BUILD_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(BUILD_DIR) -l $(LDCLFLAGS) --nk kmeans:2 -o'$@' $(+)

# Building Host
$(EXECUTABLE): check-xrt $(HOST_SRCS) $(HOST_HDRS)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

check: all
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) -x $(BUILD_DIR)/kmeans.xclbin -i ./data/100 -c ./data/100.gold_c5 -m 5 -n 5 -g 2
else
	 ./$(EXECUTABLE) -x $(BUILD_DIR)/kmeans.xclbin -i ./data/100 -c ./data/100.gold_c5 -m 5 -n 5 -g 2
endif
	sdx_analyze profile -i profile_summary.csv -f html

run_nimbix: all
	$(COMMON_REPO)/common/utility/nimbix/run_nimbix.py $(EXECUTABLE) $(CMD_ARGS) $(DSA)

aws_build: check-aws_repo $(BINARY_CONTAINERS)
	$(COMMON_REPO)/common/utility/aws/run_aws.py $(BINARY_CONTAINERS)

# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll _xocc_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir*
	-$(RMDIR) _x.*
	-$(RMDIR) ./membership.out 

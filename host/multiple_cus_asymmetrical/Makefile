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

CXX := $(XILINX_VITIS)/bin/xcpp
XOCC := v++

#Include Libraries
include $(ABS_COMMON_REPO)/common/includes/opencl/opencl.mk
include $(ABS_COMMON_REPO)/common/includes/xcl2/xcl2.mk
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


# Kernel linker flags
LDCLFLAGS += --sp vadd_1.in1:DDR[0] --sp vadd_1.in2:DDR[0] --sp vadd_1.out_r:DDR[0] --sp vadd_2.in1:DDR[1] --sp vadd_2.in2:DDR[1] --sp vadd_2.out_r:DDR[1] --sp vadd_3.in1:PLRAM[0] --sp vadd_3.in2:PLRAM[0] --sp vadd_3.out_r:PLRAM[0] --sp vadd_4.in1:PLRAM[1] --sp vadd_4.in2:PLRAM[1] --sp vadd_4.out_r:PLRAM[1]  

EXECUTABLE = host
CMD_ARGS = $(BUILD_DIR)/vadd.xclbin

EMCONFIG_DIR = $(TEMP_DIR)

BINARY_CONTAINERS += $(BUILD_DIR)/vadd.xclbin
BINARY_CONTAINER_vadd_OBJS += $(TEMP_DIR)/vadd.xo

CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

.PHONY: build
build: $(BINARY_CONTAINERS)

# Building kernel
$(TEMP_DIR)/vadd.xo: src/vadd.cpp
	mkdir -p $(TEMP_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(TEMP_DIR) -c -k vadd -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/vadd.xclbin: $(BINARY_CONTAINER_vadd_OBJS)
	mkdir -p $(BUILD_DIR)
	$(XOCC) $(CLFLAGS) --temp_dir $(BUILD_DIR) -l $(LDCLFLAGS) --nk vadd:4:vadd_1.vadd_2.vadd_3.vadd_4 -o'$@' $(+)

# Building Host
$(EXECUTABLE): check-xrt $(HOST_SRCS) $(HOST_HDRS)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

check: all
ifeq ($(findstring xilinx_aws-vu9p-f1-04261818, $(DEVICE)), xilinx_aws-vu9p-f1-04261818)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx:aws:vu9p:f1:04261818, $(DEVICE)), xilinx:aws:vu9p:f1:04261818)
$(error Nothing to be done for make)
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(BUILD_DIR)/vadd.xclbin
else
	 ./$(EXECUTABLE) $(BUILD_DIR)/vadd.xclbin
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
	-$(RMDIR) _x.* xclbin.run_summary


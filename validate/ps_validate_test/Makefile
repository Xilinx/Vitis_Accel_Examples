#
# Copyright 2022 Xilinx, Inc.
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
# makefile-generator v1.0.3
#

############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make test PLATFORM=<FPGA platform>"
	$(ECHO) "     Command to run the application. This is same as 'run' target but does not have any makefile dependency."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host"
	$(ECHO) "      Command to build host application."
	$(ECHO) ""

############################## Setting up Project Variables ##############################
# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%validate/ps_validate_test/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

TARGET := hw
HOST_ARCH := x86
SYSROOT := 

include ./utils.mk

XSA := 
ifneq ($(PLATFORM), )
XSA := $(call device2xsa, $(PLATFORM))
endif
TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)

LINK_OUTPUT := $(BUILD_DIR)/bandwidth.link.xclbin
ifeq ($(DEV_ARCH), versal)
LINK_OUTPUT := $(BUILD_DIR)/bandwidth.xsa
endif

# SoC variables
RUN_APP_SCRIPT = ./run_app.sh
PACKAGE_OUT = ./package.$(TARGET)

LAUNCH_EMULATOR = $(PACKAGE_OUT)/launch_$(TARGET).sh
RESULT_STRING = TEST PASSED

VPP := v++
VPP_PFLAGS := 
CMD_ARGS = -p $(GEN_DIR) 

include config.mk

CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++1y
LDFLAGS += $(opencl_LDFLAGS)

########################## Checking if PLATFORM in whitelist #######################
PLATFORM_BLOCKLIST += zc vck190 2019 2018 

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
HOST_SRCS += ./src/host.cpp 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0 -I$(XILINX_XRT)/include
LDFLAGS += -lrt -lstdc++ -lxrt_core -lxrt_coreutil -lpthread -luuid -L$(XILINX_XRT)/lib
LDFLAGS += -lboost_filesystem -lboost_system

############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += -t $(TARGET) --platform $(PLATFORM) --save-temps 

EXECUTABLE = ./ps_validate_test.exe
EMCONFIG_DIR = $(TEMP_DIR)

############################## Setting up PS Kernel Variables ##############################
CROSS_COMPILE := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-
CROSSCXX := $(CROSS_COMPILE)g++
ps_target = ps_validate.so
ps_OBJS = ps_validate.o
SYSROOT = /proj/xbuilds/2022.1_daily_latest/internal_platforms/sw/versal/xilinx-versal/sysroots/aarch64-xilinx-linux
CROSSCFLAGS = -I$(SYSROOT)//usr/include/xrt -I$(SYSROOT)/ -fPIC -fvisibility=hidden -lrt --sysroot=$(SYSROOT) -Ilibs
CROSSCFLAGS += -Wall -O2 -g

CROSSLDFLAGS = -shared -lxrt_core -L$(SYSROOT)/usr/lib -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ --sysroot=$(SYSROOT)

############################## Declaring Binary Containers ##############################
BINARY_CONTAINERS += $(BUILD_DIR)/ps_validate_bandwidth.xclbin
BINARY_CONTAINERS_TEMP += $(BUILD_DIR)/ps_validate_bandwidth_temp.xclbin
BINARY_CONTAINER_bandwidth_OBJS += $(TEMP_DIR)/bandwidth.xo

############################## Setting Targets ##############################
CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-platform check-device check-vitis platform_test $(EXECUTABLE) $(ps_target) $(BUILD_DIR)/$(LINK_OUTPUT) $(BINARY_CONTAINERS_TEMP) $(BINARY_CONTAINERS) emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: check-vitis check-device platform_test $(ps_target) $(BINARY_CONTAINERS_TEMP) $(BINARY_CONTAINERS)

.PHONY: xclbin
xclbin: build

############################## Setting Rules for PS Kernel objects (Building PS Kernels) ##############################
$(ps_OBJS) : ./src/ps_validate.c
	$(CROSSCXX) -c $(CROSSCFLAGS) -o $@ $<

$(ps_target): $(ps_OBJS)
	$(CROSSCXX) $< $(CROSSLDFLAGS) -o $@

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/bandwidth.xo: src/bandwidth.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) $(VPP_FLAGS) -c -k bandwidth --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/$(LINK_OUTPUT): $(BINARY_CONTAINER_bandwidth_OBJS)
	mkdir -p $(BUILD_DIR)
	$(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR)  -o'$(LINK_OUTPUT)' $(+)
$(BINARY_CONTAINERS_TEMP): $(BUILD_DIR)/$(LINK_OUTPUT)
	$(VPP) -p $(LINK_OUTPUT) $(VPP_FLAGS) --package.out_dir $(PACKAGE_OUT) -o $(BINARY_CONTAINERS_TEMP)
$(BINARY_CONTAINERS): $(BINARY_CONTAINERS_TEMP)
	xclbinutil --add-pskernel $(ps_target) --input $(BINARY_CONTAINERS_TEMP) --output $(BINARY_CONTAINERS) --force
	mkdir -p $(GEN_DIR) && $(CP) $(BINARY_CONTAINERS) $(GEN_DIR)
	$(CP) $(PLATFORM_JSON) $(GEN_DIR)

#$(BUILD_DIR)/ps_validate_bandwidth.xclbin: $(BUILD_DIR)/$(LINK_OUTPUT)
#	$(VPP) -p $(LINK_OUTPUT) $(VPP_FLAGS) --package.out_dir $(PACKAGE_OUT) --package.ps_kernel ./ps_validate.so -o $(BUILD_DIR)/ps_validate_bandwidth.xclbin
#	mkdir -p $(GEN_DIR) && $(CP) $(BINARY_CONTAINERS) $(GEN_DIR)
#	$(CP) $(PLATFORM_JSON) $(GEN_DIR)


############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): $(HOST_SRCS) | check-xrt
		$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif


.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
ifeq ($(HOST_ARCH), x86)
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif
endif


############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir*
	-$(RMDIR) package.*
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin


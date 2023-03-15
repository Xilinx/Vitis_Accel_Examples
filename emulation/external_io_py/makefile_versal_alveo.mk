#
# Copyright 2019-2021 Xilinx, Inc.
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
ifneq ($(findstring Makefile, $(MAKEFILE_LIST)), Makefile)
help:
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
	$(ECHO) "      Command to run the application. This is same as 'run' target but does not have any makefile dependency."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
endif

############################## Setting up Project Variables ##############################
TARGET := hw
include ./utils.mk

TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)

LINK_OUTPUT := $(BUILD_DIR)/increment.link.xsa
PACKAGE_OUT = ./package.$(TARGET)

VPP_PFLAGS := 
CMD_ARGS = $(BUILD_DIR)/increment.xclbin

XOS = $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_master_32.xo  $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_slave_32.xo
CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -Wall -O0 -g -std=c++1y
LDFLAGS += -L$(XILINX_XRT)/lib -pthread -lOpenCL

########################## Checking if PLATFORM in allowlist #######################
PLATFORM_BLOCKLIST += aws-vu9p-f1 samsung u2_ 
PLATFORM_ALLOWLIST += vck190 zc 

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/xcl2
HOST_SRCS += $(XF_PROJ_ROOT)/common/includes/xcl2/xcl2.cpp ./src/host.cpp 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 
############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += -t $(TARGET) --platform $(PLATFORM) --save-temps 


# Kernel linker flags
VPP_LDFLAGS_increment += --config ./krnl_incr.cfg
EXECUTABLE = ./external_io_py
EMCONFIG_DIR = $(TEMP_DIR)

############################## Setting Targets ##############################
.PHONY: all clean cleanall docs emconfig
all: check-platform check-device $(EXECUTABLE) $(BUILD_DIR)/increment.xclbin emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: check-vitis check-device $(BUILD_DIR)/increment.xclbin

.PHONY: xclbin
xclbin: build

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/increment.xo: src/increment.cpp
	mkdir -p $(TEMP_DIR)
	v++ $(VPP_FLAGS) -c -k increment --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'

$(BUILD_DIR)/increment.xclbin: $(TEMP_DIR)/increment.xo
	mkdir -p $(BUILD_DIR)
	v++ $(VPP_FLAGS) -l $(VPP_LDFLAGS) $(XOS) --temp_dir $(TEMP_DIR) $(VPP_LDFLAGS_increment) -o'$(LINK_OUTPUT)' $(+)
	v++ -p $(LINK_OUTPUT) $(VPP_FLAGS) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/increment.xclbin

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): $(HOST_SRCS) | check-xrt
		$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
run: run_blocking run_non_blocking

run_blocking: all
	$(ECHO) "Feeding the kernel with a Blocking External Traffic Generator"
	./scripts/env_setup.sh
	./scripts/launch_master_slave.sh BLOCKING &
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	cp -rf $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
	./scripts/check_result.sh BLOCKING
endif
ifneq ($(TARGET),$(findstring $(TARGET), sw_emu hw_emu))
$(error Application supports only sw_emu hw_emu TARGET. Please use the target for running the application)
endif
	
run_non_blocking: all
	$(ECHO) "Feeding the kernel with a Non-blocking External Traffic Generator"
	./scripts/env_setup.sh
	./scripts/launch_master_slave.sh NON_BLOCKING &
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	cp -rf $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
	./scripts/check_result.sh NON_BLOCKING
endif
ifneq ($(TARGET),$(findstring $(TARGET), sw_emu hw_emu))
$(error Application supports only sw_emu hw_emu TARGET. Please use the target for running the application)
endif

.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
endif
ifneq ($(TARGET),$(findstring $(TARGET), sw_emu hw_emu))
$(warning WARNING:Application supports only sw_emu hw_emu TARGET. Please use the target for running the application)
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv *.txt
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir* sd_card*
	-$(RMDIR) package.*
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin


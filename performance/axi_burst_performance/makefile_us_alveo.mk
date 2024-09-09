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
	$(ECHO) "  make all TARGET=<hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<hw_emu/hw> PLATFORM=<FPGA platform>" 
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host"
	$(ECHO) "      Command to build host application."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

endif

TARGET := hw
VPP_LDFLAGS :=
include ./utils.mk

TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)

PACKAGE_OUT = ./package.$(TARGET)

VPP := v++
VPP_PFLAGS := 
CMD_ARGS = -x1 $(BUILD_DIR)/test_kernel_maxi_256bit.xclbin -x2 $(BUILD_DIR)/test_kernel_maxi_512bit.xclbin

CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -Wall -O0 -g -std=c++1y
LDFLAGS += -L$(XILINX_XRT)/lib -pthread -lOpenCL

########################## Checking if PLATFORM in allowlist #######################
PLATFORM_BLOCKLIST += zc vck aws-vu9p-f1 samsung u2_ nodma v70

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/xcl2
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/cmdparser
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/logger
HOST_SRCS += $(XF_PROJ_ROOT)/common/includes/xcl2/xcl2.cpp $(XF_PROJ_ROOT)/common/includes/cmdparser/cmdlineparser.cpp $(XF_PROJ_ROOT)/common/includes/logger/logger.cpp ./src/host.cpp 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += --save-temps 

EXECUTABLE = ./axi_burst_performance
EMCONFIG_DIR = $(TEMP_DIR)

############################## Declaring Binary Containers ##############################
BINARY_CONTAINERS += $(BUILD_DIR)/test_kernel_maxi_256bit.xclbin
BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_256bit_1.xo
BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_256bit_2.xo
BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_256bit_3.xo
BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_256bit_4.xo
BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_256bit_5.xo
BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_256bit_6.xo
BINARY_CONTAINERS += $(BUILD_DIR)/test_kernel_maxi_512bit.xclbin
BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_512bit_1.xo
BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_512bit_2.xo
BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_512bit_3.xo
BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_512bit_4.xo
BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_512bit_5.xo
BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS += $(TEMP_DIR)/test_kernel_maxi_512bit_6.xo

############################## Setting Targets ##############################
CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-platform check-device check-vitis $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: check-vitis check-device $(BINARY_CONTAINERS)

.PHONY: xclbin
xclbin: build

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/test_kernel_maxi_256bit_1.xo: src/test_kernel_maxi_256bit_1.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_256bit_1 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_256bit_2.xo: src/test_kernel_maxi_256bit_2.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_256bit_2 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_256bit_3.xo: src/test_kernel_maxi_256bit_3.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_256bit_3 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_256bit_4.xo: src/test_kernel_maxi_256bit_4.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_256bit_4 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_256bit_5.xo: src/test_kernel_maxi_256bit_5.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_256bit_5 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_256bit_6.xo: src/test_kernel_maxi_256bit_6.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_256bit_6 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_512bit_1.xo: src/test_kernel_maxi_512bit_1.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_512bit_1 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_512bit_2.xo: src/test_kernel_maxi_512bit_2.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_512bit_2 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_512bit_3.xo: src/test_kernel_maxi_512bit_3.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_512bit_3 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_512bit_4.xo: src/test_kernel_maxi_512bit_4.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_512bit_4 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_512bit_5.xo: src/test_kernel_maxi_512bit_5.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_512bit_5 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/test_kernel_maxi_512bit_6.xo: src/test_kernel_maxi_512bit_6.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k test_kernel_maxi_512bit_6 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/test_kernel_maxi_256bit.xclbin: $(BINARY_CONTAINER_test_kernel_maxi_256bit_OBJS)
	mkdir -p $(BUILD_DIR)
	$(VPP) -l $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) $(VPP_LDFLAGS) --temp_dir $(TEMP_DIR)  -o'$(BUILD_DIR)/test_kernel_maxi_256bit.link.xclbin' $(+)
	$(VPP) -p $(BUILD_DIR)/test_kernel_maxi_256bit.link.xclbin $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/test_kernel_maxi_256bit.xclbin

$(BUILD_DIR)/test_kernel_maxi_512bit.xclbin: $(BINARY_CONTAINER_test_kernel_maxi_512bit_OBJS)
	mkdir -p $(BUILD_DIR)
	$(VPP) -l $(VPP_FLAGS) $(VPP_LDFLAGS) -t $(TARGET) --platform $(PLATFORM) --temp_dir $(TEMP_DIR)  -o'$(BUILD_DIR)/test_kernel_maxi_512bit.link.xclbin' $(+)
	$(VPP) -p $(BUILD_DIR)/test_kernel_maxi_512bit.link.xclbin -t $(TARGET) --platform $(PLATFORM) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/test_kernel_maxi_512bit.xclbin

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): $(HOST_SRCS) | check-xrt
		g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif


ifneq ($(TARGET),$(findstring $(TARGET), hw hw_emu))
$(error Software Emulation is not supported for the examples and will be deprecated in the tool in 2024.2. Please run hw_emu or hw target.)
endif

.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif


ifneq ($(TARGET),$(findstring $(TARGET), hw hw_emu))
$(warning WARNING:Software Emulation is not supported for the examples and will be deprecated in the tool in 2024.2. Please run hw_emu or hw target.)
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) *.xclbin/{*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir*
	-$(RMDIR) package.*
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin


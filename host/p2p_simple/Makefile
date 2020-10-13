############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO)  "  make test DEVICE=<FPGA platform>"
	$(ECHO)  "     Command to run the application. This is same as 'run' target but does not have any makefile dependency."
	$(ECHO)  ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make host HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build host application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""

############################## Setting up Project Variables ##############################
# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%host/p2p_simple/*}')
PWD = $(shell readlink -f .)
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))

TARGET := hw
HOST_ARCH := x86
SYSROOT := 

include ./utils.mk

XSA := $(call device2xsa, $(DEVICE))
TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)

# SoC variables
RUN_APP_SCRIPT = ./run_app.sh
PACKAGE_OUT = ./package.$(TARGET)

LAUNCH_EMULATOR = $(PACKAGE_OUT)/launch_$(TARGET).sh
RESULT_STRING = TEST PASSED

VPP := v++

include $(ABS_COMMON_REPO)/common/includes/opencl/opencl.mk
CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++11
LDFLAGS += $(opencl_LDFLAGS)

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
CXXFLAGS += -I$(ABS_COMMON_REPO)/common/includes/xcl2
CXXFLAGS += -I$(ABS_COMMON_REPO)/common/includes/cmdparser
CXXFLAGS += -I$(ABS_COMMON_REPO)/common/includes/logger
HOST_SRCS += $(ABS_COMMON_REPO)/common/includes/xcl2/xcl2.cpp $(ABS_COMMON_REPO)/common/includes/cmdparser/cmdlineparser.cpp $(ABS_COMMON_REPO)/common/includes/logger/logger.cpp ./src/host.cpp 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

ifneq ($(HOST_ARCH), x86)
	LDFLAGS += --sysroot=$(SYSROOT)
endif

############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
CLFLAGS += -t $(TARGET) --platform $(DEVICE) --save-temps 
ifneq ($(TARGET), hw)
	CLFLAGS += -g
endif


# Kernel linker flags
LDCLFLAGS += --config ./adder.ini

EXECUTABLE = ./p2p_simple
CMD_ARGS = -x $(BUILD_DIR)/adder.xclbin -i /dev/nvme0n1
EMCONFIG_DIR = $(TEMP_DIR)

############################## Declaring Binary Containers ##############################
BINARY_CONTAINERS += $(BUILD_DIR)/adder.xclbin
BINARY_CONTAINER_adder_OBJS += $(TEMP_DIR)/adder.xo

############################## Setting Targets ##############################
CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: check-vitis $(BINARY_CONTAINERS)

.PHONY: xclbin
xclbin: build

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/adder.xo: src/adder.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) -c -k adder $(CLFLAGS) --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/adder.xclbin: $(BINARY_CONTAINER_adder_OBJS)
	mkdir -p $(BUILD_DIR)
ifeq ($(HOST_ARCH), x86)
	$(VPP) -l $(LDCLFLAGS) $(CLFLAGS) --temp_dir $(BUILD_DIR)  -o'$(BUILD_DIR)/adder.link.xclbin' $(+)
	$(VPP) -p $(BUILD_DIR)/adder.link.xclbin -t $(TARGET) --platform $(DEVICE) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/adder.xclbin
else
	$(VPP) -l $(LDCLFLAGS) $(CLFLAGS) --temp_dir $(BUILD_DIR) -o'$(BUILD_DIR)/adder.xclbin' $(+)
endif

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): $(HOST_SRCS) | check-xrt
		$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(findstring zc, $(DEVICE)), zc)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring _u25_, $(DEVICE)), _u25_)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring u200, $(DEVICE)), u200)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring u250, $(DEVICE)), u250)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring u280, $(DEVICE)), u280)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring u50, $(DEVICE)), u50)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring vck, $(DEVICE)), vck)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring aws, $(DEVICE)), aws)
$(error This example is not supported for $(DEVICE))
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) -x $(BUILD_DIR)/adder.xclbin -i /dev/nvme0n1
else
	$(EXECUTABLE) -x $(BUILD_DIR)/adder.xclbin -i /dev/nvme0n1
endif


.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) -x $(BUILD_DIR)/adder.xclbin -i /dev/nvme0n1
else
	$(EXECUTABLE) -x $(BUILD_DIR)/adder.xclbin -i /dev/nvme0n1
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


.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> SYSROOT=<sysroot_path>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and SYSROOT is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make sd_card TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> SYSROOT=<sysroot_path>"
	$(ECHO) "      Command to prepare sd_card files for system run."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and SYSROOT is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make check TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> SYSROOT=<sysroot_path>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and SYSROOT is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> SYSROOT=<sysroot_path>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and SYSROOT is required for SoC shells"
	$(ECHO) ""

# Points to Utility Directory
COMMON_REPO = ../../
PWD = $(shell readlink -f .)
ABS_COMMON_REPO = $(shell readlink -f $(COMMON_REPO))

TARGET := hw
HOST_ARCH := x86
SYSROOT := 

include ./utils.mk

XSA := $(call device2xsa, $(DEVICE))
TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)
B_NAME := $(shell dirname $(DEVICE))

ifeq ($(HOST_ARCH), x86)
	CXX := $(XILINX_VITIS)/bin/xcpp
else ifeq ($(HOST_ARCH), aarch64)
	CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
else ifeq ($(HOST_ARCH), aarch32)
	CXX := $(XILINX_VITIS)/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++
endif

VPP := v++

#Include Libraries
include $(ABS_COMMON_REPO)/common/includes/opencl/opencl.mk
include $(ABS_COMMON_REPO)/common/includes/xcl2/xcl2.mk
CXXFLAGS += $(xcl2_CXXFLAGS)
LDFLAGS += $(xcl2_LDFLAGS)
HOST_SRCS += $(xcl2_SRCS)
CXXFLAGS += -pthread
CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++14
LDFLAGS += $(opencl_LDFLAGS)

HOST_SRCS += src/host.cpp 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

ifneq ($(HOST_ARCH), x86)
	LDFLAGS += --sysroot=$(SYSROOT)
endif

# Kernel compiler global settings
CLFLAGS += -t $(TARGET) --platform $(DEVICE) --save-temps 



# Adding config files to linker
LDCLFLAGS += --config krnl_stream_vadd_vmult.ini 
EXECUTABLE = vadd_stream
CMD_ARGS = $(BUILD_DIR)/krnl_stream_vadd_vmult.xclbin
EXEC_CMD_ARGS += $(EXECUTABLE)
EXEC_CMD_ARGS += krnl_stream_vadd_vmult.xclbin

EMCONFIG_DIR = $(TEMP_DIR)

BINARY_CONTAINERS += $(BUILD_DIR)/krnl_stream_vadd_vmult.xclbin
BINARY_CONTAINER_krnl_stream_vadd_vmult_OBJS += $(TEMP_DIR)/krnl_stream_vadd.xo
BINARY_CONTAINER_krnl_stream_vadd_vmult_OBJS += $(TEMP_DIR)/krnl_stream_vmult.xo

CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

.PHONY: build
build: $(BINARY_CONTAINERS)

# Building kernel
$(TEMP_DIR)/krnl_stream_vadd.xo: src/krnl_stream_vadd.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) $(CLFLAGS) --temp_dir $(TEMP_DIR) -c -k krnl_stream_vadd -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/krnl_stream_vmult.xo: src/krnl_stream_vmult.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) $(CLFLAGS) --temp_dir $(TEMP_DIR) -c -k krnl_stream_vmult -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/krnl_stream_vadd_vmult.xclbin: $(BINARY_CONTAINER_krnl_stream_vadd_vmult_OBJS)
	mkdir -p $(BUILD_DIR)
	$(VPP) $(CLFLAGS) --temp_dir $(BUILD_DIR) -l $(LDCLFLAGS) -o'$@' $(+)

# Building Host
$(EXECUTABLE): check-xrt $(HOST_SRCS) $(HOST_HDRS)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

check: all
ifeq ($(findstring xilinx_u200_xdma, $(DEVICE)), xilinx_u200_xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring zc, $(DEVICE)), zc)
$(error Nothing to be done for make)
endif
ifeq ($(findstring zed, $(DEVICE)), zed)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx_u250_xdma, $(DEVICE)), xilinx_u250_xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx_u280_xdma, $(DEVICE)), xilinx_u280_xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx_u280-es1_xdma, $(DEVICE)), xilinx_u280-es1_xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx_aws-vu9p-f1-04261818, $(DEVICE)), xilinx_aws-vu9p-f1-04261818)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx:u200:xdma, $(DEVICE)), xilinx:u200:xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx:u250:xdma, $(DEVICE)), xilinx:u250:xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx:u280:xdma, $(DEVICE)), xilinx:u280:xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx:u280-es1:xdma, $(DEVICE)), xilinx:u280-es1:xdma)
$(error Nothing to be done for make)
endif
ifeq ($(findstring xilinx:aws-vu9p-f1-04261818:dynamic, $(DEVICE)), xilinx:aws-vu9p-f1-04261818:dynamic)
$(error Nothing to be done for make)
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
ifeq ($(HOST_ARCH), x86)
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(BUILD_DIR)/krnl_stream_vadd_vmult.xclbin
else
	$(CP) $(BINARY_CONTAINERS) .
	readlink -f $(EXECUTABLE) >> _vimage/emulation/sd_card.manifest
	readlink -f xrt.ini >> _vimage/emulation/sd_card.manifest
	readlink -f $(BUILD_DIR) >> _vimage/emulation/sd_card.manifest
	$(ECHO) ./$(EXEC_CMD_ARGS) >> _vimage/emulation/init.sh
	$(ECHO) "reboot" >> _vimage/emulation/init.sh
	launch_emulator -no-reboot -runtime ocl -t $(TARGET)
	head -n -3 _vimage/emulation/sd_card.manifest | tee _vimage/emulation/sd_card.manifest
	head -n -2 _vimage/emulation/init.sh | tee _vimage/emulation/init.sh
endif
else
	 ./$(EXECUTABLE) $(BUILD_DIR)/krnl_stream_vadd_vmult.xclbin
endif
ifeq ($(HOST_ARCH), x86)
	perf_analyze profile -i profile_summary.csv -f html
endif

sd_card: all
	mkdir -p sd_card
	$(CP) $(B_NAME)/sw/$(XSA)/xrt/image/* $(BUILD_DIR)/*.xclbin $(EXECUTABLE) sd_card/

# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll _xocc_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir* sd_card
	-$(RMDIR) _x.* xclbin.run_summary qemu-memory-_* emulation/ _vimage/ pl* start_simulation.sh *.xclbin


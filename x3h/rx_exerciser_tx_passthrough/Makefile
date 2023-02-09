# Copyright 2019-2022 Xilinx, Inc.
# Copyright 2022-2023 Advanced Micro Devices, Inc.
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
# vitis makefile-generator v2.0.6

############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<hw/hw_emu> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<hw/hw_emu> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make run_frame TARGET=<hw/hw_emu> PLATFORM=<FPGA platform> "
	$(ECHO) "      Command to run application in emulation for Frame Based NTG APIs"
	$(ECHO) ""
	$(ECHO) "  make run_pcap TARGET=<hw/hw_emu> PLATFORM=<FPGA platform> "
	$(ECHO) "      Command to run application in emulation for PCAP Based NTG APIs"
	$(ECHO) ""
	$(ECHO) "  make xclbin TARGET=<hw/hw_emu> PLATFORM=<FPGA platform> "
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host TARGET=<hw/hw_emu>"
	$(ECHO) "      Command to build host application."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

############################## Setting up Project Variables ##############################

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))

# setting devault value
TARGET ?= hw_emu
HOST_ARCH := x86

DEVICE ?= xilinx_x3522p_gen4x8_hn10g_1_202220_1
#setting PLATFORM
ifeq ($(PLATFORM),)
PLATFORM := $(DEVICE)
endif
ifeq ($(PLATFORM),)
PLATFORM ?= xilinx_x3522p_gen4x8_hn10g_1_202220_1
endif

# #################### Checking if PLATFORM in whitelist ############################

include ./utils.mk
TEMP_DIR := _x_temp.$(TARGET).$(PLATFORM_NAME)
TEMP_REPORT_DIR := $(CUR_DIR)/reports/_x.$(TARGET).$(PLATFORM_NAME)
BUILD_DIR := build_dir.$(TARGET).$(PLATFORM_NAME)
ifneq ($(RESULT_DIR),)
BUILD_DIR = $(RESULT_DIR)
endif
BUILD_REPORT_DIR := $(CUR_DIR)/reports/_build.$(TARGET).$(PLATFORM_NAME)
EMCONFIG := $(BUILD_DIR)/emconfig.json
XCLBIN_DIR := $(CUR_DIR)/$(BUILD_DIR)
export XCL_BINDIR = $(XCLBIN_DIR)

EXE_FILE_DEPS :=
BINARY_CONTAINERS_DEPS :=
RUN_DEPS :=

# get global setting
CXXFLAGS += -fmessage-length=0 -I$(CUR_DIR)/src/ -I$(XILINX_XRT)/include -I$(XILINX_HLS)/include -std=c++14 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label 
LDFLAGS += -pthread -L$(XILINX_XRT)/lib -L$(XILINX_HLS)/lnx64/tools/fpo_v7_0  -Wl,--as-needed -lOpenCL -lxrt_coreutil 
VPP_FLAGS += -t $(TARGET) --platform $(XPLATFORM) --save-temps 
VPP_LDFLAGS += --optimize 2 -R 2 
CXXFLAGS += $(EXTRA_CXXFLAGS)
VPP_FLAGS += $(EXTRA_VPP_FLAGS)

########################## Setting up Host Variables ##########################
ifneq ($(TARGET), hw)
	VPP_FLAGS += -g
endif

ifeq (,$(findstring opencv,$(CXXFLAGS)))
CXXFLAGS += $(XRT_CXXFLAGS)
endif

HOST_SRCS += $(CUR_DIR)/src/host.cpp  
CXXFLAGS +=  -I $(SYSROOT)/usr/include  
CXXFLAGS += --sysroot=$(SYSROOT)
LDFLAGS +=  -L $(SYSROOT)/usr/lib -L ${SYSROOT}/opt/xilinx/xrt/lib

EXE_NAME := x3_exerciser_loop_rx
EXE_FILE := $(BUILD_DIR)/$(EXE_NAME)
EXE_FILE_DEPS := $(HOST_SRCS) $(EXE_FILE_DEPS)

########################## Kernel compiler global settings ##########################
VPP_FLAGS +=  -D NO_SYNTH

######################### binary container global settings ##########################
VPP_FLAGS_packet_passthrough_tx_v1 += --hls.clock 500000000:packet_passthrough_tx_v1
VPP_LDFLAGS_exerciser_loop_rx_temp := --config $(CUR_DIR)/connectivity.cfg
VPP_LDFLAGS_exerciser_loop_rx += $(VPP_LDFLAGS_exerciser_loop_rx_temp)
VPP_LDFLAGS += --advanced.param=compiler.worstNegativeSlack=-0.054

BINARY_CONTAINERS += $(BUILD_DIR)/exerciser_loop_rx.xclbin

# ################ Setting Rules for Binary Containers (Building Kernels) ################
$(TEMP_DIR)/packet_passthrough_tx_v1.xo: $(CUR_DIR)/src/packet_passthrough_tx_v1.cpp 
	$(ECHO) "Compiling Kernel: packet_passthrough_tx_v1"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_packet_passthrough_tx_v1) $(VPP_FLAGS) -k packet_passthrough_tx_v1 -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
BINARY_CONTAINER_exerciser_loop_rx_OBJS += $(TEMP_DIR)/packet_passthrough_tx_v1.xo
$(TEMP_DIR)/exerciser_rx.xo: $(CUR_DIR)/src/exerciser_rx.cpp 
	$(ECHO) "Compiling Kernel: exerciser_rx"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_exerciser_rx) $(VPP_FLAGS) -k exerciser_rx -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
BINARY_CONTAINER_exerciser_loop_rx_OBJS += $(TEMP_DIR)/exerciser_rx.xo
$(TEMP_DIR)/exerciser_config_rx.xo: $(CUR_DIR)/src/exerciser_config_rx.cpp 
	$(ECHO) "Compiling Kernel: exerciser_config_rx"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_exerciser_config_rx) $(VPP_FLAGS) -k exerciser_config_rx -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
BINARY_CONTAINER_exerciser_loop_rx_OBJS += $(TEMP_DIR)/exerciser_config_rx.xo
BINARY_CONTAINERS_DEPS += $(BINARY_CONTAINER_exerciser_loop_rx_OBJS)
$(BINARY_CONTAINERS): $(BINARY_CONTAINERS_DEPS)
	mkdir -p $(BUILD_DIR)
	$(VPP) -l $(VPP_FLAGS) --temp_dir $(TEMP_DIR) --report_dir $(BUILD_REPORT_DIR)/exerciser_loop_rx $(VPP_LDFLAGS)  $(VPP_LDFLAGS_exerciser_loop_rx) $(AIE_LDFLAGS)   -o $@ $^

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXE_FILE): $(EXE_FILE_DEPS) |  check_xrt
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)


$(EMCONFIG):
	emconfigutil --platform $(XPLATFORM) --od $(BUILD_DIR)
############################## Setting Essential Checks and Building Rules ##############################
RUN_DEPS += $(EXE_FILE) $(BINARY_CONTAINERS) $(EMCONFIG)
RUN_DEPS += $(SD_CARD)

.PHONY: mkflag all run
mkflag:
	mkdir -p $(BUILD_DIR)
	@for var in $(MAKEFLAGS); do echo $$var >> $(BUILD_DIR)/makefile_args.txt; done
all: check_vpp check_platform mkflag $(RUN_DEPS)
run: all 
	make run_frame
	make run_pcap

run_frame: all
	$(ECHO) "Running Frame API based Emulation"
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
					XCL_EMULATION_MODE=$(TARGET) $(EXE_FILE) $(BUILD_DIR)/exerciser_loop_rx.xclbin ./packet_gen_python/packetGen.py
run_pcap: all
	$(ECHO) "Running PCAP API based Emulation"
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
					XCL_EMULATION_MODE=$(TARGET) $(EXE_FILE) $(BUILD_DIR)/exerciser_loop_rx.xclbin ./packet_gen_python/pcapRun.py


############################## Setting Targets ##############################

.PHONY: clean cleanall emconfig
emconfig: $(EMCONFIG)

.PHONY: host
host:  check_xrt $(EXE_FILE)

.PHONY: xclbin
xclbin:  check_vpp check_xrt $(BINARY_CONTAINERS) 
############################## Cleaning Rules ##############################
cleanh:
	-$(RMDIR) $(EXE_FILE) vitis_* TempConfig system_estimate.xtxt *.rpt .run/
	-$(RMDIR) src/*.ll _xocc_* .Xil dltmp* xmltmp* *.log *.jou *.wcfg *.wdb sample_link.ini sample_compile.ini obj*  bin* *.csv *.jpg *.jpeg *.png

cleank:
	-$(RMDIR) $(BUILD_DIR)/*.xclbin _vimage *xclbin.run_summary qemu-memory-_* emulation/ _vimage/ pl*start_simulation. sh *.xclbin
	-$(RMDIR) _x_temp.*

cleanall: cleanh cleank
	-$(RMDIR) $(BUILD_DIR)  emconfig.json *.html $(TEMP_DIR) $(CUR_DIR)/reports *.csv *.run_summary  $(CUR_DIR)/*.raw package_*   $(BUILD_DIR)/run_script.sh .ipcache *.str
	-$(RMDIR)  $(CUR_DIR)/Work $(CUR_DIR)/*.xpe $(CUR_DIR)/hw.o $(CUR_DIR)/*.xsa $(CUR_DIR)/xnwOut
	-$(RMDIR) 

clean: cleanh

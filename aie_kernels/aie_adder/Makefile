#
# Copyright 2019-2020 Xilinx, Inc.
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
	$(ECHO) "  make sd_card TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to prepare sd_card files."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
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
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%aie_kernels/aie_adder/*}')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

# Makefile input options
TARGET := hw_emu
HOST_ARCH := x86
XCLBIN := adder.xclbin

# File names and locations
GRAPH := src/aie_graph.cpp
GRAPH_O := libadf.a

KERNEL := pl_s2mm.cpp pl_mm2s.cpp 
KERNEL_XO := pl_s2mm.xo pl_mm2s.xo 

HOST_SRCS := ./src/host.cpp
EXECUTABLE = ./aie_adder

# SoC variables
RUN_APP_SCRIPT = ./run_app.sh
PACKAGE_OUT = ./package.$(TARGET)

LAUNCH_EMULATOR = $(PACKAGE_OUT)/launch_$(TARGET).sh
RESULT_STRING = TEST PASSED

CONFIG_FILE := system.cfg
include ./utils.mk

ifeq ($(findstring zc, $(DEVICE)), zc)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring vck5000, $(DEVICE)), vck5000)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring aws-vu9p-f1, $(DEVICE)), aws-vu9p-f1)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring samsung, $(DEVICE)), samsung)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring _u2_, $(DEVICE)), _u2_)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif
ifeq ($(findstring dma, $(DEVICE)), dma)
$(error [ERROR]: This example is not supported for $(DEVICE).)
endif

ifneq ($(findstring vck190, $(DEVICE)), vck190)
$(warning [WARNING]: This example has not been tested for $(DEVICE). It may or may not work.)
endif

# Command-line options
VPP := v++
VPP_PFLAGS := 
AIECC := aiecompiler
AIESIM := aiesimulator

AIE_INCLUDE_FLAGS := -include="$(XILINX_VITIS)/aietools/include" -include="./src" -include="./data" -include="./"
AIE_FLAGS := --target=hw $(AIE_INCLUDE_FLAGS) --pl-freq=100 -workdir=./Work 

VPP_XO_FLAGS := -c --platform $(DEVICE) --save-temps -g
VPP_LINK_FLAGS := -l --platform $(DEVICE) $(KERNEL_XO) $(GRAPH_O) -t $(TARGET) --save-temps -g --config $(CONFIG_FILE) -o $(XCLBIN)

GCC_FLAGS := -Wall -c \
			 -std=c++14 \
			 -Wno-int-to-pointer-cast \
			 --sysroot=$(SYSROOT) \

GCC_INCLUDES := -I$(SYSROOT)/usr/include/xrt \
				-I$(SYSROOT)/usr/include \
				-I./  \
				-I${XILINX_VITIS}/aietools/include \
				-I${XILINX_VITIS}/include

GCC_LIB := -lxaiengine -ladf_api_xrt -lxrt_core -lxrt_coreutil \
		   -L$(SYSROOT)/usr/lib \
		   --sysroot=$(SYSROOT) \
		   -L${XILINX_VITIS}/aietools/lib/aarch64.o

.PHONY: all clean cleanall docs emconfig
all: kernels graph build host sd_card 

######################################################
# This step compiles the HLS C kernels and creates an ADF Graph
# the %.xo is used as the output and creates from the %.cpp files
# The graph is generated by having the -target=hw
kernels: $(KERNEL_XO)
	@echo "COMPLETE: Kernels Created."	

pl_s2mm.xo: src/pl_s2mm.cpp
	$(VPP) $(VPP_XO_FLAGS) -k $(basename $(notdir $<)) $< -o $@
pl_mm2s.xo: src/pl_mm2s.cpp
	$(VPP) $(VPP_XO_FLAGS) -k $(basename $(notdir $<)) $< -o $@

graph: $(GRAPH_O)

aiesim: $(GRAPH_O)
	$(AIESIM) --pkg-dir=./Work 

$(GRAPH_O): $(GRAPH)
	$(AIECC) $(AIE_FLAGS) $(GRAPH)
	@echo "COMPLETE: libadf.a created."
#####################################################

########################################################
# Once the kernels and graph are generated, you can build
# the hardware part of the design. This creates an xclbin
# that will be used to run the design on the platform.
build: $(GRAPH_O) $(KERNEL_XO)
	$(VPP) $(VPP_LINK_FLAGS) || (echo "task: [xclbin] failed error code: $$?"; exit 1)
	@echo "COMPLETE: .xclbin created."
########################################################

############################################################################################################################
# For hardware and hardware emulation, compile the PS code and generate the aie_adder executable. This is needed for creating the sd_card.
host: 
	$(CXX) $(GCC_FLAGS) $(GCC_INCLUDES) -o aie_control_xrt.o ./Work/ps/c_rts/aie_control_xrt.cpp
	$(CXX) $(HOST_SRCS) $(GCC_FLAGS) $(GCC_INCLUDES) -o main.o
	$(CXX) *.o $(GCC_LIB) -o $(EXECUTABLE)
	@echo "COMPLETE: Host application created."
############################################################################################################################

##################################################################################################
# Depending on the TARGET, it'll either generate the PDI for hw_emu or hw.
sd_card: $(GRAPH_O) $(KERNEL_XO) host gen_run_app 
ifeq (${TARGET},hw_emu)
	v++ $(VPP_PFLAGS) -p -t hw_emu \
		--platform $(DEVICE) \
		--package.out_dir $(PACKAGE_OUT) \
		--package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4 \
		--package.image_format=ext4 \
		--package.boot_mode=sd \
		--package.kernel_image $(SD_IMAGE_FILE) \
		--package.defer_aie_run \
		--package.sd_dir /proj/xbuilds/2021.1_daily_latest/internal_platforms/sw/versal/xrt \
		--package.sd_file $(RUN_APP_SCRIPT) \
		--package.sd_file aie_adder $(XCLBIN) $(GRAPH_O) -o krnl_adder.xclbin
	@echo "COMPLETE: hw_emu package created."
else
	v++ $(VPP_PFLAGS) -p -t hw \
		--platform $(DEVICE) \
		--package.out_dir $(PACKAGE_OUT) \
		--package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4 \
		--package.image_format=ext4 \
		--package.boot_mode=sd \
		--package.kernel_image=$(SD_IMAGE_FILE) \
		--package.defer_aie_run \
		--package.sd_dir /proj/xbuilds/2021.1_daily_latest/internal_platforms/sw/versal/xrt \
		--package.sd_file $(RUN_APP_SCRIPT) \
		--package.sd_file aie_adder $(XCLBIN) $(GRAPH_O) -o krnl_adder.xclbin
	@echo "COMPLETE: hw package created."
endif
###################################################################################################

###########################################################################
# If the target is for HW_EMU, launch the emulator
# If the target is for HW, you'll have to follow the Confluence page for
# running on a board farm system.
run: all 
ifeq (${TARGET},hw_emu)
	$(LAUNCH_EMULATOR) -run-app $(RUN_APP_SCRIPT) | tee run_app.log; exit $${PIPESTATUS[0]}
else
	@echo "Hardware build, no emulation executed."
endif
ifneq ($(TARGET),$(findstring $(TARGET), hw_emu hw))
$(error Application supports only hw_emu hw TARGET. Please use the target for running the application)
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv *.o *.xo *.xpe *.xsa cfg qemu_dts_files emu_qemu_scripts *.db sim *.sh *.a 
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb *bin* *summary* *.BIN *.bif *.exe Work *.log *.txt 

cleanall: clean
	-$(RMDIR) build_dir* sd_card*
	-$(RMDIR) package.* run_app.sh 
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin

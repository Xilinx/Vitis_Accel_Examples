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
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<hw_emu> PLATFORM=<FPGA platform> HOST_ARCH=<> EDGE_COMMON_SW=<>"
	$(ECHO) "      Command to generate the baremetal system for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<hw_emu> PLATFORM=<FPGA platform> HOST_ARCH=<> EDGE_COMMON_SW=<>"
	$(ECHO) "      Command to run the user application."
	$(ECHO) ""
	$(ECHO) "  make baremetal_elf TARGET=<hw_emu> PLATFORM=<FPGA platform> EDGE_COMMON_SW=<>"
	$(ECHO) "      Command to build the bsp sources and libraries required for compilation of user application."
	$(ECHO) ""
	$(ECHO) "  make fixed_xsa TARGET=<hw_emu> PLATFORM=<FPGA platform> EDGE_COMMON_SW=<>"
	$(ECHO) "      Command to build the fixed xsa from v++ link with an extensible platform."
	$(ECHO) ""



############################## Setting up Project Variables ##############################
# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $$(MK_PATH%emulation/aie_baremetal/*)')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

# Makefile input options
TARGET := hw_emu
FIXED_XSA := vck190_fixed.xsa
APP_NAME := vck190_fixed
WORKSPACE_FIXED := workspace_fixed
include ./utils.mk

# File names and locations
GRAPH := aie/graph.cpp
GRAPH_O := libadf.a
AIE_FILE := Work/ps/c_rts/aie_control.cpp

PL_O = polar_clip.xo s2mm.xo mm2s.xo

TCL := tcl/baremetal_bsp.tcl

HOST_SRC := ps/main.cpp
HOST_OBJ := main.o aie_control.o 
EXECUTABLE := main.elf
HOST_INC := -I./aie -I./

# SoC variables
PACKAGE_OUT := ./package.$(TARGET)
LAUNCH_EMULATOR := $(PACKAGE_OUT)/launch_$(TARGET).sh

CONFIG_FILE := system.cfg

########################## Checking if PLATFORM in whitelist #######################
PLATFORM_BLOCKLIST += samsung u2_ zc nodma  
PLATFORM_ALLOWLIST += vck190

# Command-line options
VPP := v++
AIECC := aiecompiler

AIE_INCLUDE_FLAGS := -include="$(XILINX_VITIS)/aietools/include" -include="./aie" -include="./data" -include="./" -include="./aie/kernels"
AIE_FLAGS := $(AIE_INCLUDE_FLAGS) --pl-freq=100 -workdir=./Work

VPP_FLAGS := -t $(TARGET) --platform $(PLATFORM) --save-temps -g
 
GCC_FLAGS := -g -c -D__PS_BARE_METAL__ -std=c++14 

GCC_INCLUDES := -I./ -I./aie \
				-I${XILINX_VITIS}/aietools/include \
				-I${XILINX_VITIS}/include \
				-I./workspace_fixed/standalone/src/ \
				-I./standalone_domain/bspinclude/include

GCC_LIB := -ladf_api \
		   -L${XILINX_VITIS}/aietools/lib/aarchnone64.o \
		   -L./standalone_domain/bsplib/lib 

.PHONY: all clean cleanall

############################## Setting up Host Variables ##############################

# Host compiler global settings
CXXFLAGS += -Wall -O0 -g3 -fmessage-length=0
LDFLAGS += --start-group,-lxil,-lgcc,-lc,-lstdc++,--end-group 
HOST_CXX = $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-none/bin/aarch64-none-elf-gcc

############################## Setting Targets ##############################
CP := cp -rf 

$(GRAPH_O): $(GRAPH)
	$(AIECC) $(AIE_FLAGS) $(GRAPH)
	@echo "COMPLETE: libadf.a created."

%.xo: pl_kernels/%.cpp
	$(VPP) $(VPP_FLAGS) -c -k $(basename $(notdir $<)) $< -o $@	

$(FIXED_XSA): $(GRAPH_O) $(PL_O) 
	$(VPP) $(VPP_FLAGS) -l $(GRAPH_O) $(PL_O) --config system.cfg \
	--vivado.prop fileset.sim_1.xsim.simulate.runtime=400us -o $@

$(WORKSPACE_FIXED): $(FIXED_XSA)
	xsct $(TCL) $(APP_NAME) $(FIXED_XSA)
	$(CP) workspace_fixed/$(APP_NAME)/export/$(APP_NAME)/sw/$(APP_NAME)/standalone_domain .

main.o: $(HOST_SRC) aie_control.o 
	$(HOST_CXX) $(GCC_INCLUDES) $(HOST_INC) $(GCC_FLAGS) -o main.o $(HOST_SRC)
	
aie_control.o: $(AIE_FILE) 
	$(HOST_CXX) $(GCC_INCLUDES) $(HOST_INC) -I./aie/kernels \
	$(GCC_FLAGS) -o aie_control.o $(AIE_FILE) 

$(EXECUTABLE): $(WORKSPACE_FIXED) $(HOST_OBJ)
	$(HOST_CXX) *.o $(GCC_LIB) -mcpu=cortex-a72 \
	-Wl,-T -Wl,./ps/lscript.ld -o "$@" \
	./workspace_fixed/standalone/Debug/src/platform.o -Wl,$(LDFLAGS) 

package: $(FIXED_XSA) $(GRAPH_O)
	$(VPP) -p $(VPP_FLAGS) \
	--package.out_dir $(PACKAGE_OUT) \
	--package.domain aiengine \
	--package.boot_mode sd \
	--package.image_format=fat32 \
	--package.ps_elf $(EXECUTABLE),a72-0 $^
	
run: all
	$(LAUNCH_EMULATOR)

aie: $(GRAPH_O)

kernels: $(PL_O)

fixed_xsa: $(FIXED_XSA)

generate_bsp: $(WORKSPACE_FIXED)

baremetal_elf: $(EXECUTABLE)

all: fixed_xsa baremetal_elf package 

############################## Cleaning Rules ##############################
# Cleaning stuff
RMDIR = rm -rf
clean:
	-$(RMDIR) main.elf* standalone_domain 
	-$(RMDIR) $(EXECUTABLE) $(FIXED_XSA)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv *.o *.xo *.xpe *.xsa cfg qemu_dts_files emu_qemu_scripts *.db sim *.sh *.a 
	-$(RMDIR) sw/*.ll pl_kernels/*.ll aie/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb *bin* *summary* *.BIN *.bif *.exe Work *.log *.txt 
	
cleanall: clean 
	-$(RMDIR) workspace_fixed* fixed.xsa package.hw_emu hw_emu.xsa .Xil *.xsa
	-$(RMDIR) -rf _x *.log *.jou *.o v++.package_summary vpl_gen_fixed_presynth.pdi fixed.* 
	-$(RMDIR) package.* run_app.sh 
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage start_simulation.sh *.xclbin

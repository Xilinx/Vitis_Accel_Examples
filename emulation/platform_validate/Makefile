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
	$(ECHO) "  make fixed_xsa TARGET=<hw_emu> EXTENSIBLE_XSA=<user XSA file>"
	$(ECHO) "      Command to build the fixed xsa from v++ link with an extensible XSA."
	$(ECHO) ""
	$(ECHO) "  make baremetal_elf"
	$(ECHO) "      Command to generate standalone elf as per user application."
	$(ECHO) ""
	$(ECHO) "  make all TARGET=<hw_emu> EXTENSIBLE_XSA=<user XSA file> HOST_ARCH=<aarch64>"
	$(ECHO) "      Command to generate the baremetal system for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<hw_emu> EXTENSIBLE_XSA=<user XSA file> HOST_ARCH=<aarch64>"
	$(ECHO) "      Command to run the user application."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""




############################## Setting up Project Variables ##############################
# Points to top directory of Git repository
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
COMMON_REPO ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $$(MK_PATH%emulation/platform_validate/*)')
PWD = $(shell readlink -f .)
XF_PROJ_ROOT = $(shell readlink -f $(COMMON_REPO))

# Makefile input options
TARGET := hw_emu

EXTENSIBLE_XSA ?= $(PLATFORM_REPO_PATHS)/xilinx_vck190_base_202210_1/hw_emu/hw_emu.xsa 

APP_NAME := vck190_fixed
WORKSPACE_FIXED := workspace_fixed

FIXED_XSA := vck190_fixed.xsa
FIXED_PLATFORM := ./workspace_fixed/$(APP_NAME)/export/$(APP_NAME)/$(APP_NAME).xpfm

include ./utils.mk

# File names and locations
TCL := tcl/baremetal_bsp.tcl

HOST_SRC := src/main.cpp
HOST_OBJ := main.o 
EXECUTABLE := main.elf

# SoC variables
PACKAGE_OUT := ./package.$(TARGET)
PACKAGE_DIR := $(PACKAGE_OUT)
LAUNCH_EMULATOR := $(PACKAGE_OUT)/launch_$(TARGET).sh

########################## Checking if PLATFORM in whitelist #######################
PLATFORM_BLOCKLIST += samsung u2_ zc nodma  
PLATFORM_ALLOWLIST += vck190

# Command-line options
VPP := v++
CP := cp -rf
VPP_FLAGS := --save-temps -g 
GCC_FLAGS := -g -c -D__PS_BARE_METAL__ -std=c++14 

# Host compiler includepaths
GCC_INCLUDES := -I./ -I./aie \
				-I${XILINX_VITIS}/aietools/include \
				-I${XILINX_VITIS}/include \
				-I./workspace_fixed/standalone/src/ \
				-I./standalone_domain/bspinclude/include

# Host linker includepaths
GCC_LIB := -ladf_api \
		   -L${XILINX_VITIS}/aietools/lib/aarchnone64.o \
		   -L./standalone_domain/bsplib/lib 

.PHONY: all clean cleanall

############################## Setting up Host Variables ##############################

# Host compiler global settings
CXXFLAGS += -Wall -O0 -g3 -fmessage-length=0
HOST_CXX = $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-none/bin/aarch64-none-elf-gcc

# Host linker global settings
LDFLAGS += --start-group,-lxil,-lgcc,-lc,-lstdc++,--end-group 

############################## Setting Targets ##############################

# Fixed XSA generation
$(FIXED_XSA):  
	$(VPP) -l $(VPP_FLAGS) -t $(TARGET) --platform $(EXTENSIBLE_XSA) \
	--advanced.param compiler.addOutputTypes=hw_export \
	--vivado.prop fileset.sim_1.xsim.simulate.runtime=400us -o $@

# Fixed workspace creation with standalone domain
$(WORKSPACE_FIXED): $(FIXED_XSA)
	xsct $(TCL) $(APP_NAME) $(FIXED_XSA)
	$(CP) workspace_fixed/$(APP_NAME)/export/$(APP_NAME)/sw/$(APP_NAME)/standalone_domain .

# Host object creation  
main.o: $(WORKSPACE_FIXED)
	$(HOST_CXX) $(GCC_INCLUDES) $(HOST_INC) $(GCC_FLAGS) -o main.o $(HOST_SRC)

# Standaone ELF generation
$(EXECUTABLE): $(WORKSPACE_FIXED) $(HOST_OBJ)
	$(HOST_CXX) *.o $(GCC_LIB) -mcpu=cortex-a72 \
	-Wl,-T -Wl,workspace_fixed/standalone/src/lscript.ld -o "$@" \
	./workspace_fixed/standalone/Debug/src/platform.o -Wl,$(LDFLAGS) 

# v++ packaging using fixed platform
$(PACKAGE_DIR): $(EXECUTABLE)
	$(VPP) -p -t $(TARGET) --platform $(FIXED_PLATFORM) \
	--package.out_dir $(PACKAGE_OUT) \
	--package.boot_mode sd \
	--package.ps_elf $(EXECUTABLE),a72-0 

################################ Running Targets ################################
fixed_xsa: $(FIXED_XSA)

generate_bsp: $(WORKSPACE_FIXED)

baremetal_elf: $(EXECUTABLE)

package: $(PACKAGE_DIR)

all: fixed_xsa generate_bsp baremetal_elf package 

run: all
	$(LAUNCH_EMULATOR)

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

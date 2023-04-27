#
#Copyright 2020 Xilinx, Inc.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#    http://www.apache.org/licenses/LICENSE-2.0
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#

############################## Help Section ##############################
ifneq ($(findstring Makefile, $(MAKEFILE_LIST)), Makefile)
help:
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform> EMU_PS=<X86/QEMU> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to run application in emulation.Default sw_emu will run on x86 ,to launch on qemu specify EMU_PS=QEMU."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make sd_card TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to prepare sd_card files."
	$(ECHO) ""
	$(ECHO) "  make host TARGET=<sw_emu/hw_emu/hw> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build host application."
	$(ECHO) "      EDGE_COMMON_SW is required for SoC shells. Please download and use the pre-built image from - "
	$(ECHO) "      https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
endif

############################## Setting up Project Variables ##############################
TARGET := hw
SYSROOT := $(EDGE_COMMON_SW)/sysroots/aarch64-xilinx-linux/
SD_IMAGE_FILE := $(EDGE_COMMON_SW)/Image
CXX :=aarch64-linux-gnu-g++
PLATFORM_REPO_PATHS = /proj/xbuilds/2022.2_daily_latest/internal_platforms
PLATFORM = $(PLATFORM_REPO_PATHS)/xilinx_vck190_base_202220_1/xilinx_vck190_base_202220_1.xpfm
XPFM	 = vck190_xpfm

# Makefile input options
LINK_OUTPUT := vck190_aie_base_graph_$(TARGET).xsa

# File names and locations
GRAPH := src/aie/graph.cpp
GRAPH_O := libadf.a

HOST_SRCS := ./src/sw/host.cpp
EXECUTABLE = ./single_aie_gmio_xrt
HOST_OBJ   = aie_control_xrt.o host.o
AIE_CTRL_CPP = ../Work/ps/c_rts/aie_control_xrt.cpp

# SoC variables
RUN_APP_SCRIPT = ./run_app.sh
PACKAGE_OUT = ./package.$(TARGET)

LAUNCH_EMULATOR =$(PACKAGE_OUT)/launch_$(TARGET).sh
RESULT_STRING = TEST PASSED

CONFIG_FILE := system.cfg
include ./utils.mk

# Command-line options
VPP := v++
VPP_FLAGS=--save-temps --verbose --config ${CONFIG_FILE}
AIECC := aiecompiler
AIESIM := aiesimulator

AIE_INCLUDE_FLAGS := -include="./src/aie" -include="./"
GCC_FLAGS := 	-std=c++14 \
		-O0 -g -Wall -c \
		 -fmessage-length=0 --sysroot=$(SYSROOT)

GCC_INCLUDES :=	-I$(XILINX_VIVADO)/include/ \
		-I$(SYSROOT)/usr/include/xrt/  -I$(XILINX_VITIS)/aietools/include -I./ -I./src/aie 

GCC_LIB := -ladf_api_xrt -lgcc -lc -lxrt_coreutil -lxrt_core -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ \
		-L$(SYSROOT)/usr/lib/ \
		--sysroot=$(SYSROOT) \
		-L$(XILINX_VITIS)/aietools/lib/aarch64.o

##########################################################################################################################################################
### DO NOT MODIFY BELOW THIS LINE UNLESS NECESSARY
################################################################################################################################################

.PHONY: clean

all: $(LINK_OUTPUT) $(EXECUTABLE) package

graph: check-device $(GRAPH_O)

aiesim: $(GRAPH_O)
	$(AIESIM) --pkg-dir=./Work --dump-vcd foo

$(GRAPH_O): ./src/aie/*
	$(AIECC) $(AIE_INCLUDE_FLAGS) --platform $(PLATFORM) -workdir=./Work $(GRAPH)


xclbin: $(LINK_OUTPUT)


$(LINK_OUTPUT): $(GRAPH_O) $(CONFIG_FILE) 
	$(VPP) -g -l --platform $(PLATFORM) $(GRAPH_O) -t $(TARGET) $(VPP_FLAGS) --config $(CONFIG_FILE) -o $@

host: $(EXECUTABLE)
$(EXECUTABLE): ${GRAPH} ./Work/ps/c_rts/aie_control_xrt.cpp
		$(CXX) $(GCC_FLAGS) $(GCC_INCLUDES) -o ./aie_control_xrt.o ./Work/ps/c_rts/aie_control_xrt.cpp
		$(CXX) $(HOST_SRCS) $(GCC_FLAGS) $(GCC_INCLUDES) -o ./single_aie_gmio_xrt.o
		$(CXX) -o $(EXECUTABLE) *.o $(GCC_LIB) 


package: gen_run_app package_$(TARGET) 
package_$(TARGET): $(GRAPH_O) $(LINK_OUTPUT) $(EXECUTABLE) 
	$(VPP) -p -t $(TARGET) --platform $(PLATFORM) \
		--package.out_dir $(PACKAGE_OUT) \
		--package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4 \
		--package.kernel_image $(SD_IMAGE_FILE) \
		--package.boot_mode=sd \
		--package.image_format=ext4 \
		--package.defer_aie_run \
		--package.sd_file $(RUN_APP_SCRIPT) \
		--package.sd_file $(EXECUTABLE) $(LINK_OUTPUT) $(GRAPH_O)

run: all 
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	bash -c '$(LAUNCH_EMULATOR) -run-app $(RUN_APP_SCRIPT) | tee run_app.log; exit $${PIPESTATUS[0]}'
else
	@echo "Hardware build, no emulation executed."
endif

check_edge_sw:
ifndef EDGE_COMMON_SW
	$(error EDGE_COMMON_SW variable is not set, please download and use the pre-built image from https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)
endif

clean:
	rm -rf _x v++_* $(LINK_OUTPUT) $(PACKAGE_OUT) $(GRAPH_O) *.o.* *.o *.xpe *.xo.* \
	       vck190_aie_base*.xclbin* *.xsa *.log *.jou xnwOut Work Map_Report.csv \
	       ilpProblem* sol.db drivers .Xil
	rm -rf ${HOST_OBJ} .Xil ${EXECUTABLE} aie_control_xrt.cpp
cleanall: clean
	-$(RMDIR) build_dir* sd_card*
	-$(RMDIR) package.* run_app.sh 
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage start_simulation.sh *.xclbin

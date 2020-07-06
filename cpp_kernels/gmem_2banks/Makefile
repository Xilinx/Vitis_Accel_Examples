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
	$(ECHO)  "     Command to run the application. This is same as 'check' target but does not have any makefile dependency."
	$(ECHO)  ""
	$(ECHO) "  make sd_card TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to prepare sd_card files."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make check TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86> EDGE_COMMON_SW=<rootfs and kernel image path>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH and EDGE_COMMON_SW is required for SoC shells"
	$(ECHO) ""

############################## Setting up Project Variables ##############################
# Points to top directory of Git repository
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

# SoC variables
RUN_APP_SCRIPT = run_app.sh
PACKAGE_OUT = package.$(TARGET)

LAUNCH_EMULATOR = $(PACKAGE_OUT)/launch_$(TARGET).sh
RESULT_STRING = TEST PASSED

VPP := v++
SDCARD := sd_card

#Include Libraries
include $(ABS_COMMON_REPO)/common/includes/opencl/opencl.mk
include $(ABS_COMMON_REPO)/common/includes/xcl2/xcl2.mk
include $(ABS_COMMON_REPO)/common/includes/cmdparser/cmdparser.mk
include $(ABS_COMMON_REPO)/common/includes/logger/logger.mk
include $(ABS_COMMON_REPO)/common/includes/bitmap/bitmap.mk
CXXFLAGS += $(xcl2_CXXFLAGS) $(cmdparser_CXXFLAGS) $(logger_CXXFLAGS) $(bitmap_CXXFLAGS)
LDFLAGS += $(xcl2_LDFLAGS) $(cmdparser_LDFLAGS) $(logger_LDFLAGS) $(bitmap_LDFLAGS)
HOST_SRCS += $(xcl2_SRCS) $(cmdparser_SRCS) $(logger_SRCS) $(bitmap_SRCS)
CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++11
LDFLAGS += $(opencl_LDFLAGS)

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
HOST_SRCS += src/host.cpp

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
CLFLAGS_apply_watermark +=  --config ./bandwidth.ini



# Adding config files to linker
LDCLFLAGS_apply_watermark += --config apply_watermark.ini 
EXECUTABLE = host
CMD_ARGS = -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
EMCONFIG_DIR = $(TEMP_DIR)
EMU_DIR = $(SDCARD)/data/emulation

############################## Declaring Binary Containers ##############################
BINARY_CONTAINERS += $(BUILD_DIR)/apply_watermark.xclbin
BINARY_CONTAINER_apply_watermark_OBJS += $(TEMP_DIR)/apply_watermark.xo

############################## Setting Targets ##############################
CP = cp -rf
DATA = ./data

.PHONY: all clean cleanall docs emconfig
all: check-devices $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig sd_card

.PHONY: exe
exe: $(EXECUTABLE)

.PHONY: build
build: check-vitis $(BINARY_CONTAINERS)

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/apply_watermark.xo: src/apply_watermark.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) $(CLFLAGS) $(CLFLAGS_apply_watermark) --temp_dir $(TEMP_DIR) -c -k apply_watermark -I'$(<D)' -o'$@' '$<'
$(BUILD_DIR)/apply_watermark.xclbin: $(BINARY_CONTAINER_apply_watermark_OBJS)
	mkdir -p $(BUILD_DIR)
	$(VPP) $(CLFLAGS) --temp_dir $(BUILD_DIR) -l $(LDCLFLAGS) $(LDCLFLAGS_apply_watermark) -o'$@' $(+)

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): check-xrt $(HOST_SRCS) $(HOST_HDRS)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
check: all
ifeq ($(findstring zc, $(DEVICE)), zc)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring zed, $(DEVICE)), zed)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring vck, $(DEVICE)), vck)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring u50, $(DEVICE)), u50)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring samsung, $(DEVICE)), samsung)
$(error This example is not supported for $(DEVICE))
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
ifeq ($(HOST_ARCH), x86)
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
else
	$(ABS_COMMON_REPO)/common/utility/run_emulation.pl "./${LAUNCH_EMULATOR} | tee run_app.log" "./${RUN_APP_SCRIPT} $(TARGET)" "${RESULT_STRING}" "7"
endif
else
ifeq ($(HOST_ARCH), x86)
	./$(EXECUTABLE) -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
endif
endif


############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(findstring zc, $(DEVICE)), zc)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring zed, $(DEVICE)), zed)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring vck, $(DEVICE)), vck)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring u50, $(DEVICE)), u50)
$(error This example is not supported for $(DEVICE))
endif
ifeq ($(findstring samsung, $(DEVICE)), samsung)
$(error This example is not supported for $(DEVICE))
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
ifeq ($(HOST_ARCH), x86)
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
else
	$(ABS_COMMON_REPO)/common/utility/run_emulation.pl "./${LAUNCH_EMULATOR} | tee run_app.log" "./${RUN_APP_SCRIPT} $(TARGET)" "${RESULT_STRING}" "7"
endif
else
ifeq ($(HOST_ARCH), x86)
	./$(EXECUTABLE) -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
endif
endif

.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
ifeq ($(HOST_ARCH), x86)
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
else
	$(ABS_COMMON_REPO)/common/utility/run_emulation.pl "./${LAUNCH_EMULATOR} | tee embedded_run.log" "./${RUN_APP_SCRIPT} $(TARGET)" "${RESULT_STRING}" "7"
endif
else
ifeq ($(HOST_ARCH), x86)
	./$(EXECUTABLE) -x $(BUILD_DIR)/apply_watermark.xclbin -i ./data/input.bmp -c ./data/golden.bmp
else
	$(ECHO) "Please copy the content of sd_card folder and data to an SD Card and run on the board"
endif
endif


############################## Preparing sdcard ##############################
sd_card: gen_run_app
ifneq ($(HOST_ARCH), x86)
	$(VPP) -t $(TARGET) --platform $(DEVICE) -p $(BUILD_DIR)/apply_watermark.xclbin --package.out_dir $(PACKAGE_OUT) --package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4 --package.sd_file $(SD_IMAGE_FILE) --package.sd_file xrt.ini --package.sd_file $(RUN_APP_SCRIPT) --package.sd_file $(EXECUTABLE) --package.sd_file ./data/input.bmp --package.sd_file ./data/golden.bmp -o apply_watermark.xclbin
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir* sd_card*
	-$(RMDIR) package.*
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation/ _vimage/ pl* start_simulation.sh *.xclbin
	-$(RMDIR) ./output.bmp 

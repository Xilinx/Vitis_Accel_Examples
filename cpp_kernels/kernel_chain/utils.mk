#+-------------------------------------------------------------------------------
# The following parameters are assigned with default values. These parameters can
# be overridden through the make command line
#+-------------------------------------------------------------------------------

PROFILE := no

#Generates profile summary report
ifeq ($(PROFILE), yes)
VPP_LDFLAGS += --profile.data all:all:all
endif

DEBUG := no
B_TEMP = `$(XF_PROJ_ROOT)/common/utility/parse_platform_list.py $(DEVICE)`

#Generates debug summary report
ifeq ($(DEBUG), yes)
VPP_LDFLAGS += --dk list_ports
endif

#Setting Platform Path
ifeq ($(findstring xpfm, $(DEVICE)), xpfm)
	B_NAME = $(shell dirname $(DEVICE))
else
	B_NAME = $(B_TEMP)/$(DEVICE)
endif

#Checks for XILINX_VITIS
check-vitis:
ifndef XILINX_VITIS
	$(error XILINX_VITIS variable is not set, please set correctly and rerun)
endif

#Checks for Device Family
ifeq ($(HOST_ARCH), aarch32)
	DEV_FAM = 7Series
else ifeq ($(HOST_ARCH), aarch64)
	DEV_FAM = Ultrascale
endif

#Checks for XILINX_XRT
check-xrt:
ifeq ($(HOST_ARCH), x86)
ifndef XILINX_XRT
	$(error XILINX_XRT variable is not set, please set correctly and rerun)
endif
else
ifndef XILINX_VITIS
	$(error XILINX_VITIS variable is not set, please set correctly and rerun)
endif
endif

#Checks for Correct architecture
ifneq ($(HOST_ARCH), $(filter $(HOST_ARCH),aarch64 aarch32 x86))
$(error HOST_ARCH variable not set, please set correctly and rerun)
endif

#Setting CXX
CXX := g++

#Checks for EDGE_COMMON_SW
ifneq ($(HOST_ARCH), x86)
ifndef EDGE_COMMON_SW
$(error EDGE_COMMON_SW variable is not set, please set correctly and rerun)
endif
ifeq ($(HOST_ARCH), aarch64)
SYSROOT := $(EDGE_COMMON_SW)/sysroots/aarch64-xilinx-linux
SD_IMAGE_FILE := $(EDGE_COMMON_SW)/Image
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
else ifeq ($(HOST_ARCH), aarch32)
SYSROOT := $(EDGE_COMMON_SW)/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/
SD_IMAGE_FILE := $(EDGE_COMMON_SW)/uImage
CXX := $(XILINX_VITIS)/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++
endif
endif

check-devices:
ifndef DEVICE
	$(error DEVICE not set. Please set the DEVICE properly and rerun. Run "make help" for more details.)
endif

#   device2xsa - create a filesystem friendly name from device name
#   $(1) - full name of device
device2xsa = $(strip $(patsubst %.xpfm, % , $(shell basename $(DEVICE))))

############################## Deprecated Checks and Running Rules ##############################
check:
	$(ECHO) "WARNING: \"make check\" is a deprecated command. Please use \"make run\" instead"
	make run

exe:
	$(ECHO) "WARNING: \"make exe\" is a deprecated command. Please use \"make host\" instead"
	make host

# Cleaning stuff
RM = rm -f
RMDIR = rm -rf

ECHO:= @echo

docs: README.rst

README.rst: description.json
	$(XF_PROJ_ROOT)/common/utility/readme_gen/readme_gen.py description.json

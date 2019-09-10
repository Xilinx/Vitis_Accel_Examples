#+-------------------------------------------------------------------------------
# The following parameters are assigned with default values. These parameters can
# be overridden through the make command line
#+-------------------------------------------------------------------------------

PROFILE := no

#Generates profile summary report
ifeq ($(PROFILE), yes)
LDCLFLAGS += --profile_kernel data:all:all:all
endif

DEBUG := no

#Generates debug summary report
ifeq ($(DEBUG), yes)
LDCLFLAGS += --dk list_ports
endif

#Checks for XILINX_VITIS
ifndef XILINX_VITIS
$(error XILINX_VITIS variable is not set, please set correctly and rerun)
endif

#Checks for XILINX_XRT
check-xrt:
ifndef XILINX_XRT
	$(error XILINX_XRT variable is not set, please set correctly and rerun)
endif

#Checks for SYSROOT
ifneq ($(HOST_ARCH), x86)
ifndef SYSROOT
$(error SYSROOT variable is not set, please set correctly and rerun)
endif
endif

check-devices:
ifndef DEVICE
	$(error DEVICE not set. Please set the DEVICE properly and rerun. Run "make help" for more details.)
endif

#   device2xsa - create a filesystem friendly name from device name
#   $(1) - full name of device
device2xsa = $(strip $(patsubst %.xpfm, % , $(shell basename $(DEVICE))))

# Cleaning stuff
RM = rm -f
RMDIR = rm -rf

ECHO:= @echo

docs: README.md

README.md: description.json
	$(ABS_COMMON_REPO)/common/utility/readme_gen/readme_gen.py description.json

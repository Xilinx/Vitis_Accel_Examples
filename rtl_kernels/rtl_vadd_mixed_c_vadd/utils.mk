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

#Generates debug summary report
ifeq ($(DEBUG), yes)
VPP_LDFLAGS += --dk list_ports
endif

#Setting PLATFORM 
ifeq ($(PLATFORM),)
ifneq ($(DEVICE),)
$(warning WARNING: DEVICE is deprecated in make command. Please use PLATFORM instead)
PLATFORM := $(DEVICE)
endif
endif

#Checks for XILINX_VITIS
check-vitis:
ifndef XILINX_VITIS
	$(error XILINX_VITIS variable is not set, please set correctly and rerun)
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

check-device:
	@set -eu; \
	inallowlist=False; \
	inblocklist=False; \
	if [ "$(PLATFORM_ALLOWLIST)" = "" ]; \
	    then inallowlist=True; \
	fi; \
	for dev in $(PLATFORM_ALLOWLIST); \
	    do if [[ $$(echo $(PLATFORM) | grep $$dev) != "" ]]; \
	    then inallowlist=True; fi; \
	done ;\
	for dev in $(PLATFORM_BLOCKLIST); \
	    do if [[ $$(echo $(PLATFORM) | grep $$dev) != "" ]]; \
	    then inblocklist=True; fi; \
	done ;\
	if [[ $$inblocklist == True ]]; \
	    then echo "[ERROR]: This example is not supported for $(PLATFORM)."; exit 1;\
	fi; \
	if [[ $$inallowlist == False ]]; \
	    then echo "[Warning]: The platform $(PLATFORM) not in allowlist."; \
	fi;

#Checks for Correct architecture
ifneq ($(HOST_ARCH), $(filter $(HOST_ARCH),aarch64 aarch32 x86))
$(error HOST_ARCH variable not set, please set correctly and rerun)
endif

#Setting CXX
CXX := g++

#Checks for EDGE_COMMON_SW
ifneq ($(HOST_ARCH), x86)
ifndef EDGE_COMMON_SW
$(error EDGE_COMMON_SW variable is not set, please download and use the pre-built image from https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)
endif
ifeq ($(HOST_ARCH), aarch64)
SYSROOT := $(EDGE_COMMON_SW)/sysroots/cortexa72-cortexa53-xilinx-linux
SD_IMAGE_FILE := $(EDGE_COMMON_SW)/Image
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
else ifeq ($(HOST_ARCH), aarch32)
SYSROOT := $(EDGE_COMMON_SW)/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/
SD_IMAGE_FILE := $(EDGE_COMMON_SW)/uImage
CXX := $(XILINX_VITIS)/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++
endif
endif

gen_run_app:
ifneq ($(HOST_ARCH), x86)
	rm -rf run_app.sh
	$(ECHO) 'export LD_LIBRARY_PATH=/mnt:/tmp:$$LD_LIBRARY_PATH' >> run_app.sh
	$(ECHO) 'export PATH=$$PATH:/sbin' >> run_app.sh
	$(ECHO) 'export XILINX_XRT=/usr' >> run_app.sh
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(ECHO) 'export XILINX_VITIS=$$PWD' >> run_app.sh
	$(ECHO) 'export XCL_EMULATION_MODE=$(TARGET)' >> run_app.sh
endif
	$(ECHO) '$(EXECUTABLE) vadd.xclbin' >> run_app.sh
	$(ECHO) 'return_code=$$?' >> run_app.sh
	$(ECHO) 'if [ $$return_code -ne 0 ]; then' >> run_app.sh
	$(ECHO) 'echo "ERROR: host run failed, RC=$$return_code"' >> run_app.sh
	$(ECHO) 'fi' >> run_app.sh
	$(ECHO) 'echo "INFO: host run completed."' >> run_app.sh
endif
check-platform:
ifndef PLATFORM
	$(error PLATFORM not set. Please set the PLATFORM properly and rerun. Run "make help" for more details.)
endif

#   device2xsa - create a filesystem friendly name from device name
#   $(1) - full name of device
device2xsa = $(strip $(patsubst %.xpfm, % , $(shell basename $(PLATFORM))))

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

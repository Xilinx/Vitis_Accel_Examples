GEN_DIR = ./test
USR_VPP_FLAGS:=
USR_VPP_LDFLAGS:=

platform_test:
	platforminfo -j -d $(PLATFORM) > platform_info.json
	$(XF_PROJ_ROOT)/common/utility/platform_gen.py platform_info.json bandwidth
	$(XF_PROJ_ROOT)/common/utility/platform_gen.py platform_info.json hostmemory 

ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
ifeq ($(findstring 202010, $(PLATFORM)), 202010)
$(error [ERROR]: This example is not supported for $(PLATFORM) when targeting hw_emu.)
endif
endif

VPP_LDFLAGS+= --config platform_bandwidth.cfg
VPP_LDFLAGS+= --config platform_hostmemory.cfg
ifneq ($(USR_VPP_FLAGS), )      
VPP_FLAGS += $(USR_VPP_FLAGS)
endif
ifneq ($(USR_VPP_LDFLAGS), )      
VPP_LDFLAGS += $(USR_VPP_LDFLAGS)
endif
PLATFORM_JSON=platform.json

GEN_DIR = ./test
USR_VPP_FLAGS:=
USR_VPP_LDFLAGS:=

platform_test:
	platforminfo -j -d $(DEVICE) > platform_info.json
	$(XF_PROJ_ROOT)/common/utility/platform_gen.py platform_info.json bandwidth

VPP_LDFLAGS+= --config platform_bandwidth.cfg
ifneq ($(USR_VPP_FLAGS), )      
VPP_FLAGS += $(USR_VPP_FLAGS)
endif
ifneq ($(USR_VPP_LDFLAGS), )      
VPP_LDFLAGS += $(USR_VPP_LDFLAGS)
endif
PLATFORM_JSON=platform.json

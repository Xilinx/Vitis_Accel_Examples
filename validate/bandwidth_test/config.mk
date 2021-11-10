GEN_DIR = ./test
USR_VPP_FLAGS:=
USR_VPP_LDFLAGS:=

OSDIST := $(shell lsb_release -i )	
OSREL := $(shell lsb_release -r )	

ifeq ($(HOST_ARCH), x86)	
ifeq ($(findstring CentOS, $(OSDIST)), CentOS)	
ifeq ($(findstring 7., $(OSREL)), 7.)	
LDFLAGS += -lboost_system-mt
endif	
endif	
ifeq ($(findstring RedHat, $(OSDIST)), RedHat)	
ifeq ($(findstring 7., $(OSREL)), 7.)	
LDFLAGS += -lboost_system-mt	
endif	
endif
endif

platform_test:
	platforminfo -j -d $(PLATFORM) > platform_info.json
	$(XF_PROJ_ROOT)/common/utility/platform_gen.py platform_info.json bandwidth

VPP_LDFLAGS+= --config platform_bandwidth.cfg
ifneq ($(USR_VPP_FLAGS), )      
VPP_FLAGS += $(USR_VPP_FLAGS)
endif
ifneq ($(USR_VPP_LDFLAGS), )      
VPP_LDFLAGS += $(USR_VPP_LDFLAGS)
endif
PLATFORM_JSON=platform.json

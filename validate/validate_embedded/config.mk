USR_VPP_FLAGS:=
USR_VPP_LDFLAGS:=

ifeq ($(findstring zcu102_base_dfx, $(PLATFORM)), zcu102_base_dfx)
VPP_LDFLAGS+= --config $(XF_PROJ_ROOT)/common/platform/zcu102_base_dfx/validate.cfg 
VPP_PFLAGS += --package.sd_file $(XF_PROJ_ROOT)/common/platform/zcu102_base_dfx/platform.json
else ifeq ($(findstring zcu102_base, $(PLATFORM)), zcu102_base)
VPP_LDFLAGS+= --config $(XF_PROJ_ROOT)/common/platform/zcu102_base/validate.cfg
VPP_PFLAGS += --package.sd_file $(XF_PROJ_ROOT)/common/platform/zcu102_base/platform.json
else ifeq ($(findstring zcu104_base, $(PLATFORM)), zcu104_base)
VPP_LDFLAGS+= --config $(XF_PROJ_ROOT)/common/platform/zcu104_base/validate.cfg 
VPP_PFLAGS += --package.sd_file $(XF_PROJ_ROOT)/common/platform/zcu104_base/platform.json
else ifeq ($(findstring vck190, $(PLATFORM)), vck190)
VPP_LDFLAGS+= --config $(XF_PROJ_ROOT)/common/platform/vck190/validate.cfg
VPP_PFLAGS += --package.sd_file $(XF_PROJ_ROOT)/common/platform/vck190/platform.json
endif

ifneq ($(USR_VPP_FLAGS), )      
VPP_FLAGS += $(USR_VPP_FLAGS)
endif
ifneq ($(USR_VPP_LDFLAGS), )      
VPP_LDFLAGS += $(USR_VPP_LDFLAGS)
endif

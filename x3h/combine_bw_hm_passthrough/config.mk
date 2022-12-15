ifneq ($(USR_VPP_FLAGS), )      
VPP_FLAGS += $(USR_VPP_FLAGS)
endif
ifneq ($(USR_VPP_LDFLAGS), )      
VPP_LDFLAGS += $(USR_VPP_LDFLAGS)
endif

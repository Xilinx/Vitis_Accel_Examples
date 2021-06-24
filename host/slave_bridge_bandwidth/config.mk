VPP_LDFLAGS := --config ./krnl_bandwidth.cfg

ifeq ($(TARGET),$(findstring $(TARGET), hw_emu))
ifeq ($(findstring 202010, $(DEVICE)), 202010)
VPP_LDFLAGS := --config ./krnl_bandwidth_202010_hw_emu.cfg
endif
endif

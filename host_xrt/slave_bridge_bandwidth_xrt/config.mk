VPP_LDFLAGS := --config ./bandwidth.cfg

ifeq ($(TARGET),$(findstring $(TARGET), hw_emu))
ifeq ($(findstring 202010, $(DEVICE)), 202010)
VPP_LDFLAGS := --config ./bandwidth_202010_hw_emu.cfg
endif
endif

VPP_LDFLAGS := --config ./krnl_vadd.cfg

ifeq ($(TARGET),$(findstring $(TARGET), hw_emu))
ifeq ($(findstring 202010, $(DEVICE)), 202010)
VPP_LDFLAGS := --config ./krnl_vadd_202010_hw_emu.cfg
endif
endif

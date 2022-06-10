ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
	VPP_FLAGS_increment_func += --config hw_emu_func.cfg
endif

ifeq ($(TARGET), hw_emu)
	LDFLAGS += -lxrt_hwemu
else ifeq ($(TARGET), hw)
	LDFLAGS += -lxrt_core
endif

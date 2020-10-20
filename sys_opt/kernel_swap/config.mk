ifneq ($(HOST_ARCH), x86)
CXXFLAGS += -Ddevice_type
endif

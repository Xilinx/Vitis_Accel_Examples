ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
ifeq ($(findstring 2020, $(PLATFORM)), 2020)
$(error [ERROR]: This example is not supported for $(PLATFORM) when targeting hw_emu.)
endif
ifeq ($(findstring 202110, $(PLATFORM)), 202110)
$(error [ERROR]: This example is not supported for $(PLATFORM) when targeting hw_emu.)
endif
ifeq ($(findstring u50, $(PLATFORM)), u50)
$(error [ERROR]: This example is not supported for $(PLATFORM) when targeting hw_emu.)
endif
endif

ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
ifeq ($(findstring 202010, $(PLATFORM)), 202010)
$(error [ERROR]: This example is not supported for $(PLATFORM) when targeting hw_emu.)
endif
endif

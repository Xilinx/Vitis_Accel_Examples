ifeq ($(TARGET),$(findstring $(TARGET), hw_emu))
ifeq ($(findstring 202010, $(DEVICE)), 202010)
$(error [ERROR]: This example is not supported for $(DEVICE) when targeting hw_emu.)
endif
endif

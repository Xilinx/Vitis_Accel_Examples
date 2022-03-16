DRIVE_PATH:=
ifneq ($(DRIVE_PATH), )
	CMD_ARGS += -p $(DRIVE_PATH)
else ifeq ($(findstring u2_, $(PLATFORM)), u2_)
ifeq ($(TARGET),$(filter $(TARGET),hw))	
	CMD_ARGS += -p /dev/nvme0n1
endif
endif

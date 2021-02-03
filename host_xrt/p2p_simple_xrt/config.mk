DRIVE_PATH:=
ifneq ($(DRIVE_PATH), )
	EXTRA_HOST_ARGS := -p $(DRIVE_PATH)
else ifeq ($(findstring _u2_, $(DEVICE)), _u2_)
ifeq ($(TARGET),$(filter $(TARGET),hw))	
	EXTRA_HOST_ARGS := -p /dev/nvme0n1
endif
endif

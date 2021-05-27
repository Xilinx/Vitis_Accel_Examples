OSDIST := $(shell lsb_release -i )
OSREL := $(shell lsb_release -r )

ifeq ($(HOST_ARCH), x86)
ifeq ($(findstring CentOS, $(OSDIST)), CentOS)
ifeq ($(findstring 7., $(OSREL)), 7.)
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif
endif
ifeq ($(findstring RedHat, $(OSDIST)), RedHat)
ifeq ($(findstring 7., $(OSREL)), 7.)
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif
endif
ifeq ($(findstring Ubuntu, $(OSDIST)), Ubuntu)
ifeq ($(findstring 16., $(OSREL)), 16.)
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif
endif
endif

DRIVE_PATH:=
ifneq ($(DRIVE_PATH), )
	CMD_ARGS += -p $(DRIVE_PATH)
else ifeq ($(findstring _u2_, $(DEVICE)), _u2_)
ifeq ($(TARGET),$(filter $(TARGET),hw))	
	CMD_ARGS += -p /dev/nvme0n1
endif
endif

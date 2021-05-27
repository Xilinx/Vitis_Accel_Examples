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

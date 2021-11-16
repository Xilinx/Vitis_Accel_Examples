# By default, the value of ddr_banks is kept 2 
# The ddr_banks should be chosen in such a way that the selected platform supports the DDR configuration.

ddr_banks := 1

# Kernel linker config files
ifeq ($(findstring samsung, $(PLATFORM)), samsung)
ddr_banks := 1
endif

ifeq ($(findstring vck190, $(PLATFORM)), vck190)
VPP_LDFLAGS+= --config vck190.cfg
else
ifeq ($(findstring zc, $(PLATFORM)), zc)
VPP_LDFLAGS+= --config $(ddr_banks)bank_zc.cfg
else
ifeq ($(findstring samsung, $(PLATFORM)), samsung)
VPP_LDFLAGS+= --config samsung.cfg
else
VPP_LDFLAGS+= --config $(ddr_banks)bank.cfg
endif
endif
endif

VPP_FLAGS += -DNDDR_BANKS=$(ddr_banks)

CXXFLAGS += -DNDDR_BANKS=$(ddr_banks)

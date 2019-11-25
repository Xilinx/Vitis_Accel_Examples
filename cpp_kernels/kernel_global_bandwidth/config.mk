# By default, the value of ddr_banks is kept 2 
# The ddr_banks should be chosen in such a way that the selected platform supports the DDR configuration.

ddr_banks := 2

# Kernel linker config files
ifeq ($(findstring zc, $(DEVICE)), samsung_U2x4)
ddr_banks := 1
endif

ifeq ($(findstring zc, $(DEVICE)), zc)
LDCLFLAGS+= --config $(ddr_banks)bank_zc.ini
else
LDCLFLAGS+= --config $(ddr_banks)bank.ini
endif

CLFLAGS += -DNDDR_BANKS=$(ddr_banks)

CXXFLAGS += -DNDDR_BANKS=$(ddr_banks)

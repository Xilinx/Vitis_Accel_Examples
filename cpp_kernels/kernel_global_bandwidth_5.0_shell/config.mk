# By default, the value of ddr_banks is kept 2 as every platform has atleast 2DDR configuration. 
# The ddr_banks should be chosen in such a way that the selected platform supports the DDR configuration.
ddr_banks := 2

# Kernel linker flags
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem0:bank0 
ifneq ($(strip $(ddr_banks)),1)
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem1:bank1
ifneq ($(strip $(ddr_banks)),2)
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem2:bank2
ifneq ($(strip $(ddr_banks)),3)
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem3:bank3
endif
endif
endif

CLFLAGS += -DNDDR_BANKS=$(ddr_banks)

CXXFLAGS += -DNDDR_BANKS=$(ddr_banks)

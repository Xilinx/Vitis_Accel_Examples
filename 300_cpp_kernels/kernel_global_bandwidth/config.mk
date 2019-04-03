# By default, the value of ddr_banks is kept 2 as every platform has atleast 2DDR configuration. 
# The ddr_banks should be chosen in such a way that the selected platform supports the DDR configuration.
ddr_banks := 2

# Kernel linker flags
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem0:DDR[0] 
ifneq ($(strip $(ddr_banks)),1)
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem1:DDR[1]
ifneq ($(strip $(ddr_banks)),2)
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem2:DDR[2]
ifneq ($(strip $(ddr_banks)),3)
LDCLFLAGS+= --sp bandwidth_1.m_axi_gmem3:DDR[3]
endif
endif
endif

CLFLAGS += -DNDDR_BANKS=$(ddr_banks)

CXXFLAGS += -DNDDR_BANKS=$(ddr_banks)

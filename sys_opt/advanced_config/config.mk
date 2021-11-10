ifeq ($(findstring zc, $(PLATFORM)), zc)
VPP_LDFLAGS += --config vadd_soc.cfg
VPP_FLAGS += --config vadd_vadd_soc.cfg
else ifeq ($(findstring vck, $(PLATFORM)), vck)
VPP_LDFLAGS += --config vadd_soc.cfg
VPP_FLAGS += --config vadd_vadd_soc.cfg
else
VPP_LDFLAGS += --config vadd_pcie.cfg
VPP_FLAGS += --config vadd_vadd_pcie.cfg
endif

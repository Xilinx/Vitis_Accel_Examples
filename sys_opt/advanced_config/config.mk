ifeq ($(findstring zc, $(PLATFORM)), zc)
VPP_LDFLAGS += --config vadd_soc.cfg
VPP_FLAGS += --config vadd_vadd_soc.cfg
else ifeq ($(findstring vck190, $(PLATFORM)), vck190)
VPP_LDFLAGS += --config vadd_soc.cfg
VPP_FLAGS += --config vadd_vadd_soc.cfg
else
VPP_LDFLAGS += --config vadd_pcie.cfg
VPP_FLAGS += --config vadd_vadd_pcie.cfg
endif

ifeq ($(findstring zc, $(DEVICE)), zc)
VPP_LDFLAGS += --config vadd_soc.cfg
VPP_FLAGS += --config vadd_vadd_soc.cfg
else ifeq ($(findstring vck, $(DEVICE)), vck)
VPP_LDFLAGS += --config vadd_soc.cfg
VPP_FLAGS += --config vadd_vadd_soc.cfg
else
VPP_LDFLAGS += --config vadd_pcie.cfg
VPP_FLAGS += --config vadd_vadd_pcie.cfg
endif

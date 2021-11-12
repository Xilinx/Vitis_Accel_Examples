ifeq ($(findstring zc, $(PLATFORM)), zc)
VPP_LDFLAGS += --config vadd_soc.cfg
else ifeq ($(findstring vck190, $(PLATFORM)), vck190)
VPP_LDFLAGS += --config vadd_soc.cfg
else
VPP_LDFLAGS += --config vadd_pcie.cfg
endif

VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/vadd.xo: scripts/package_kernel.tcl scripts/gen_xo.tcl src/hdl/*.sv src/hdl/*.v
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_xo.tcl -tclargs $(TEMP_DIR)/vadd.xo vadd $(TARGET) $(PLATFORM) $(XSA)

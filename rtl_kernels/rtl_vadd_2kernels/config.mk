VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/vadd0.xo: scripts/package_kernel_0.tcl scripts/gen_vadd0_xo.tcl src/ip_0/hdl/*.sv src/ip_0/hdl/*.v 
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_vadd0_xo.tcl -tclargs $(TEMP_DIR)/vadd0.xo vadd $(TARGET) $(PLATFORM) $(XSA)
$(TEMP_DIR)/vadd1.xo: scripts/package_kernel_1.tcl scripts/gen_vadd1_xo.tcl src/ip_1/hdl/*.sv src/ip_1/hdl/*.v
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_vadd1_xo.tcl -tclargs $(TEMP_DIR)/vadd1.xo vadd $(TARGET) $(PLATFORM) $(XSA)

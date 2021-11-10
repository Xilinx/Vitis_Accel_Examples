VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/vadd.xo: src/krnl_vadd/package_kernel.tcl src/krnl_vadd/gen_xo.tcl src/krnl_vadd/hdl/*.sv src/krnl_vadd/hdl/*.v 
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source src/krnl_vadd/gen_xo.tcl -tclargs $(TEMP_DIR)/vadd.xo $(TARGET) $(PLATFORM) $(XSA)

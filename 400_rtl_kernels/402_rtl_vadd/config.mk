VIVADO := $(XILINX_VIVADO)/bin/vivado
$(BUILD_DIR)/vadd.xo: src/kernel.xml scripts/package_kernel.tcl scripts/gen_xo.tcl src/hdl/*.sv src/hdl/*.v 
	mkdir -p $(BUILD_DIR)
	$(VIVADO) -mode batch -source scripts/gen_xo.tcl -tclargs $(BUILD_DIR)/vadd.xo vadd $(TARGET) $(DSA)

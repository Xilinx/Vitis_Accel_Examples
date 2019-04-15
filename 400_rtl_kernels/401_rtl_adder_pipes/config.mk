VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/input.xo: src/kernel_input_stage.xml scripts/gen_input_xo.tcl scripts/package_kernel_input_stage.tcl src/input_stage/hdl/*.sv src/input_stage/hdl/*.v
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_input_xo.tcl -tclargs $(TEMP_DIR)/input.xo input $(TARGET) $(DSA)

$(TEMP_DIR)/adder.xo: src/kernel_adder_stage.xml scripts/gen_adder_xo.tcl scripts/package_kernel_adder_stage.tcl src/adder_stage/hdl/*.sv src/adder_stage/hdl/*.v
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_adder_xo.tcl -tclargs $(TEMP_DIR)/adder.xo adder $(TARGET) $(DSA)

$(TEMP_DIR)/output.xo: src/kernel_output_stage.xml scripts/gen_output_xo.tcl scripts/package_kernel_output_stage.tcl src/output_stage/hdl/*.sv src/output_stage/hdl/*.v
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_output_xo.tcl -tclargs $(TEMP_DIR)/output.xo output $(TARGET) $(DSA)

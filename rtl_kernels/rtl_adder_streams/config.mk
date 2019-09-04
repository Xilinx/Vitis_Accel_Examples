VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/input.xo:
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_input_xo.tcl -tclargs $(TEMP_DIR)/input.xo input $(TARGET) $(DEVICE) $(XSA)

$(TEMP_DIR)/adder.xo: 
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_adder_xo.tcl -tclargs $(TEMP_DIR)/adder.xo adder $(TARGET) $(DEVICE) $(XSA)

$(TEMP_DIR)/output.xo:
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_output_xo.tcl -tclargs $(TEMP_DIR)/output.xo output $(TARGET) $(DEVICE) $(XSA)

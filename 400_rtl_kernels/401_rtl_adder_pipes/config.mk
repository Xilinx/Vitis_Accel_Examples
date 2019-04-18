VIVADO := $(XILINX_VIVADO)/bin/vivado
$(XCLBIN)/input.$(TARGET).$(DSA).xo: src/kernel_input_stage.xml scripts/gen_input_xo.tcl scripts/package_kernel_input_stage.tcl src/input_stage/hdl/*.sv src/input_stage/hdl/*.v
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_input_xo.tcl -tclargs $(XCLBIN)/input.$(TARGET).$(DSA).xo input $(TARGET) $(DSA)

$(XCLBIN)/adder.$(TARGET).$(DSA).xo: src/kernel_adder_stage.xml scripts/gen_adder_xo.tcl scripts/package_kernel_adder_stage.tcl src/adder_stage/hdl/*.sv src/adder_stage/hdl/*.v
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_adder_xo.tcl -tclargs $(XCLBIN)/adder.$(TARGET).$(DSA).xo adder $(TARGET) $(DSA)

$(XCLBIN)/output.$(TARGET).$(DSA).xo: src/kernel_output_stage.xml scripts/gen_output_xo.tcl scripts/package_kernel_output_stage.tcl src/output_stage/hdl/*.sv src/output_stage/hdl/*.v
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_output_xo.tcl -tclargs $(XCLBIN)/output.$(TARGET).$(DSA).xo output $(TARGET) $(DSA)


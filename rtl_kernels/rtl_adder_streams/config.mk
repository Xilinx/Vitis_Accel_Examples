VIVADO := $(XILINX_VIVADO)/bin/vivado
$(XCLBIN)/input.$(TARGET).$(DSA).xo:
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_input_xo.tcl -tclargs $(XCLBIN)/input.$(TARGET).$(DSA).xo

$(XCLBIN)/adder.$(TARGET).$(DSA).xo: 
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_adder_xo.tcl -tclargs $(XCLBIN)/adder.$(TARGET).$(DSA).xo

$(XCLBIN)/output.$(TARGET).$(DSA).xo:
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_output_xo.tcl -tclargs $(XCLBIN)/output.$(TARGET).$(DSA).xo


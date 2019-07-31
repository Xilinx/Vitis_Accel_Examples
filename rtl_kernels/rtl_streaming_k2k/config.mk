VIVADO := $(XILINX_VIVADO)/bin/vivado
$(XCLBIN)/myadder1.$(TARGET).$(DSA).xo:
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source ./src/gen_xo_myadder1.tcl -tclargs $(DEVICE) $(XCLBIN)/myadder1.$(TARGET).$(DSA).xo
$(XCLBIN)/myadder2.$(TARGET).$(DSA).xo:
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source ./src/gen_xo_myadder2.tcl -tclargs $(DEVICE) $(XCLBIN)/myadder2.$(TARGET).$(DSA).xo

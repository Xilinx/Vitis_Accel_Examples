VIVADO := $(XILINX_VIVADO)/bin/vivado
$(XCLBIN)/myadder1.$(TARGET).$(DSA).xo:  
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source ./src/gen_xo.tcl -tclargs $(DEVICE) $(XCLBIN)/myadder1.$(TARGET).$(DSA).xo

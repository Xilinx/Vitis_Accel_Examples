VIVADO := $(XILINX_VIVADO)/bin/vivado
$(XCLBIN)/vadd0.$(TARGET).$(DSA).xo: src/kernel_0.xml scripts/package_kernel_0.tcl scripts/gen_vadd0_xo.tcl src/ip_0/hdl/*.sv src/ip_0/hdl/*.v 
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_vadd0_xo.tcl -tclargs $(XCLBIN)/vadd0.$(TARGET).$(DSA).xo vadd $(TARGET) $(DSA)
$(XCLBIN)/vadd1.$(TARGET).$(DSA).xo: src/kernel_1.xml scripts/package_kernel_1.tcl scripts/gen_vadd1_xo.tcl src/ip_1/hdl/*.sv src/ip_1/hdl/*.v
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_vadd1_xo.tcl -tclargs $(XCLBIN)/vadd1.$(TARGET).$(DSA).xo vadd $(TARGET) $(DSA)

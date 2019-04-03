VIVADO := $(XILINX_VIVADO)/bin/vivado
$(XCLBIN)/rtl_vadd.$(TARGET).$(DSA).xo: src/kernel.xml scripts/package_kernel.tcl scripts/gen_rtl_vadd_xo.tcl src/hdl/*.sv src/hdl/*.v
	mkdir -p $(XCLBIN)
	$(VIVADO) -mode batch -source scripts/gen_rtl_vadd_xo.tcl -tclargs $(XCLBIN)/rtl_vadd.$(TARGET).$(DSA).xo vadd $(TARGET) $(DSA)
$(XCLBIN)/krnl_vadd.$(TARGET).$(DSA).xo: ./src/krnl_vadd.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k krnl_vadd -I'$(<D)' -o'$@' '$<' 

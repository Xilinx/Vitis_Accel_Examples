VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/myadder1.xo: 
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source ./src/gen_xo.tcl -tclargs $(DEVICE) $(TEMP_DIR)/myadder1.xo

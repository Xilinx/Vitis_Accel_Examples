VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/myadder1.xo:
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source ./src/gen_xo_myadder1.tcl -tclargs $(DEVICE) $(TEMP_DIR)/myadder1.xo
$(TEMP_DIR)/myadder2.xo:
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source ./src/gen_xo_myadder2.tcl -tclargs $(DEVICE) $(TEMP_DIR)/myadder2.xo

VIVADO := $(XILINX_VIVADO)/bin/vivado
$(TEMP_DIR)/myadder.xo: 
	mkdir -p $(TEMP_DIR)
	$(VIVADO) -mode batch -source scripts/gen_xo.tcl -tclargs $(TEMP_DIR)/myadder.xo myadder $(TARGET) $(PLATFORM) $(XSA)
$(TEMP_DIR)/krnl_s2mm.xo: ./src/krnl_s2mm.cpp
	mkdir -p $(TEMP_DIR)
	v++ $(VPP_FLAGS) -c --platform $(PLATFORM) -k krnl_s2mm -I'$(<D)' -o'$@' '$<'
$(TEMP_DIR)/krnl_mm2s.xo: ./src/krnl_mm2s.cpp
	mkdir -p $(TEMP_DIR)
	v++ $(VPP_FLAGS) -c --platform $(PLATFORM) -k krnl_mm2s -I'$(<D)' -o'$@' '$<'

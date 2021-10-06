increment_tlm.xo: src/increment.cpp ./increment.json
	mkdir -p xo_build
	$(RM) $@; rm xo_build/* -rf
	cd xo_build; create_sc_xo ../src/increment.cpp ../increment.json; cp increment_tlm.xo ..

$(TEMP_DIR)/increment.xo: src/increment.cpp
	mkdir -p $(TEMP_DIR)
	$(VPP) $(VPP_FLAGS) -c -k increment --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'

ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
VPP_FLAGS += --config global.cfg -I .
VPP_LDFLAGS += --config ./krnl_incr_sc.cfg
BINARY_CONTAINER_krnl_incr_OBJS += increment_tlm.xo
else
VPP_LDFLAGS += --config ./krnl_incr.cfg
BINARY_CONTAINER_krnl_incr_OBJS += $(TEMP_DIR)/increment.xo
endif

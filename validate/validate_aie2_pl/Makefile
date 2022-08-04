MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
MLLIB_ROOT := $(CUR_DIR)/mllib

check_vivado:
ifeq (,$(wildcard $(XILINX_VIVADO)/bin/vivado))
	$(error Cannot locate Vivado installation. Please set XILINX_VIVADO variable.)
endif
check_vpp:
ifeq (,$(wildcard $(XILINX_VITIS)/bin/v++))
	$(error Cannot locate Vitis installation. Please set XILINX_VITIS variable.)
endif

XILINX_VITIS_AIETOOLS := $(XILINX_VITIS)/aietools
TARGET   := hw_emu

PLATFORM = /proj/xbuilds/Vitis_HW_EMU/2022.2/xilinx_v70_hw_emu_202220_1/2022.2_0727_2230/xilinx_v70_hw_emu_202220_1/xilinx_v70_hw_emu_202220_1.xpfm

PL_CTRL_AXIMM_CH_NUM ?= 1
#########################compile ADF#############################################
GRAPH    = src/graph.cpp
LIBADF  = libadf.a
.PHONY: aie
aie: ${LIBADF}

${LIBADF}: ${GRAPH}
	aiecompiler --aiearch=aie-ml -v -log-level=5 --enable-multi-layer --enable-core-processor-bus --disable-dma-autostart --disable-multirate-analysis --dataflow --platform=${PLATFORM} --output=graph.json src/graph.cpp

######################call ucode gen tool to generate ucode######################
.PHONY: ucode_gen
XOS = ./tools/pl_controller_hw.xo
ucode_gen: ./tools/ucode_gen.sh ${XOS}

./tools/ucode_gen.sh ${XOS}:
	make -f ${CUR_DIR}/pl_controller/Makefile PLATFORM=$(PLATFORM) PL_CTRL_AXIMM_CH_NUM=$(PL_CTRL_AXIMM_CH_NUM) DEST_DIR=./tools install

UCODE := ucode.bin
.PHONY: ucode
ucode: ${UCODE}

${UCODE}:./tools/ucode_gen.sh libadf.a 
	./tools/ucode_gen.sh -i ./Work/ps/c_rts -o ${UCODE} -c $(PL_CTRL_AXIMM_CH_NUM)
#######################compile to generate xclbin################################ 
#LINK_TARGET_FMT := xclbin
ifneq (,$(wildcard $(XILINX_VITIS)/bin/v++))
VITIS_VER = $(shell v++ --version | grep 'v++' | sed 's/^[[:space:]]*//' | sed -e 's/^[*]* v++ v//g' | cut -d " " -f1)
ifeq ($(shell expr $(VITIS_VER) \>= 2022.1), 1)
LINK_TARGET_FMT ?= xsa
else
LINK_TARGET_FMT ?= xclbin
endif
endif

XCLBIN   = aie_base_graph.${LINK_TARGET_FMT}
VPP_SPEC = ${CUR_DIR}/system.cfg
VCC      = v++
VPP_LDFLAGS= --save-temps --verbose --config ${VPP_SPEC}

${XCLBIN}: ${LIBADF} ${VPP_SPEC} ${XOS} | check_vpp
	${VCC} -g -l --platform ${PLATFORM} ${LIBADF} ${XOS} --vivado.prop fileset.sim_1.xsim.simulate.runtime=800us \
	      	-t ${TARGET} ${VPP_LDFLAGS} -o $@
	
####################compile PS code###############################################
.PHONY: host
HOST_EXE = main.exe
host: ${HOST_EXE}

CXX := g++
CXX_VER := 8.3.0
CXX_V := $(shell echo $(CXX_VER) | awk -F. '{print tolower($$1)}')
ifneq ($(shell expr $(shell echo "__GNUG__" | g++ -E -x c++ - | tail -1) \>= $(CXX_V)), 1)
ifeq (,$(wildcard $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/bin/g++))
$(error [ERROR]: g++ version too old. Please use $(CXX_VER) or above)
else
CXX := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/bin/g++
LD_LIBRARY_PATH := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/lib64:$(LD_LIBRARY_PATH)
endif
endif

${HOST_EXE}: sw/*.cpp ${CUR_DIR}/pl_controller/ps_helper/*.cpp
	make -C sw/ CXX=$(CXX) PL_CTRL_AXIMM_CH_NUM=$(PL_CTRL_AXIMM_CH_NUM)

##################run hardware emulation######################################################
.PHONY: package run_hw_emu

FINAL_XCLBIN = final.xclbin
package: package_${TARGET}
package_${TARGET}: ${LIBADF} ${XCLBIN} ${HOST_EXE} 
	${VCC} -p -t ${TARGET} -f ${PLATFORM} -o ${FINAL_XCLBIN} \
		--package.out_dir package.hw_emu \
		--package.boot_mode=ospi \
		--package.defer_aie_run \
		${XCLBIN} ${LIBADF} 

HOST_ARGS = ${UCODE}
run_hw_emu: package.hw_emu/launch_hw_emu.sh
	emconfigutil --platform $(PLATFORM) --nd 1
	@echo "Running Emulation: $(HOST_PREAMBLE) ./$(HOST_EXE) $(FINAL_XCLBIN)"
	XCL_EMULATION_MODE=$(TARGET) \
	./$(HOST_EXE) $(FINAL_XCLBIN) $(HOST_ARGS) | tee launch_hw_emu.log
	grep "Test passed" launch_hw_emu.log || exit 1
.PHONY: run

run: aie ucode_gen ucode ${XCLBIN} host package run_hw_emu

.PHONY: clean
clean:
	make clean -C sw/
	rm -rf ./tools me.soln ${LIBADF} _x v++_* *.o.* package.hw_emu aiesimulator_output pl_sample_counts .AIE_SIM_CMD_LINE_OPTIONS .analytics .metadata \
	       *.xclbin* *.log Work Map_Report.csv \
	       BOOT* *.xo* emu_qemu_scripts *.cdo *.bif compile_log_pre *.elf *.o launch_hw_emu.sh qemu_args.txt *.xsa xnwOut *.pdi baremetal_plat v++.package_summary libsdf* lpd* NOC* pmc* plm* sd_card* sim ilpProblem* sol.db drivers .Xil baremetal_plat $(HOST_EXE) $(ELF) aie_base_graph.xsa.link_summary ${UCODE}

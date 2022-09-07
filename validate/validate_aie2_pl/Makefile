MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/tests/*}')

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


#PLATFORM = /proj/xbuilds/2022.2_daily_latest/xbb/dsadev/opt/xilinx/platforms/xilinx_v70_gen5x8_qdma_2_202220_1/xilinx_v70_gen5x8_qdma_2_202220_1.xpfm

PLATFORM = $(DEVICE)
#PLATFORM = /proj/xbuilds/Vitis_HW_EMU/2022.2/xilinx_v70_hw_emu_202220_1/2022.2_0817_2230/xilinx_v70_hw_emu_202220_1/xilinx_v70_hw_emu_202220_1.xpfm
AIE_DEVICE := xcvc2802-vsvh1760-2LHP-e-S-es1 

#########################compile ADF#############################################
LIBADF  = libadf.a
AIE_CXXFLAGS += --pl-freq=1000 -workdir=$(CUR_DIR)/Work
AIE_CXXFLAGS += -I $(XILINX_VITIS_AIETOOLS)/include
AIE_CXXFLAGS += -I $(CUR_DIR)/aie
AIE_CXXFLAGS += -I $(CUR_DIR)/aie/kernels
AIE_CXXFLAGS += --aiearch=aie-ml
AIE_CXXFLAGS +=  --dataflow --platform=${PLATFORM}
#AIE_CXXFLAGS += --phydevice=$(AIE_DEVICE)
#AIE_CXXFLAGS += -I $(CUR_DIR)
AIE_CXXFLAGS += -v
.PHONY: aie
aie: ${LIBADF}

${LIBADF}:./aie/graph.cpp
	aiecompiler $(AIE_CXXFLAGS) $^

XOS = ./pl_kernels/pl_controller_top.xo
XOS += ./pl_kernels/sender_receiver.xo

$(XOS):./pl_kernels/*.cpp
	make -C pl_kernels/

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
VPP_LDFLAGS += --save-temps --verbose --config ${VPP_SPEC}
ifeq ($(TARGET),hw_emu)
VPP_LDFLAGS += --advanced.param compiler.userPostSysLinkOverlayTcl=post_sys_link.tcl
endif

${XCLBIN}: ${LIBADF} ${VPP_SPEC} ${XOS} | check_vpp
	${VCC} -g -l --platform ${PLATFORM} ${LIBADF} ${XOS} --vivado.prop fileset.sim_1.xsim.simulate.runtime=6000us \
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

${HOST_EXE}: sw/*.cpp
	make -C sw/ CXX=$(CXX)

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

HOST_ARGS = ./Work/ps/c_rts/aie_control_config.json ./Work/reports/dma_lock_report.json
run_hw_emu: ${UCODE} package.hw_emu/launch_hw_emu.sh
	emconfigutil --platform $(PLATFORM) --nd 1
	@echo "Running Emulation: $(HOST_PREAMBLE) ./$(HOST_EXE) $(FINAL_XCLBIN)"
	XCL_EMULATION_MODE=$(TARGET) \
	./$(HOST_EXE) $(FINAL_XCLBIN) $(HOST_ARGS) | tee launch_hw_emu.log
	grep "Test passed" launch_hw_emu.log || exit 1
.PHONY: run

run: aie ${UCODE} ${XCLBIN} host package run_hw_emu
all: aie ${UCODE} ${XCLBIN} host package 
.PHONY: all

.PHONY: clean
clean:
	make clean -C sw/
	rm -rf ./tools me.soln ${LIBADF} _x v++_* *.o.* package.hw_emu aiesimulator_output pl_sample_counts .AIE_SIM_CMD_LINE_OPTIONS .analytics .metadata \
	       *.xclbin* *.log Work Map_Report.csv \
	       BOOT* *.xo* emu_qemu_scripts *.cdo *.bif compile_log_pre *.elf *.o launch_hw_emu.sh qemu_args.txt *.xsa xnwOut *.pdi baremetal_plat v++.package_summary libsdf* lpd* NOC* pmc* plm* sd_card* sim ilpProblem* sol.db drivers .Xil baremetal_plat $(HOST_EXE) $(ELF) aie_base_graph.xsa.link_summary ${UCODE}

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/pl_controller/*}')

check_vivado:
ifeq (,$(wildcard $(XILINX_VIVADO)/bin/vivado))
@echo "Cannot locate Vivado installation. Please set XILINX_VIVADO variable." && false
endif

check_vpp:
ifeq (,$(wildcard $(XILINX_VITIS)/bin/v++))
@echo "Cannot locate Vitis installation. Please set XILINX_VITIS variable." && false
endif

XILINX_VITIS_AIETOOLS := $(XILINX_VITIS)/aietools
TARGET   ?= hw_emu

ifndef BUILD_DIR
$(error Cannot locate the direcotry for xo build. Please set BUILD_DIR variable.)
endif

TEMP_DIR = ${BUILD_DIR}/build.$(TARGET)

XOS      = ${BUILD_DIR}/${KERNEL_NAME}.xo
TEMP_REPORT_DIR := $(TEMP_DIR)/reports
TEMP_LOG_DIR := $(TEMP_DIR)/logs
FLAGS = --hls.clock 300000000:${KERNEL_NAME}
FLAGS += -DCH_NUM=$(CH_NUM)

VCC      = v++

.PHONY: build clean

build: ${XOS}

$(XOS): $(CUR_DIR)/$(KERNEL_NAME).cpp | check_vpp
	${VCC} -c $(FLAGS) --save-temps -t ${TARGET} --platform ${PLATFORM} -k $(KERNEL_NAME) --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) --log_dir $(TEMP_LOG_DIR) $< -o $@ --optimize 2

clean: 
	rm -fr ${BUILD_DIR}/* ./Xil xcd.log xrc.log

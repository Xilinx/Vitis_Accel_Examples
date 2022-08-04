MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/pl_controller/*}')

ifndef DEST_DIR
$(error Cannot decide the destination directory to put xo file. Please set DEST_DIR variable.)
endif

.PHONY: install

install: xo ucode

KNL_SRCFILES = $(wildcard ${CUR_DIR}/ucode_gen/*.cpp)
KNL_SRCFILES += $(wildcard ${CUR_DIR}/ucode_gen/*.hpp)

KERNEL_NAME ?= pl_controller_hw
CH_NUM ?= 2

xo: ${KNL_SRCFILES}
	make -f ${CUR_DIR}/kernel/build.mk BUILD_DIR=$(DEST_DIR) PLATFORM=$(PLATFORM) KERNEL_NAME=$(KERNEL_NAME) CH_NUM=$(CH_NUM) build
	

UCODE_SRCFILES = $(wildcard ${CUR_DIR}/ucode_gen/*.cpp)
UCODE_SRCFILES += $(wildcard ${CUR_DIR}/ucode_gen/*.hpp)

ucode: $(UCODE_SRCFILES) ${CUR_DIR}/ucode_gen/build.mk 
	cp -f $(UCODE_SRCFILES) $(DEST_DIR)
	cp -f ${CUR_DIR}/ucode_gen/build.mk $(DEST_DIR)
	cp -f ${CUR_DIR}/ucode_gen/ucode_gen.sh $(DEST_DIR)
	cp -f ${CUR_DIR}/kernel/utils.hpp $(DEST_DIR)


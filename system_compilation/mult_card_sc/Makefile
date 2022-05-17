ACC_SRCS  := ./src/xacc.cpp
HOST_SRCS := ./src/main.cpp
HOST_ARGS := 0 Makefile ./src/xacc.hpp ./src/xacc.cpp ./src/main.cpp

NCARDS       ?= 2
EMCONFIG_ND  := $(NCARDS)
EXTRA_CFLAGS := -DNCARDS=$(NCARDS)

include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk
test: run

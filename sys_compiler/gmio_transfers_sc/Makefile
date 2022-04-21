ACC_SRCS  := ./src/acc.cpp
HOST_SRCS := ./src/main.cpp   

ifneq (,$(shell echo $(DEVICE) | awk '/zcu102/'))
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc_arm.mk
else
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk
endif
test: run

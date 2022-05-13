HOST_SRCS := src/main.cpp
ACC_SRCS  := src/stream_krnl.cpp 

ifneq (,$(shell echo $(DEVICE) | awk '/zcu102/'))
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc_arm.mk
else
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk
endif
test: run

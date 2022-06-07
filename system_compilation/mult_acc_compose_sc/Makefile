ACC_SRCS = ./src/filter.cpp
HOST_SRCS = ./src/main.cpp

ifneq (,$(shell echo $(DEVICE) | awk '/u2/'))
XILINX_SC_PFM_CONFIG=u2
endif

ifneq (,$(shell echo $(DEVICE) | awk '/u55/'))
XILINX_SC_PFM_CONFIG=u55
endif

ifneq (,$(shell echo $(DEVICE) | awk '/u50/'))
XILINX_SC_PFM_CONFIG=u50
endif

ifneq (,$(shell echo $(DEVICE) | awk '/u280/'))
XILINX_SC_PFM_CONFIG=u50
endif

ifneq (,$(shell echo $(DEVICE) | awk '/u200/'))
XILINX_SC_PFM_CONFIG=u200
endif

ifneq (,$(shell echo $(DEVICE) | awk '/u250/'))
XILINX_SC_PFM_CONFIG=u200
endif

ifneq (,$(shell echo $(DEVICE) | awk '/u25/'))
XILINX_SC_PFM_CONFIG=u25
endif

ifneq (,$(shell echo $(DEVICE) | awk '/zcu102/'))
XILINX_SC_PFM_CONFIG=zcu102
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc_arm.mk
else
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk
endif
test: run

ACC_SRCS  := ./src/eth.cpp
HOST_SRCS := ./src/main.cpp

ifneq (,$(findstring u25,$(DEVICE))) # u25
XILINX_SC_PFM_CONFIG=build_ddr2
else ifneq (,$(findstring u5,$(DEVICE))) # u50 u55
XILINX_SC_PFM_CONFIG=build_hbm32
else ifneq (,$(findstring u280,$(DEVICE))) # u280
XILINX_SC_PFM_CONFIG=build_hbm32
else ifneq (,$(shell echo $(DEVICE) | awk '/u2/')) 
XILINX_SC_PFM_CONFIG=build_bank1
else ifneq (,$(shell echo $(DEVICE) | awk '/zcu102/'))
XILINX_SC_PFM_CONFIG=build_hp
else
XILINX_SC_PFM_CONFIG=build_ddr4
endif

ifneq (,$(shell echo $(DEVICE) | awk '/zcu102/'))
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc_arm.mk
else
include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk
endif

test: run

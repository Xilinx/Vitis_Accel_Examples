ACC_SRCS  := ./src/eth.cpp
HOST_SRCS := ./src/main.cpp

DEVICE_ONLY:= $(shell echo ${DEVICE} | cut -d_ -f2)
DMA_KIND:= $(shell echo ${DEVICE} | cut -d_ -f4)

# we build a switch case to select the include path which is device/platform specific
#  - nodma platform use host include path, independantly of device!
#  - devices with 4 DDR banks use one include path
#  - devices with HBM use another include path
#  - default situation is to use include/DEVICE_ONLY for the include directory

ifneq (,$(filter ${DMA_KIND},nodma))
	INCLUDE_PATH := include/host
else ifneq (,$(filter ${DEVICE_ONLY},u200 u250))
	INCLUDE_PATH := include/ddr4
else ifneq (,$(filter ${DEVICE_ONLY},u50 u50lv u55 u55n u55c u280))
	INCLUDE_PATH := include/hbm
else
	INCLUDE_PATH := include/${DEVICE_ONLY}
endif
EXTRA_CFLAGS += -I ${INCLUDE_PATH}

include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk

test: run

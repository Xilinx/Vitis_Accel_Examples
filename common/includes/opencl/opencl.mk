# Definition of include file locations
ifneq ($(HOST_ARCH), x86)
    xrt_path = $(SYSROOT)/usr
    OPENCL_INCLUDE:= $(xrt_path)/include/xrt
else
    xrt_path = $(XILINX_XRT)
    OPENCL_INCLUDE:= $(xrt_path)/include
endif

VIVADO_INCLUDE:= $(XILINX_VIVADO)/include
opencl_CXXFLAGS=-I$(OPENCL_INCLUDE) -I$(VIVADO_INCLUDE)
OPENCL_LIB:= $(xrt_path)/lib
opencl_LDFLAGS=-L$(OPENCL_LIB) -lOpenCL -pthread

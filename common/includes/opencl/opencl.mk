# Definition of include file locations
OPENCL_INCLUDE:= $(XILINX_XRT)/include/
VIVADO_INCLUDE:= $(XILINX_VIVADO)/include/

opencl_CXXFLAGS=-I$(OPENCL_INCLUDE) -I$(VIVADO_INCLUDE)

OPENCL_LIB:=$(XILINX_XRT)/lib/
opencl_LDFLAGS=-L$(OPENCL_LIB) -lOpenCL -lpthread

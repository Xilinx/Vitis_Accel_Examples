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

ifeq ($(findstring vck, $(DEVICE)), vck)
	VPP_PFLAGS+=--package.sd_dir /proj/xbuilds/2021.1_daily_latest/internal_platforms/sw/versal/xrt
endif
ifeq ($(findstring zcu, $(DEVICE)), zcu)
	VPP_PFLAGS+=--package.sd_dir /proj/xbuilds/2021.1_daily_latest/internal_platforms/sw/zynqmp/xrt
endif
ifeq ($(findstring zc7, $(DEVICE)), zc7)
	VPP_PFLAGS+=--package.sd_dir /proj/xbuilds/2021.1_daily_latest/internal_platforms/sw/zynq/xrt
endif

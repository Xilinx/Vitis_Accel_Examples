#Number of compute units 
NK:= 2
#Number of parallel points
PP:= 96

CXXFLAGS += -D __USE_OPENCL__ -DNUM_CU=$(NK)
VPP_FLAGS += -DPARALLEL_POINTS=$(PP)


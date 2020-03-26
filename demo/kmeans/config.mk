#Number of compute units 
NK:= 2
#Number of parallel points
PP:= 96

CXXFLAGS += -D __USE_OPENCL__ -DNUM_CU=$(NK)
CLFLAGS += -DPARALLEL_POINTS=$(PP)


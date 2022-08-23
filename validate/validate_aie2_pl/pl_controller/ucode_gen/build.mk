MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))

ifeq (,$(wildcard $(XILINX_VIVADO)/bin/vivado))
$(error Cannot locate Vivado installation. Please set XILINX_VIVADO variable.)
endif


ifeq (,$(wildcard $(XILINX_VITIS)/bin/v++))
$(error Cannot locate Vitis installation. Please set XILINX_VITIS variable.)
endif


ifndef IN_DIR
$(error Cannot locate the aie_runtime_control.cpp. Please set IN_DIR variable.)
endif

ifndef OUT_FILE
$(error Cannot locate output file. Please set OUT_FILE variable.)
endif

CH_NUM ?= 2

XILINX_VITIS_AIETOOLS := $(XILINX_VITIS)/aietools
HOST_ARGS := -work $(CUR_DIR)/../ -json $(IN_DIR)/aie_control_config.json -out $(OUT_FILE)

SRCS = $(CUR_DIR)/gen_micro_code.cpp
SRCS += $(IN_DIR)/aie_control.cpp
SRCS += $(CUR_DIR)/aie_runtime_control_wrap.cpp
SRCS += $(CUR_DIR)/aie_runtime_control_impl.cpp

CXXFLAGS += -I$(IN_DIR)

CXXFLAGS += -I$(CUR_DIR)/rapidjson/include
CXXFLAGS += -I$(XILINX_VITIS_AIETOOLS)/include
CXXFLAGS += -I$(XILINX_VITIS_AIETOOLS)/include/drivers/aiengine
CXXFLAGS += -I$(XILINX_VITIS_AIETOOLS)/include/aie_api
CXXFLAGS += -D__PL_CTRL_EXT_BUF_NAME__ -D__CDO__ -DCH_NUM=$(CH_NUM)
CXXFLAGS += -std=c++17

# fix undefine issues
CXXFLAGS += -I$(XILINX_VITIS)/data/embeddedsw/lib/bsp/standalone_v7_6/src/common
CXXFLAGS += -I$(XILINX_VITIS)/data/embeddedsw/XilinxProcessorIPLib/drivers/common_v1_1/src
CXXFLAGS += -I$(XILINX_VITIS)/data/embeddedsw/lib/sw_apps/imgsel/misc
CXXFLAGS += -I$(XILINX_VITIS)/data/embeddedsw/lib/bsp/standalone_v7_6/src/arm/ARMv8/64bit
CXXFLAGS += -I$(XILINX_VITIS)/data/embeddedsw/lib/bsp/standalone_v7_6/src/arm/common/gcc

LDFLAGS += -L$(XILINX_VITIS_AIETOOLS)/lib/lnx64.o
LDFLAGS += -lxaiengine -ladf_api -Wl,--unresolved-symbols=ignore-all -Wno-missing-declarations -g

CXX := g++
CXX_VER := 8.3.0
CXX_V := $(shell echo $(CXX_VER) | awk -F. '{print tolower($$1)}')
ifneq ($(shell expr $(shell echo "__GNUG__" | g++ -E -x c++ - | tail -1) \>= $(CXX_V)), 1)
ifeq (,$(wildcard $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/bin/g++))
$(error [ERROR]: g++ version too old. Please use $(CXX_VER) or above)
else
CXX := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/bin/g++
LD_LIBRARY_PATH := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/lib64:$(LD_LIBRARY_PATH)
endif
endif

LIBRARY_PATH := $(XILINX_VITIS_AIETOOLS)/lib/lnx64.o:$(LD_LIBRARY_PATH)

.PHONY: run compile clean

rapidjson := $(CUR_DIR)/rapidjson/readme.md

$(rapidjson):
	git clone https://github.com/Tencent/rapidjson.git -b v1.1.0 $(CUR_DIR)/rapidjson

run: compile 
	LD_LIBRARY_PATH=$(LIBRARY_PATH) $(CUR_DIR)/ucode_gen.exe $(HOST_ARGS)

compile: $(CUR_DIR)/ucode_gen.exe

$(CUR_DIR)/ucode_gen.exe: $(rapidjson) $(SRCS) $(IN_DIR)/aie_runtime_control.cpp
	$(CXX) $(SRCS) $(CXXFLAGS) $(LDFLAGS) -o $(CUR_DIR)/ucode_gen.exe

clean:
	rm -rf $(CUR_DIR)/ucode_gen.exe $(CUR_DIR)/rapidjson


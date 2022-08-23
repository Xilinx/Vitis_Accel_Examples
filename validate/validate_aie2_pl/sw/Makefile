#
#Copyright 2020 Xilinx, Inc.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#    http://www.apache.org/licenses/LICENSE-2.0
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XF_PROJ_ROOT ?= ${CUR_DIR}/../

HOST_EXE   = ../main.exe
HOST_INC   = -I${XF_PROJ_ROOT}/pl_controller/ -I../

HOST_OBJ   = pl_controller_sw_xrt.o main.o

#Check OS and setting env for xrt c++ api
OSDIST = $(shell lsb_release -i |awk -F: '{print tolower($$2)}' | tr -d ' \t' )
OSREL = $(shell lsb_release -r |awk -F: '{print tolower($$2)}' |tr -d ' \t')

# for centos and redhat
ifneq ($(findstring centos,$(OSDIST)),)
ifeq (7,$(shell echo $(OSREL) | awk -F. '{print tolower($$1)}' ))
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif
else ifneq ($(findstring redhat,$(OSDIST)),)
ifeq (7,$(shell echo $(OSREL) | awk -F. '{print tolower($$1)}' ))
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
endif
endif

CXXFLAGS += -std=c++17 -I$(XILINX_HLS)/include/ -I$(XILINX_VIVADO)/include/ -I${XILINX_XRT}/include/ -g -Wall -c -fmessage-length=0 -I${XILINX_VITIS}/aietools/include ${HOST_INC}
CXXFLAGS += -DPL_CTRL_AXIMM_CH_NUM=$(PL_CTRL_AXIMM_CH_NUM)

#LDFLAGS += -ladf_api_xrt 
LDFLAGS += -lgcc -lc -lxilinxopencl -lxrt_coreutil -lpthread -lrt -ldl -lcrypt -lstdc++ -L${XILINX_XRT}/lib/ -Wl,-rpath-link,$(XILINX_XRT)/lib

.PHONY: clean

all: ${HOST_EXE}

${HOST_EXE}: ${HOST_OBJ}
	${CXX} -o $@ $^ ${LDFLAGS} ${HOST_INC} 

main.o: main.cpp
	${CXX} ${CXXFLAGS} -o $@ $<

pl_controller_sw_xrt.o: ${XF_PROJ_ROOT}/pl_controller/ps_helper/pl_controller_sw_xrt.cpp
	${CXX} ${CXXFLAGS} -o $@ $<

clean:
	rm -rf ${HOST_OBJ} .Xil ${HOST_EXE} 

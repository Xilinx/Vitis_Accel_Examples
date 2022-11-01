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
#PLATFORM = /proj/xbuilds/Vitis_HW_EMU/2022.2/xilinx_v70_hw_emu_202220_1/2022.2_0817_2230/xilinx_v70_hw_emu_202220_1/xilinx_v70_hw_emu_202220_1.xpfm
PLATFORM = $(DEVICE)
TARGET := hw

.PHONY: clean

VPP_FLAGS += -I$(XILINX_VITIS)/aietools/include
VPP_FLAGS += -t $(TARGET)

all:pl_controller_top.xo  sender_receiver.xo
pl_controller_top.xo: pl_controller_top.cpp
	v++ -c ${VPP_FLAGS} --platform ${PLATFORM} -k pl_controller_top --kernel_frequency 250 $< -o $@ --verbose --save-temps

sender_receiver.xo: sender_receiver.cpp
	v++ -c ${VPP_FLAGS} --platform ${PLATFORM} -k sender_receiver --kernel_frequency 250 $< -o $@ --verbose --save-temps

#merge_kernel.xo: merge_kernel.cpp
#	v++ -c ${VPP_FLAGS} --platform ${PLATFORM} -k merge_kernel --kernel_frequency 250 $< -o $@ --verbose --save-temps

clean:
	rm -rf _x *.xo* *.log

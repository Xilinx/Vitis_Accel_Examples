#!/bin/bash
echo "slave launched"
echo "$PWD"
export GCC=g++
export IPC_XTLM=$XILINX_VIVADO/data/emulation/ip_utils/xtlm_ipc/xtlm_ipc_v1_0/cpp/src/
export IPC_XTLM_INC=$XILINX_VIVADO/data/emulation/ip_utils/xtlm_ipc/xtlm_ipc_v1_0/cpp/inc/
export PROTO_PATH=$XILINX_VIVADO/data/simmodels/xsim/2021.2/lnx64/6.2.0/ext/protobuf/
export LD_LIBRARY_PATH=$XILINX_VIVADO/lib/lnx64.o/Default:$XILINX_VIVADO/lib/lnx64.o/:$LD_LIBRARY_PATH
if [ "$1" = "BLOCKING" ];
then
	rm -rf b_receive
	$GCC -g  ./src/b_receive.cpp $IPC_XTLM/common/xtlm_ipc.pb.cc $IPC_XTLM/axis/*.cpp $IPC_XTLM/common/*.cpp -I$IPC_XTLM_INC/ -I$IPC_XTLM_INC/axis -I$IPC_XTLM_INC/common -I$PROTO_PATH/include/ -L$PROTO_PATH/ -lprotobuf -L$XILINX_VIVADO/lib/lnx64.o/ -lrdizlib -L$GCC/../../lib64/ -lstdc++ -lpthread -o b_receive
	./b_receive
else
	rm -rf nb_receive
	$GCC -g  ./src/nb_receive.cpp $IPC_XTLM/common/xtlm_ipc.pb.cc $IPC_XTLM/axis/*.cpp $IPC_XTLM/common/*.cpp -I$IPC_XTLM_INC/ -I$IPC_XTLM_INC/axis -I$IPC_XTLM_INC/common -I$PROTO_PATH/include/ -L$PROTO_PATH/ -lprotobuf -L$XILINX_VIVADO/lib/lnx64.o/ -lrdizlib -L$GCC/../../lib64/ -lstdc++ -lpthread -o nb_receive
	./nb_receive
fi
echo "slave done"

#!/bin/bash
echo "slave launched"
echo "$PWD"
export GCC=g++
export IPC_XTLM=$XILINX_VIVADO/data/emulation/ip_utils/xtlm_ipc/xtlm_ipc_v1_0/cpp/
export PROTO_PATH=$XILINX_VIVADO/data/simmodels/xsim/2022.1/lnx64/6.2.0/ext/protobuf/
export LD_LIBRARY_PATH=$IPC_XTLM/lib:$XILINX_VIVADO/lib/lnx64.o/Default:$XILINX_VIVADO/lib/lnx64.o/:$LD_LIBRARY_PATH
if [ "$1" = "BLOCKING" ];
then
	rm -rf b_receive
	$GCC -g  ./src/b_receive.cpp -I$IPC_XTLM/inc/ -I$PROTO_PATH/include/ -L$PROTO_PATH/ -lprotobuf -L$IPC_XTLM/lib/ -lxtlm_ipc -L$XILINX_VIVADO/lib/lnx64.o/ -lrdizlib -L$GCC/../../lib64/ -lstdc++ -lpthread -o b_receive
	./b_receive
else
	rm -rf nb_receive
	$GCC -g  ./src/nb_receive.cpp -I$IPC_XTLM/inc/ -I$PROTO_PATH/include/ -L$PROTO_PATH/ -lprotobuf -L$IPC_XTLM/lib/ -lxtlm_ipc -L$XILINX_VIVADO/lib/lnx64.o/ -lrdizlib -L$GCC/../../lib64/ -lstdc++ -lpthread -o nb_receive
	./nb_receive
fi
echo "slave done"

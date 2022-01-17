#!/bin/bash
echo "master launched"
echo "$PWD"
export PYTHONPATH=$XILINX_VIVADO/data/emulation/hw_em/lib/python:$XILINX_VIVADO/data/emulation/ip_utils/xtlm_ipc/xtlm_ipc_v1_0/python
echo " py path is $PYTHONPATH "
export LD_LIBRARY_PATH=$XILINX_VIVADO/tps/lnx64/python-3.8.3/lib/:$LD_LIBRARY_PATH
if [ "$1" = "BLOCKING" ];
then
    $XILINX_VIVADO/tps/lnx64/python-3.8.3/bin/python3 ./src/b_test_master.py
else
    $XILINX_VIVADO/tps/lnx64/python-3.8.3/bin/python3 ./src/nb_test_master.py
fi
echo "master done"

#!/bin/bash
echo "master launched"
echo "$PWD"
export PYTHONPATH=$XILINX_VIVADO/data/emulation/hw_em/lib/python:$XILINX_VIVADO/data/python/xtlm_ipc/xtlm_ipc_v1_0
echo " py path is $PYTHONPATH "
export LD_LIBRARY_PATH=$XILINX_VIVADO/tps/lnx64/python-3.8.3/lib/:$LD_LIBRARY_PATH
$XILINX_VIVADO/tps/lnx64/python-3.8.3/bin/python3 ./test_master.py
echo "master done"

export LD_LIBRARY_PATH=/mnt:/tmp:$LD_LIBRARY_PATH
export XILINX_XRT=/usr
export XILINX_VITIS=$PWD
export XCL_EMULATION_MODE=sw_emu
./ddr_bandwidth -x testKernel.xclbin
return_code=$?
if [ $return_code -ne 0 ]; then
echo "ERROR: host run failed, RC=$return_code"
fi
echo "INFO: host run completed."

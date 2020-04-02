export LD_LIBRARY_PATH=/mnt:/tmp:$LD_LIBRARY_PATH
export XCL_EMULATION_MODE=$1
export XILINX_XRT=/usr
export XILINX_VITIS=/mnt
./vadd_stream krnl_stream_adder1.xclbin
return_code=$?
if [ $return_code -ne 0 ]; then
	echo "ERROR: host run failed, RC=$return_code"
fi
echo "INFO: host run completed."

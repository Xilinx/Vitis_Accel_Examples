P2P FPGA to FPGA
=============
PCIe peer-to-peer communication (P2P) is a PCIe feature which enables two PCIe devices to directly transfer data between each other without using host RAM as a temporary storage. The latest version of SDx PCIe platforms support P2P feature via PCIe Resizeable BAR Capability

In this Example we highlight the data transfer between FPGA and FPGA devices.

Following is the real log reported while running the design on U200 platform:

```
Device count - 2
Initializing OpenCL objects
INFO: Importing ./build_dir.sw_emu.xilinx_u200_qdma_201920_1/krnl_mmult.xclbin
INFO: Loaded file
INFO: Created Binary
INFO: Built Program
INFO: Importing ./build_dir.sw_emu.xilinx_u200_qdma_201920_1/krnl_madd.xclbin
INFO: Loaded file
INFO: Created Binary
INFO: Built Program
Set Args FPGA-1

Set Args FPGA-2

Write data to FPGA-1 

Write data to FPGA-2 

Launch FPGA-1

Transferring from FPGA-1 to FPGA-2...
Import FD:7
bytes_written is 262144 is successful 
Launch FPGA-2

Read data back from FPGA-2 

Computed '65536/65536' correct values!
     p2p	    537ms	  11050ms	    4.86%	   116.39MB/s	
Test passed!
```

P2P Bandwidth
=============

PCIe peer-to-peer communication (P2P) is a PCIe feature which enables
two PCIe devices to directly transfer data between each other without
using host RAM as a temporary storage. The latest version of SDx PCIe
platforms support P2P feature via PCIe Resizeable BAR Capability

In this Example we highlight the Synchronous and Asynchronous data
transfer between SSD and FPGA and capture the performance numbers for
p2p transfer, Kernel and XDMA.

Following is the real log reported while running the design on samsung
platform:

::

   ./host copy_kernel.xclbin -w /dev/nvme0n1
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   INFO: Preparing 1048576KB test data in 8 pipelines
   ############################################################
                        Synchronous P2P                         
   ############################################################
   INFO: Kick off test
   HOST -> FPGA(host BO) -> FPGA(p2p BO) -> SSD
    overall      556211ms    100.00%    1841.03MB/s    
        p2p      359223ms     64.58%    2850.60MB/s    
     kernel      199578ms     35.88%    5130.83MB/s    
       XDMA      313203ms     56.31%    3269.45MB/s    
   INFO: Evaluating test result
   INFO: Test passed

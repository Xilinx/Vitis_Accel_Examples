P2P overlap bandwidth Example
=============================

This is simple example to test Synchronous and Asyncronous data transfer between SSD and FPGA.

**KEY CONCEPTS:** `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__, SmartSSD, XDMA

**KEYWORDS:** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U200
 - Alveo U250
 - Alveo U50
 - Alveo U280
 - AWS VU9P F1
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/sample.txt
   src/copy_kernel.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_overlap_bandwidth -x <copy_kernel XCLBIN> -f ./data/sample.txt

DETAILS
-------

PCIe peer-to-peer communication (P2P) is a PCIe feature which enables
two PCIe devices to directly transfer data between each other without
using host RAM as a temporary storage. The latest version of Vitis PCIe
platforms support P2P feature via PCIe Resizeable BAR Capability

In this Example we highlight the Synchronous and Asynchronous data
transfer between SSD and FPGA and capture the performance numbers for
p2p transfer, Kernel and XDMA.

By default synchronous flow is ran and following is the real log 
reported while running the design on samsung platform:

::

   ./p2p_overlap_bandwidth -x copy_kernel.xclbin -p /dev/nvme0n1
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   INFO: Preparing 1048576KB test data in 8 pipelines
   ############################################################
                        Synchronous P2P                         
   ############################################################
   INFO: Kick off test
   HOST -> FPGA(host BO) -> FPGA(p2p BO) -> SSD
    overall      556211ms    100.00%    1861.03MB/s    
        p2p      359223ms     57.58%    3250.60MB/s    
     kernel      199578ms     207.88%    871.83MB/s    
       XDMA      313203ms     99.11%    1866.45MB/s    
   INFO: Evaluating test result
   INFO: Test passed

If we need to run the asynchronous flow, build the host with
ENABLE_ASYNC_FLOW=yes : 

.. code:: cpp

   make host ENABLE_ASYNC_FLOW=yes

Following is the real log reported while running the design
on samsung platform for asynchronous flow:

:: 

    ./p2p_overlap_bandwidth -x copy_kernel.xclbin -p /dev/nvme0n1
    INFO: Successfully opened NVME SSD /dev/nvme0n1
    INFO: Preparing 1048576KB test data in 8 pipelines
    ############################################################
                         Synchronous P2P                         
    ############################################################
    INFO: Kick off test
    HOST -> FPGA(host BO) -> FPGA(p2p BO) -> SSD
    overall      551211ms    100.00%    1862.23MB/s    
        p2p      357223ms     58.58%    3200.30MB/s    
     kernel      192578ms     211.88%    879.89MB/s    
       XDMA      315203ms     99.31%    1867.35MB/s    
    INFO: Evaluating test result
    INFO: Test passed
    ############################################################
                         Asynchronous P2P                         
    ############################################################
    INFO: Kick off test
    HOST -> FPGA(host BO) -> FPGA(p2p BO) -> SSD
    overall      556211ms    100.00%    1548.03MB/s    
        p2p      359223ms     61.58%    3371.60MB/s    
     kernel      199578ms     198.88%    697.83MB/s    
       XDMA      313203ms     98.23%    1806.45MB/s    
    INFO: Evaluating test result
    INFO: Test passed



For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
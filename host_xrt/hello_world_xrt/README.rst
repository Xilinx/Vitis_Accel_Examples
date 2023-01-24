Hello World XRT (XRT Native API's)
==================================

This is simple example of vector addition to describe the usage of XRT Native API's. The kernel uses HLS Dataflow which allows the user to schedule multiple task together to achieve higher throughput.

**KEY CONCEPTS:** `XRT Native API <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__, `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Data-driven-Task-level-Parallelism>`__

**KEYWORDS:** `xrt::device <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-the-Device-ID-and-Loading-the-XCLBIN>`__, load_xclbin, `xrt::bo <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Host-Applications-with-XRT-API>`__, `xrt::kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__, `map <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Software-and-PL-Kernels>`__, `sync <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Software-and-PL-Kernels>`__, `XCL_BO_SYNC_BO_TO_DEVICE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Host-Applications-with-XRT-API>`__, `XCL_BO_SYNC_BO_FROM_DEVICE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Host-Applications-with-XRT-API>`__, `gmem <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__, `#pragma HLS INTERFACE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Pragmas>`__, `dataflow <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Dataflow>`__, `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hello_world_xrt -x <vadd XCLBIN>

DETAILS
-------

This is a simple hello world example to explain the XRT Native API's

- Device and XCLBIN APIs

::

    xrt::device(unsigned int didx)
    This call open the device and return device handle.

    load_xclbin(binaryFile)
    This function reads the file from disk and loads the xclbin. This 
    will return the UUID of the xclbin.


    
- Buffer APIs

::

    xrt::bo(xclDeviceHandle dhdl, size_t size, memory_group grp)
    The buffer type is default buffer object with host buffer and 
    device buffer. The host buffer is allocated and managed by XRT.
    
    void sync(xclBOSyncDirection dir, size_t sz, size_t offset)
    Synchronize specified size bytes of buffer starting at specified offset.

    void *map()
    Map the contents of the buffer object into host memory



- Kernel APIs

::

    xrt::kernel(const xrt::device &device, const xrt::uuid &xclbin_id, const std::string &name)
    A kernel object represents a set of instances matching a specified name.
    The kernel is created by finding matching kernel instances in the 
    currently loaded xclbin. Most interaction with kernel objects are through
    xrt::run objects created from the kernel object to represent an execution of the kernel 

To visit github.io of this repository, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
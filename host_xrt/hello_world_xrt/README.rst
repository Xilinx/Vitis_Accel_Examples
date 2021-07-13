Hello World XRT (XRT Native API's)
==================================

This is simple example of vector addition to describe the usage of XRT Native API's.

**KEY CONCEPTS:** `XRT Native API <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#jln1620691667890>`__

**KEYWORDS:** `xrt::device <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#zja1524097906844>`__, load_xclbin, `xrt::bo <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185>`__, `xrt::kernel <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#lqq1622157666187>`__, `map <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185>`__, `sync <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185>`__, XCL_BO_SYNC_BO_TO_DEVICE, XCL_BO_SYNC_BO_FROM_DEVICE

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

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


    

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
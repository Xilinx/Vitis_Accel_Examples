Hello World XRT (XRT Native API's)
===========

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


    

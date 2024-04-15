Hello World Python
==================

This is a simple hello world example to explain python based host code.

- Device and XCLBIN APIs

::

    d = pyxrt.device(index)
    This call open the device and return device handle.

    xbin = pyxrt.xclbin(bitstreamFile)
    uuid = d.load_xclbin(xbin)
    This function reads the file from disk and loads the xclbin. This 
    will return the UUID of the xclbin.
    
- Buffer APIs

::

    boHandle1 = pyxrt.bo(d, DATA_SIZE, pyxrt.bo.normal, kHandle.group_id(0)) 
    The buffer type is default buffer object with host buffer and 
    device buffer. The host buffer is allocated and managed by XRT.
    
    boHandle1.sync(pyxrt.xclBOSyncDirection.XCL_BO_SYNC_BO_TO_DEVICE, DATA_SIZE, 0)    
    Synchronize specified size bytes of buffer starting at specified offset.

    bo1 = boHandle1.map()
    Map the contents of the buffer object into host memory

- Kernel APIs

::

    run = kHandle(boHandle1, boHandle2, boHandle3, COUNT)
    state = run.wait()
    A kernel object represents a set of instances matching a specified name.
    The kernel is created by finding matching kernel instances in the 
    currently loaded xclbin.

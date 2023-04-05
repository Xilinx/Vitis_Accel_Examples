Hello World Python(HLS C/C++ Kernel)
====================================

This is simple example to describe the usage of python based host code in Vitis Environment. The kernel is simple vector addition using HLS.

**KEY CONCEPTS:** Python Host

**KEYWORDS:** pyxrt

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
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Versal V70

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.py
   src/utils_binding.py
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./src/host.py -k <vadd XCLBIN>

DETAILS
-------

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

To visit github.io of this repository, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
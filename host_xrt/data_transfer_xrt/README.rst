Data Transfer XRT (XRT Native API's)
====================================

This example illustrates transferring of data to and from the FPGA using xrt native api's

**KEY CONCEPTS:** `XRT Native API <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__, `Data Transfer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__, `Write Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__, `Read Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

**KEYWORDS:** `xrt::bo::write <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Host-and-Kernels>`__, `xrt::bo::read <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Host-and-Kernels>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/dummy_kernel.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./data_transfer_xrt -x <dummy_kernel XCLBIN>

DETAILS
-------

This example illustrates how to transfer data back and forth between
host and device using buffer read/write API's
  
    - ``xrt::bo::read``
    - ``xrt::bo::write``


To transfer the data from the host to the device, the user first needs to update the host-side buffer backing pointer followed by a DMA transfer to the device. 

The following APIs are used for the above tasks

    1. ``xrt::bo::write``
    2. ``xrt::bo::sync`` with flag ``XCL_BO_SYNC_BO_TO_DEVICE``

.. code:: cpp

    std::cout << "Write the input data\n";
    input_buffer.write(buff_in_data);
    
    std::cout << "synchronize input buffer data to device global memory\n";
    input_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);


To transfer the data from the device to the host, the steps are reverse, the user first needs to do a DMA transfer from the device followed by the reading data from the host-side buffer backing pointer. 

The following APIs are used for the above tasks

    1. ``xrt::bo::sync`` with flag ``XCL_BO_SYNC_BO_FROM_DEVICE``
    2. ``xrt::bo::read``    

.. code:: cpp

    std::cout << "synchronize output buffer data from the device" << std::endl;
    output_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Read the output data\n";
    output_buffer.read(buff_out_data);

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
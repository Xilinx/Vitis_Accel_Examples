Data Transfer XRT (XRT Native API's)
====================================

This example illustrates transferring of data to and from the FPGA using xrt native api's

**KEY CONCEPTS:** `XRT Native API <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#jln1620691667890>`__, `Data Transfer <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132>`__, `Write Buffers <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132>`__, `Read Buffers <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132>`__

**KEYWORDS:** `xrt::bo::write <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185>`__, `xrt::bo::read <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

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
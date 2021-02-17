Data Transfer XRT
==================

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

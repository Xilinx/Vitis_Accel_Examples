Register Access (XRT Native API's)
==================================

To read and write from the AXI-Lite register space corresponding to a CU, the CU must be opened in exclusive mode (in shared mode, multiple processes can access the CU's address space, hence it is unsafe if they are trying to access/change registers at the same time leading to a potential race behavior). The required APIs for kernel register read and write are
  
    - ``xrt::kernel::read_register``
    - ``xrt::kernel::write_register``


.. code:: c
      :number-lines: 67
       
           int read_data; 
           int write_data = 7;

           auto krnl = xrt::kernel(device, uuid, "increment:{increment_1}", xrt::kernel::cu_access_mode::exclusive);
 
           //Write the input data using write_register
           auto write_offset = krnl.offset(2);
           krnl.write_register(write_offset,write_data);
    
           //Read the output using read_register
           auto read_offset = krnl.offset(3);
           read_data = krnl.read_register(read_offset);
 

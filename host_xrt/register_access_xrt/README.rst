Register access (XRT Native API's)
==================================

This is simple example which demonstrate register read & write using native xrt api's.

**KEYWORDS:** read_register, write_register, cu_access_mode, exclusive

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./register_access_xrt -x <increment XCLBIN>

DETAILS
-------

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
 

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
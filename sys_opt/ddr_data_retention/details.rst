DDR Data Retention
==================

This example shows how host can swap between kernels and share the same buffer between two kernels. Since the DDR can hold the data between successive XCLBIN loads, the user can dump data to the DDR using the first xclbin and then reuse the same in successive xclbins directly. This reduces the number of transfers between host and device by 2 for each reuse.

In the design, the host application first programs the device with the binary containing ``krnl_vmult`` and executes the operation.

Before the first kernel call, the common ``d_mul`` buffer is created using ``NULL`` buffer. This is because, it will be populated by the first kernel and then reused by the second kernel and thus, would not require any host side memory to map to.

.. code:: cpp

   OCL_CHECK(err,
             cl::Buffer d_mul(context, CL_MEM_HOST_NO_ACCESS | CL_MEM_READ_WRITE, sizeof(int) * LENGTH, NULL, &err));

The first kernel is run with the following arrangement -

.. code:: cpp

   OCL_CHECK(err, err = krnl_vmul.setArg(0, d_a));
   OCL_CHECK(err, err = krnl_vmul.setArg(1, d_b));
   OCL_CHECK(err, err = krnl_vmul.setArg(2, d_mul));
   OCL_CHECK(err, err = krnl_vmul.setArg(3, vector_length));

   OCL_CHECK(err, err = q.enqueueMigrateMemObjects({d_a, d_b}, 0 /* 0 means from host*/));
   
   // This function will execute the kernel on the FPGA
   OCL_CHECK(err, err = q.enqueueTask(krnl_vmul));
   OCL_CHECK(err, err = q.finish());

Once the first kernel finishes the operation, the host reprograms the 2nd Binary.

.. code:: cpp

   OCL_CHECK(err, cl::Program program(context, devices, vadd_bins, NULL, &err));

The second kernel is run with the following arrangement -

.. code:: cpp

   OCL_CHECK(err, krnl_vadd.setArg(0, d_mul));
   OCL_CHECK(err, krnl_vadd.setArg(1, d_mul));
   OCL_CHECK(err, krnl_vadd.setArg(2, d_add));
   OCL_CHECK(err, krnl_vadd.setArg(3, vector_length));
   
   // This function will execute the kernel on the FPGA
   OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));
   
   OCL_CHECK(err, err = q.enqueueMigrateMemObjects({d_add}, CL_MIGRATE_MEM_OBJECT_HOST));
   OCL_CHECK(err, err = q.finish());

In order to enable this feature for emulation, the following config settings are required in the ``xrt.ini`` - 

:: 

   [Emulation]
   enable_memory_persistence=true

In order to enable this feature on the board, the following command needs to be given - 

:: 

   xbmgmt config --enable_retention [--ddr] [--card bdf]


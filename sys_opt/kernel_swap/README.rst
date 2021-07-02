Kernel Swap
===========

This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.

**KEY CONCEPTS:** Handling Buffer sharing across multiple binaries, Multiple Kernel Binaries

**KEYWORDS:** `enqueueMigrateMemObjects <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132>`__, `CL_MIGRATE_MEM_OBJECT_HOST <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#kcd1556227380924>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   flat
   zcu102_base_20
   zcu104_base_20
   vck
   zc7
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd.cpp
   src/krnl_vmul.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./kernel_swap -x1 <krnl_vmul XCLBIN> -x2 <krnl_vadd XCLBIN>

DETAILS
-------

This example shows how host can swap the kernels and share the same
buffer between two kernels which exist in separate binary containers.
All the Vitis platforms do not persist the buffer data if device is
reprogrammed by different binary. So host has to migrate data from
device to host memory before swapping the next binary. After
re-programming the binary, host has to migrate the buffer back to device
for next kernel operation.

In design, host application first programs the device using binary
containing ``krnl_vmult`` and executes the operation.

Pointer ``h_temp`` in host is used to store the result of first kernel
and provide the data to next kernel operation.

During first kernel call, ``d_mul`` buffer is created using ``h_temp``
buffer.

.. code:: cpp

   cl::Buffer d_mul(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, sizeof(int) * LENGTH, h_temp.data(), &err);

Once kernel finishes the operation, result of the kernel will be read
from device to host into ``h_temp`` buffer.

.. code:: cpp

   q.enqueueMigrateMemObjects({d_mul},CL_MIGRATE_MEM_OBJECT_HOST);

After this, Host is ready to reprogram the 2nd Binary.

.. code:: cpp

   cl::Program program(context, devices, vadd_bins);

After reprogramming with new binary, a new buffer ``d_temp`` will be
created using same ``h_temp`` host pointer.

.. code:: cpp

   cl::Buffer d_temp(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,   sizeof(int) * LENGTH, h_temp.data());

And data will be transfered from host to device for 2nd kernel’s
execution.

.. code:: cpp

   q.enqueueMigrateMemObjects({d_temp}, 0/* 0 means from host*/);

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
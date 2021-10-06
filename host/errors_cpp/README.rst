Error Handling (CPP)
====================

This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.

**KEY CONCEPTS:** OpenCL Host API, Error handling

**KEYWORDS:** CL_SUCCESS, CL_DEVICE_NOT_FOUND, CL_DEVICE_NOT_AVAILABLE, CL_INVALID_VALUE, CL_INVALID_KERNEL_NAME, CL_INVALID_BUFFER_SIZE

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./errors_cpp <vector_addition XCLBIN>

DETAILS
-------

This example discusses the different reasons for errors in OpenCL C++
and how to handle them at runtime. Host program uses APIs like
``clCreateBuffer`` wrapped by cl::buffer which may generate errors on
unsuccessful execution. Common error codes and possible reasons can be
seen in the launch log of the sw-emulation for this example.

Errors codes, explanations and APIs which generate these errors are
documented at
`Errors <https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/errors.html>`__.

Error handling in OpenCL is performed using the cl_int specifier. OpenCL
functions either return or accept pointers to cl_int types to indicate
if an error occurred.

.. code:: cpp

   cl_int err;

Error while trying to get Platform IDs without valid values

.. code:: cpp

   err = cl::Platform::get(&platforms);

No platforms were found. This could be caused because the OpenCL icd was
not installed in /etc/OpenCL/vendors directory.

Error when trying to access device type which does not exist

.. code:: cpp

   err = platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);

This error appears when we try to create a device and no devices are
found on the platform. In this case we passed ``CL_DEVICE_TYPE_CPU`` as
the device type which is not available on the provided platform.

Error while creating Context

.. code:: cpp

   cl::Context context(0, props, nullptr, nullptr, &err);

Most ``clCreate*`` calls accept error codes as their last parameter
instead of returning the error value. This error occurred because we
passed 0 for the num_devices variable.

Error while creating Program with Binary:

.. code:: cpp

   cl::Program program(context, devices, invalid_bin, nullptr, &err);

Errors caused during program creation are usually due to invalid
binaries. The binary may be targeting a different platform. It may also
have been corrupted or incorrectly read from disk.

Error while creating Kernel:

.. code:: cpp

   cl::Kernel kernel(program, "InvalidKernelName", &err);

Errors calling clCreateKernel are usually caused if the name passed into
the function does not match a kernel in the binary.

Error while creating Buffers:

.. code:: cpp

   cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, 0, nullptr, &err);

There can be several reasons for buffer creation to fail. It could be
because device could not allocate enough memory for this buffer. The
pointer could be null and either CL_MEM_USE_HOST_PTR or
CL_MEM_COPY_HOST_PTR are passed into the flags parameter. In this case
we passed zero(0) as the size of the buffer.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
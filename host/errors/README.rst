Error Handling (C)
==================

This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.

**KEY CONCEPTS:** OpenCL API, Error handling

**KEYWORDS:** CL_SUCCESS, CL_DEVICE_NOT_FOUND, CL_DEVICE_NOT_AVAILABLE

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

   ./errors <vector_addition XCLBIN>

DETAILS
-------

This example explains the different reasons for errors in OpenCL C and
how to catch them at runtime.

Host program uses various APIs like
``clGetPlatformIDs``,\ ``clGetPlatformInfo`` which may generate errors
on unsuccessful execution.

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

   err = clGetPlatformIDs(0, nullptr, nullptr));

Received Expected Error calling above call from ``clGetPlatformIDs``
This error is usually caused by a failed OpenCL installation or if both
the platforms and num_platforms parameters are null.

Error when trying to access device type which does not exist

.. code:: cpp

   err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 0, nullptr, &num_devices));

This error appears when we try to create a device and no devices are
found on the platform. In this case we passed ``CL_DEVICE_TYPE_CPU`` as
the device type which is not available on the provided platform.

Error while creating Context

.. code:: cpp

   cl_context context = clCreateContext(props, 0, &device_id, nullptr, nullptr, &err);

Most ``clCreate*`` calls accept error codes as their last parameter
instead of returning the error value. This error occurred because we
passed 0 for the num_devices variable.

Error while creating Program with Binary:

.. code:: cpp

   cl_program program = clCreateProgramWithBinary(context,1,&device_id,&binary_size,&incorrect_binary_data,nullptr,&err);

Errors caused during program creation are usually due to invalid
binaries. The binary may be targeting a different platform. It may also
have been corrupted or incorrectly read from disk.

Error while creating Kernel:

.. code:: cpp

   cl_kernel kernel = clCreateKernel(program, "InvalidKernelName", &err);

Errors calling clCreateKernel are usually caused if the name passed into
the function does not match a kernel in the binary.

Error while creating Buffers:

.. code:: cpp

   cl_mem buffer_a = clCreateBuffer(context, CL_MEM_READ_ONLY, 0, nullptr, &err);

There can be several reasons for buffer creation to fail. It could be
because device could not allocate enough memory for this buffer. The
pointer could be null and either CL_MEM_USE_HOST_PTR or
CL_MEM_COPY_HOST_PTR are passed into the flags parameter. In this case
we passed zero(0) as the size of the buffer.

Error while Writing Data into Buffer:

.. code:: cpp

   err = clEnqueueWriteBuffer(command_queue,buffer_a,CL_FALSE,0,size + 1,A.data(),0,nullptr,nullptr))

Errors calling ``clEnqueueWriteBuffer`` tend to occur due to invalid
pointers or invalid size of the transfer. Make sure that the host
pointer is correct and that you are transferring less than the size of
the buffer. Here we tried to transfer data that was larger than the size
of the buffer.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
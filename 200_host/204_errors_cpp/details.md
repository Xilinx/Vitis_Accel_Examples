Error Handling (CPP)
===================
This example discusses the different reasons for errors in OpenCL C++ and how to handle them at runtime.

Host program uses APIs like `clCreateBuffer` wrapped by cl::buffer  which may generate errors on unsuccessful execution. Common error codes  and possible reasons can be seen in the launch log of the sw-emulation for this example.

`[XRT] ERROR: size==0
Recoverable Error calling clCreateBuffer: CL_INVALID_BUFFER_SIZE
There can be several reasons for buffer creation to fail. It could be because device could not allocate enough memory for this buffer. The pointer could be null and either CL_MEM_USE_HOST_PTR or CL_MEM_COPY_HOST_PTR are passed into the flags parameter. In this case we passed zero(0) as the size of the buffer. We intentionally caused this error so we will not be exiting the program.`



					       SW-emulation log


Errors codes, explanations and APIs which generate these errors are documented at
[Errors](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/errors.html).


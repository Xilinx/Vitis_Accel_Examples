Error Handling (CPP)
===================
This example discusses the different reasons for errors in OpenCL C++ and how to handle them at runtime.

Host program uses APIs like `clCreateBuffer` wrapped by cl::buffer  which may generate errors on unsuccessful execution. Common error codes  and possible reasons can be seen in the launch log of the sw-emulation for this example.

Errors codes, explanations and APIs which generate these errors are documented at
[Errors](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/errors.html).

Error Handling (C)
===================
This example discusses the different reasons for errors in OpenCL C and how to handle them at runtime.

Host program uses various APIs like `clGetPlatformIDs`,`clGetPlatformInfo`  which may generate errors on unsuccessful execution. Common error codes  and possible reasons can be seen in the launch log of the sw-emulation for this example.

Following text is from the `SW-Emulation log` :

`ERROR: number of devices is 0`

`Recoverable Error calling clCreateContext: CL_INVALID_VALUE
Most clCreate* calls accept error codes as their last parameter instead of returning the error value. This error occured because we passed 0 for the num_devices varaible. We intentionally threw this error so we will not be exiting the program.`

Errors codes, explanations and APIs which generate these errors are documented at
[Errors](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/errors.html).


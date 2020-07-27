Multiple Compute Units
======================

This example explains how to create multiple instances of a kernel and
execute them concurrently.

For the same kernel to be instantiated into multiple compute units,
``nk`` flag is used to specify the number of compute units in a ``.ini``
config file as shown below:

::

   [connectivity]
   nk=vadd:4

For kernels to execute concurrently, command queue is initialized with
out of order execution mode enabled.

\```c++ OCL_CHECK(err, q = cl::CommandQueue(context, device,
CL_QUEUE_PROFILING_ENABLE \| CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
&err));

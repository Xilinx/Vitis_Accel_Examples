Two Parallel Read/Write on Local Memory (OpenCL Kernel)
=======================================================

This is a simple example to demonstrate how to utilize both ports of
local memory in kernels.

Kernel’s local memory is usually ``BRAM`` which has two ports for
read/write. In loops where one iteration doesn’t depend on previous
iterations, two ports can be used to improve the performance of the
kernel.

Two ports can be utilized concurrently by using ``opencl_unroll_hint``.
The unroll attribute transforms loops by creating multiples copies of
the loop body in the register transfer level (RTL) design, which allows
some or all loop iterations to occur in parallel.

.. code:: cpp

    __attribute__((opencl_unroll_hint(2)))

Here loop is unrolled by a factor of 2 thus two iterations of the loop
are executed concurrently. In this case, two ports of BRAM will be
utilized rather than 1 reducing the total loop latency by half
approximately.

Streaming Kernel to Kernel
==========================

This example demonstrates how two kernels ``kernel_stream_vadd`` and
``kernel_stream_vmult`` can communicate through a stream interface.
Rather than loading the results from producer kernel
``kernel_stream_vadd`` to global memory and then to consumer kernel
``kernel_stream_vmult`` we can use stream to transfer this data
directly. It reduces the latency of the application and also reduces the
memory utilization.

For kernel to kernel streaming transfer, kernel to kernel data transfer
ports do not need ``clSetKernelArg`` from host code.

Producer kernel ``krnl_stream_vadd`` streaming output port needs to be
connected to consumer kernel ``krnl_stream_vmult`` stream input port
during ``v++`` using ``krnl_stream_vadd_vmult.ini`` file as below:

::

   [connectivity]
   stream_connect=krnl_stream_vadd_1.output:krnl_stream_vmult_1.b

This stream connection specify that ``output`` streaming port of
``krnl_stream_vadd_1`` will be connected to ``b`` streaming port of
``krnl_stream_vmult`` during system linker run. This connectivity file
is provided to v++ command as below:

::

    --config krnl_stream_vadd_vmult.ini

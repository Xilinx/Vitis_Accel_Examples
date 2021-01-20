Streaming Kernel to Kernel Memory Mapped
========================================

This example demonstrates how kernels can have memory mapped inputs
along with stream interface from one kernel to another.

``kernel_stream_vadd`` has two memory mapped inputs and one stream
output. ``kernel_stream_vmult`` has one memory mapped input along with
kernel to kernel stream from ``kernel_stream_vadd`` which acts as its
second input.

Kernel arguments are not required to be setup in host code for kernel to
kernel streaming interfaces. Argument 2 for ``kernel_vadd`` and argument
1 for ``kernel_vmult`` are not declared.

.. code:: cpp

   err = krnl_vadd.setArg(0, buffer_in1);
   err = krnl_vadd.setArg(1, buffer_in2);
   err = krnl_vadd.setArg(3, size));
   err = krnl_vmult.setArg(0, buffer_in3));
   err = krnl_vmult.setArg(2, buffer_output));
   err = krnl_vmult.setArg(3, size));

Producer kernel stream output port must be connected to consumer kernel
stream input port during the ``v++`` linking stage.

::

   --config krnl_stream_vadd_vmult.cfg 

Content of ``krnl_stream_vadd_vmult.cfg`` file is below:

::

   [connectivity]
   stream_connect=krnl_stream_vadd_1.out:krnl_stream_vmult_1.in2:64 

Above specify that ``out`` port of ``krnl_stream_vadd_1`` kernel is
connected to ``in2`` port of ``krnl_stream_vmult_1``. The FIFO depth of this stream is 64.

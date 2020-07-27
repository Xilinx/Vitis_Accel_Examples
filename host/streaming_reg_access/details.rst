Streaming Reg Access(HLS C/C++)
===============================

Sometimes the kernel does not need to be controlled by the host. For
example, if the kernel is only communicating through the stream, it only
works when the data is available at its input through the stream, and
the kernel stalls when there is no data to process, waiting for new data
to arrive through the stream to start working again. These type of
kernels has no control signal connected to the AXI4-Lite Slave
interface.

The free-running kernel only contains ``hls::stream`` inputs and
outputs. The recommended coding guidelines include:

*Consider a kernel with AP_CTRL_NONE only when it has no memory mapped
input and output.*\ There is no need to start the kernel by
clEnqueueTask or clEnqueueNDRangeKernel from the host. *Host
communicates with a continuously running kernel by the stream read and
write requests, if necessary.*\ Dont use clSetKernelArg to pass scalar
argument to ap_ctrl_none kernel, only use xclRegWrite (API implemented
in 2019.2) API.

To read and write register values from the AXI4-Lite Slave interface,
the new APIs xclRegRead/xclRegWrite must be used These APIs require
exclusive CU context reservation via API xclOpenContext as shown in the
code sample below.

::

   xclOpenContext(device_handle, xclbin_id, cu_index, false);
   xclRegRead(device_handle, cu_index, offset, &data);
   xclRegWrite(device_handle, cu_index, offset, data);
   xclCloseContext(device_handle, xclbin_id, cu_index);

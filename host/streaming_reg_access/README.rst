Control and Status Register access continuously streaming kernel (HLS C/C++)
============================================================================

This is simple example which demonstrate streaming free-running kernel with scalar input and output.

**KEY CONCEPTS:** `Free Running Kernel <https://www.xilinx.com/html_docs/xilinx2020_2/vitis_doc/streamingconnections.html#uug1556136182736>`__

**KEYWORDS:** `xclRegRead <https://www.xilinx.com/html_docs/xilinx2020_2/vitis_doc/obl1532064985142.html#tpi1504034339424__section_tnh_pks_rx>`__, `xclRegWrite <https://www.xilinx.com/html_docs/xilinx2020_2/vitis_doc/obl1532064985142.html#tpi1504034339424__section_tnh_pks_rx>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   u30
   u250
   zc
   vck
   xdma
   qep
   aws-vu9p-f1
   samsung
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_reg_access -x <krnl_incr XCLBIN> -i 5

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
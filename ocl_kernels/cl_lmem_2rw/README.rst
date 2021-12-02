Two Parallel Read/Write on Local Memory (OpenCL Kernel)
=======================================================

This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory.

**KEY CONCEPTS:** `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__, `2port BRAM Utilization <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Port-Level-I/O-Memory-Interface-Protocol>`__, two read/write Local Memory

**KEYWORDS:** `opencl_unroll_hint <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Attributes>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc
 - Samsung U.2 SmartSSD

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_lmem_2rw <vadd XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
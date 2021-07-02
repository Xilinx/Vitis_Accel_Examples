Dataflow SubFunction OpenCL (OpenCL Kernel)
===========================================

This is simple example of vector addition to demonstrate how OpenCL Dataflow allows user to run multiple sub functions together to achieve higher throughput.

**KEY CONCEPTS:** SubFunction Level Parallelism

**KEYWORDS:** `xcl_dataflow <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#dmj1504034358681>`__, `xclDataflowFifoDepth <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#fgh1568640841739__section_mh4_qf4_bjb>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/adder.cl
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_dataflow_subfunc <adder XCLBIN>

DETAILS
-------

This example demonstrates how ``xcl_dataflow`` attribute can be used to
implement task level parallelism for subfunctions inside a function.

``adder`` kernel uses a function ``run_subfunc`` which has 3
subfunctions ``read_input``, ``compute_add`` and ``write_result``.
``xcl_dataflow`` is used here to parallelize these subfunctions inside
``run_subfunc``.

.. code:: cpp

   __attribute__ ((xcl_dataflow))
   void run_subfunc(__global int *in, __global int *out, int inc, int size)
   {
       int buffer_in[BUFFER_SIZE];
       int buffer_out[BUFFER_SIZE];

       read_input(in,buffer_in,size);
       compute_add(buffer_in,buffer_out,inc,size);
       write_result(out,buffer_out,size);
   }

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
Hello World (HLS C/C++ Kernel)
==============================

This is simple example of vector addition to describe how to use HLS kernels in Vitis Environment. The kernel uses HLS Dataflow which allows the user to schedule multiple task together to achieve higher throughput.

**KEY CONCEPTS:** `HLS C Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/C/C-Kernel>`__, `OpenCL Host APIs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Programming>`__, `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Data-driven-Task-level-Parallelism>`__

**KEYWORDS:** `gmem <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__, `#pragma HLS INTERFACE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Pragmas>`__, `m_axi <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Defining-Interfaces>`__, `dataflow <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Dataflow>`__, `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hello_world <vadd XCLBIN>

DETAILS
-------

This example is a simple hello world example to explain the Host and Kernel code structure. Here a simple ``vadd`` kernel is used to explain the same. The kernel uses HLS Dataflow which allows the user to schedule multiple task together to achieve higher throughput.

Vitis kernel can have one s_axilite interface which will be used by host application to configure the kernel. All the global memory access arguments are associated to m_axi(AXI Master Interface) as below:

.. code:: cpp	

   #pragma HLS INTERFACE m_axi port = in1 bundle = gmem0
   #pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
   #pragma HLS INTERFACE m_axi port = out bundle = gmem0

Multiple interfaces can be created based on the requirements. For example when multiple memory accessing arguments need access to global memory simultaneously, user can create multiple master interfaces and can connect to different arguments.

Usually data stored in the array is consumed or produced in a sequential manner, a more efficient communication mechanism is to use streaming data as specified by the STREAM pragma, where FIFOs are used instead of RAMs.

Vector addition in kernel is divided into 4 sub-tasks(read input 1, read input 2 , compute_add and write) which are then performed concurrently using ``Dataflow``.

.. code:: cpp

   #pragma HLS dataflow
       load_input(in1, in1_stream, vSize);
       load_input(in2, in2_stream, vSize);
       compute_add(in1_stream, in2_stream, out_stream, vSize);
       store_result(out, out_stream, vSize);

To visit github.io of this repository, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
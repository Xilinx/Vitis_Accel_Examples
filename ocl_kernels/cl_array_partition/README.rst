Array Partitioning (OpenCL Kernel)
==================================

This example shows how to use array partitioning to improve performance of a kernel

**KEY CONCEPTS:** `Kernel Optimization <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html>`__, `Array Partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#swq1539734225427>`__

**KEYWORDS:** `xcl_array_partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#kal1504034361186>`__, `complete <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/matmul.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_array_partition <matmul XCLBIN>

DETAILS
-------

This example demonstrates how ``array partition`` in OpenCL kernels can
improve the performance of an application. Operations like matrix
multiplication require multiple accesses to data in memory for a single
iteration. Two kernels are executed, ``matmul_partition`` with the array
partitions and ``matmul`` without the array partitions and performance
is compared.

``xcl_array_partition`` is used to partition an array into multiple
smaller arrays or memories with more number of ports for read and write
operations. This results in improved throughput of the design.

Arrays can be partitioned in three ways, ``cyclic`` where elements are
put into smaller arrays one by one in the interleaved manner until the
whole array is partitioned, ``block`` where elements are put into
smaller arrays from continuous blocks of original array and ``complete``
where array is decomposed into individual elements each having its own
read/write ports.

.. code:: cpp

       int B[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));
       
       int C[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));

       int temp_sum[MAX_SIZE]    __attribute__((xcl_array_partition(complete, 1)));

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
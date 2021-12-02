Loop Pipelining (C)
===================

This example demonstrates how loop pipelining can be used to improve the performance of a kernel.

**KEY CONCEPTS:** `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__, `Loop Pipelining <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Function-and-Loop-Pipelining>`__

**KEYWORDS:** `pragma HLS PIPELINE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-pipeline>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc
 - Embedded ZCU102

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   src/vector_addition_BAD.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./loop_pipeline <vector_addition XCLBIN>

DETAILS
-------

This example demonstrates how loop pipelining can be used to improve the
performance of a kernel. Pipelining is a technique to increase
instruction level parallelism in the hardware implementation of an
algorithm by overlapping independent stages of operations and functions.

Two kernels ``kernel_vadd`` and ``kernel_pipelined`` are used for
comparison in performance. Kernel execution time will be less for the
pipelined kernel as it uses ``pragma HLS PIPELINE`` in read and write
loops which enables ``burst transfers`` resulting in effective usage of
bandwidth of ``m_axi`` in interface.

.. code:: cpp

   read_a:
           for (int x = 0; x < N; ++x) {
              #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
              #pragma HLS PIPELINE II=1
               result[x] = a[i * N + x];
           }

Following is the log reported while running the design on U250 platform:

::

   Platform Name: Xilinx
   INFO: Reading build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/vector_addition.xclbin
   Loading: 'build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/vector_addition.xclbin'
   Trying to program device[0]: xilinx_u250_gen3x16_xdma_shell_3_1
   Device[0]: program successful!
   |-------------------------+-------------------------|
   | Kernel                  |    Wall-Clock Time (ns) |
   |-------------------------+-------------------------|
   | vadd: simple            |                   66858 |
   | vadd: pipelined         |                   16410 |
   |-------------------------+-------------------------|
   | Speedup                 |                4.074223 |
   |-------------------------+-------------------------|
   Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.
   Please refer to profile summary for kernel execution time for hardware emulation.
   TEST PASSED.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
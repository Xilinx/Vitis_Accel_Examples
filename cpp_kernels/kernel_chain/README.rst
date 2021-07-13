Stream Chain Matrix Multiplication (C)
======================================

This is a kernel containing the cascaded Matrix Multiplication using dataflow. ap_ctrl_chain is enabled for this kernel to showcase how multiple enqueue of Kernel calls can be overlapped to give higher performance. ap_ctrl_chain allow kernel to start processing of next kernel operation before completing the current kernel operation.

**KEY CONCEPTS:** `ap_ctrl_chain <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/managing_interface_synthesis.html#qls1539734256651__ae476333>`__, PLRAM

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   samsung
   vck
   zc
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_chain_mmult.cpp
   src/krnl_mmult.hpp
   src/krnl_simple_mmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./kernel_chain <krnl_mmult XCLBIN>

DETAILS
-------

This examples demonstrates how ‘ap_ctrl_chain’ in HLS Kernel can help to
improve the performance. In this example matrix multiplication
functionality is used to showcase the benefit. Example contains two
kernels 1. krnl_chain_mmult: Showcases the Kernel with ‘ap_ctrl_chain’
functionality 2. krnl_simple_mmult: Same kernel without ‘ap_ctrl_chain’

Kernel consists of following sub-blocks 1. mm2s: converts AXIMM data
from memory to AXIS data 2. mmult: compute for matrix multplication 2.
s2mm: converts AXIS data from compute kernel to AXIMM

ap_ctrl_chain implements a set of block-level control ports to start the
design operation, continue operation, and indicate when the design is
idle, done, and ready for new input data. The ap_ctrl_chain interface
mode is similar to ap_ctrl_hs but provides an additional input signal
ap_continue to apply back pressure. Xilinx recommends using the
ap_ctrl_chain block-level I/O protocol when chaining Vivado HLS blocks
together.

This examples shows the performance difference between the chain kernel
and simple kernel.

.. code:: cpp

   #pragma HLS INTERFACE s_axilite port = return bundle = control
   #pragma HLS INTERFACE ap_ctrl_chain port = return bundle = control

Following is the real log reported while running the design on U200
platform with 10 iterations:

::

    --------------------------------------------------
                    Performance Summary                  
    --------------------------------------------------
    -------------------------+-------------------------
     Kernel(10 iterations)   |    Wall-Clock Time (s)  
    -------------------------+-------------------------
     krnl_chain_mmult        | 0.00305935              
     krnl_simple_mmult       | 0.00534336              
    -------------------------+-------------------------
     Speedup:                | 1.74657	                
    -------------------------+-------------------------

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
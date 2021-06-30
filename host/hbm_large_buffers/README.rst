HBM Large Buffers
=================

This is a simple example of vector addition to describe how HBM pseudo-channels can be grouped to handle buffers larger than 256 MB.

**KEY CONCEPTS:** `High Bandwidth Memory <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#lgl1614021146997>`__, Multiple HBM Pseudo-channel Groups

**KEYWORDS:** `HBM <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#lgl1614021146997>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   u200
   zc
   vck
   u250
   u280
   aws-vu9p-f1
   _u2_
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hbm_large_buffers <krnl_vadd XCLBIN>

DETAILS
-------

This is a simple example of vector addition to describe how HBM pseudo-channels(PC) can be grouped to handle buffers larger than 256 MB.

Compute unit interfaces are associated to fixed HBM PC groups using
``krnl_vadd.cfg`` file:

::

   [connectivity]
   sp=krnl_vadd_1.in1:HBM[0:31]
   sp=krnl_vadd_1.in2:HBM[0:31]
   sp=krnl_vadd_1.out_r:HBM[0:31]

Here, All three buffers will be created into different bank groups and the application will run and performance will be reported. Following is the real log reported while running on U50 platform :

::

   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u50_gen3x16_xdma_201920_3/krnl_vadd.xclbin
   Loading: './build_dir.hw.xilinx_u50_gen3x16_xdma_201920_3/krnl_vadd.xclbin'
   Trying to program device[0]: xilinx_u50_gen3x16_xdma_201920_3
   Device[0]: program successful!
   Running Three Separate Groups for Three Buffers
   Each buffer is allocated with different HBM Group.
   input 1 -> PC 0 : 31 
   input 2 -> PC 0 : 31 
   output  -> PC 0 : 31 
   THROUGHPUT = 39.2318 GB/s 
   TEST PASSED


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
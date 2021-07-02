Wide Memory Read/Write (OpenCL Kernel)
======================================

This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, V++ compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.

**KEY CONCEPTS:** Kernel to DDR, `wide memory access <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#ddw1586913493144__section_kc3_ykf_jlb>`__, `burst read and write <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#ddw1586913493144__section_ogb_tkf_jlb>`__

**KEYWORDS:** uint16, `xcl_pipeline_loop <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#sgo1504034359903>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc702
   nodma

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

   ./cl_wide_mem_rw <vadd XCLBIN>

DETAILS
-------

This example demonstrates the use of ``ap_uint<>`` datatype which
enables ``V++`` to figure out the memory datawidth for the kernel to
global memory transfers.Using this datatype, larger chunks of data can
be transferred as compared to conventional datatypes such as ``int``.

For efficient use of this bandwidth, ``ap_uint<16>`` is used. User must
include ``ap_int.h`` to use these datatypes. \```c++ #define DATAWIDTH
512 typedef ap_uint uint512_dt;

local uint16 v1_local[LOCAL_MEM_SIZE]; local uint16
result_local[LOCAL_MEM_SIZE];

::

   A maximum of 512 bits can be transferred b/w kernel and global memory, attribute `xcl_pipeline_loop` is used to infer the burst read/write.
   ```c++
    v1_rd: __attribute__((xcl_pipeline_loop(1)))
           __attribute__((xcl_loop_tripcount(c_size, c_size)))
           for (int j = 0 ; j <  chunk_size; j++){
               v1_local[j] = in1 [i + j];
           }

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
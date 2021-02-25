Slave Bridge Bandwidth(C)
=========================

This is slave bridge bandwidth example to describe host memory and kernel bandwidth test.

**KEY CONCEPTS:** slave bridge, bandwidth, address translation unit

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, HOST[0]

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   zc
   vck
   aws-vu9p-f1
   2019
   2018
   202010
   samsung
   _u2_

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   src/read_bandwidth.cpp
   src/write_bandwidth.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_bw.exe <bandwidth XCLBIN>

DETAILS
-------

This is a host application to test Slavebridge bandwidth. It contains 3 kernels namely read_bandwidth, write_bandwidth and bandwidth (for concurrent read and write).The host application runs all 3 kernels with variable buffer sizes and measures the bandwidth for each case. Finally the design reports the maximum bandwidth achieved for each case.

The host allocates a buffer into specific host-only buffer using ``XCL_MEM_EXT_HOST_ONLY``. The ``cl_mem_ext_ptr`` object needs to be used in cases where memory assignment is done by user explicitly:

.. code:: cpp

   cl_mem_ext_ptr_t input_buffer_ext;
   input_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
   input_buffer_ext.obj = NULL;
   input_buffer_ext.param = 0;
   
   OCL_CHECK(err, buffer[0] = new cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, bufsize,
                                             &input_buffer_ext, &err));

Using the ``sp`` option  in the krnl_bandwidth.cfg file, AXI-Master Port is connected to the Slave-Bridge IP:

::

   sp=bandwidth_1.input0:HOST[0]
   sp=bandwidth_1.output0:HOST[0]
   sp=read_bandwidth_1.input0:HOST[0]
   sp=write_bandwidth_1.output0:HOST[0]

Following is the real log reported while running the design on U250 platform:

::

   INFO: Reading ./build_dir.hw.xilinx_u250_gen3x16_xdma_1_1_202010_1/bandwidth.xclbin
   Loading: './build_dir.hw.xilinx_u250_gen3x16_xdma_1_1_202010_1/bandwidth.xclbin'
   Trying to program device[0]: xilinx_u250_gen3x16_xdma_shell_1_1
   Device[0]: program successful!
   Concurrent Read and Write Throughput = 2.65555 (GB/sec) for buffer size 0.00390625 MB
   Read Throughput = 2.31054 (GB/sec) for buffer size 0.00390625 MB
   Write Throughput = 2.34751 (GB/sec) for buffer size 0.00390625 MB
    
   Concurrent Read and Write Throughput = 4.73728 (GB/sec) for buffer size 0.0078125 MB
   Read Throughput = 3.94692 (GB/sec) for buffer size 0.0078125 MB
   Write Throughput = 3.85908 (GB/sec) for buffer size 0.0078125 MB
    
   Concurrent Read and Write Throughput = 7.91636 (GB/sec) for buffer size 0.015625 MB
   Read Throughput = 5.77328 (GB/sec) for buffer size 0.015625 MB
   Write Throughput = 5.87328 (GB/sec) for buffer size 0.015625 MB
    
   Concurrent Read and Write Throughput = 11.8423 (GB/sec) for buffer size 0.03125 MB
   Read Throughput = 7.79106 (GB/sec) for buffer size 0.03125 MB
   Write Throughput = 7.83305 (GB/sec) for buffer size 0.03125 MB
    
   Concurrent Read and Write Throughput = 15.6581 (GB/sec) for buffer size 0.0625 MB
   Read Throughput = 9.32546 (GB/sec) for buffer size 0.0625 MB
   Write Throughput = 9.48782 (GB/sec) for buffer size 0.0625 MB
    
   Concurrent Read and Write Throughput = 18.5828 (GB/sec) for buffer size 0.125 MB
   Read Throughput = 10.3222 (GB/sec) for buffer size 0.125 MB
   Write Throughput = 10.5661 (GB/sec) for buffer size 0.125 MB
    
   Concurrent Read and Write Throughput = 20.5549 (GB/sec) for buffer size 0.25 MB
   Read Throughput = 10.8991 (GB/sec) for buffer size 0.25 MB
   Write Throughput = 11.1812 (GB/sec) for buffer size 0.25 MB
    
   Concurrent Read and Write Throughput = 21.6408 (GB/sec) for buffer size 0.5 MB
   Read Throughput = 11.1996 (GB/sec) for buffer size 0.5 MB
   Write Throughput = 11.5278 (GB/sec) for buffer size 0.5 MB
    
   Concurrent Read and Write Throughput = 22.0769 (GB/sec) for buffer size 1 MB
   Read Throughput = 11.3371 (GB/sec) for buffer size 1 MB
   Write Throughput = 11.7182 (GB/sec) for buffer size 1 MB
    
   Concurrent Read and Write Throughput = 21.9922 (GB/sec) for buffer size 2 MB
   Read Throughput = 11.2091 (GB/sec) for buffer size 2 MB
   Write Throughput = 11.8187 (GB/sec) for buffer size 2 MB
    
   Concurrent Read and Write Throughput = 20.8569 (GB/sec) for buffer size 4 MB
   Read Throughput = 11.2285 (GB/sec) for buffer size 4 MB
   Write Throughput = 11.8662 (GB/sec) for buffer size 4 MB
    
   Concurrent Read and Write Throughput = 19.6829 (GB/sec) for buffer size 8 MB
   Read Throughput = 10.9834 (GB/sec) for buffer size 8 MB
   Write Throughput = 11.8879 (GB/sec) for buffer size 8 MB
    
   Concurrent Read and Write Throughput = 14.6202 (GB/sec) for buffer size 16 MB
   Read Throughput = 11.0069 (GB/sec) for buffer size 16 MB
   Write Throughput = 11.2662 (GB/sec) for buffer size 16 MB
    
   Concurrent Read and Write Throughput = 13.4051 (GB/sec) for buffer size 32 MB
   Read Throughput = 10.9978 (GB/sec) for buffer size 32 MB
   Write Throughput = 11.1839 (GB/sec) for buffer size 32 MB
    
   Concurrent Read and Write Throughput = 13.0062 (GB/sec) for buffer size 64 MB
   Read Throughput = 10.9995 (GB/sec) for buffer size 64 MB
   Write Throughput = 11.1202 (GB/sec) for buffer size 64 MB
   
   Concurrent Read and Write Throughput = 12.7202 (GB/sec) for buffer size 128 MB
   Read Throughput = 10.9816 (GB/sec) for buffer size 128 MB
   Write Throughput = 10.5749 (GB/sec) for buffer size 128 MB
    
   Concurrent Read and Write Throughput = 12.4748 (GB/sec) for buffer size 256 MB
   Read Throughput = 10.9845 (GB/sec) for buffer size 256 MB
   Write Throughput = 10.5827 (GB/sec) for buffer size 256 MB
    
   Maximum bandwidth achieved :
   Concurrent Read and Write Throughput = 22.0769 (GB/sec) for buffer size 1 MB
   Read Throughput = 11.3371 (GB/sec) for buffer size 1 MB
   Write Throughput = 11.8879 (GB/sec) for buffer size 8 MB
    
   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
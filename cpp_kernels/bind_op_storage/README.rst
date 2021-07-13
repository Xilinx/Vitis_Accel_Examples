BIND OP and STORAGE (HLS C/C++ Kernel)
======================================

This is simple example of vector addition to describe how to use BIND OP and STORAGE for better implementation style.

**KEY CONCEPTS:** `BIND OP <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#yew1585574779610>`__, `BIND STORAGE <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#yew1585574779610>`__

**KEYWORDS:** `BIND_OP <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#ttl1584844636775>`__, `BIND_STORAGE <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#chr1584844747152>`__, `impl <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#ttl1584844636775__ad411605>`__, `op <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#ttl1584844636775__ad411605>`__, `type <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#ttl1584844636775__ad411605>`__, `latency <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#ttl1584844636775__ad411605>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zcu102
   zc706
   nodma

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

   ./bind_op_storage <vadd XCLBIN>

DETAILS
-------

In this design we propose a easy way to specify hardware resource and
its properties by new bind_op and bind_storage pragma.

bind_op pragma
~~~~~~~~~~~~~~

::

   #pragma HLS bind_op variable=<string> op=<string> impl=<string> latency=<unsigned>

Using the bind_op pragma we can specify DSP/non-DSP resources and allows
use to specify the latency. In this example we are using addition and
multiply operations using dsp resources.

::

   #pragma HLS BIND_OP variable=v1_buffer op=mul  impl=DSP latency=2
   #pragma HLS BIND_OP variable=v2_buffer op=mul  impl=DSP latency=2
   #pragma HLS BIND_OP variable=vout_buffer op=add  impl=DSP 

bind_storage pragma
~~~~~~~~~~~~~~~~~~~

::

   #pragma HLS bind_storage variable=<string> type=<string> impl=<string> latency=<unsigned> 

Using the bind_storage pragma we have flexibility to decide on the port
type(FIFO/RAM_1P/RAM_2P), storage (BRAM/URAM/LUTRAM) and latency. In
this example we are using RAM_1P with latency 2 for input buffers.

::

   #pragma HLS BIND_STORAGE variable=v1_buffer type=RAM_1P impl=BRAM latency=2
   #pragma HLS BIND_STORAGE variable=v2_buffer type=RAM_1P impl=LUTRAM latency=2
   #pragma HLS BIND_STORAGE variable=vout_buffer type=RAM_1P impl=URAM

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
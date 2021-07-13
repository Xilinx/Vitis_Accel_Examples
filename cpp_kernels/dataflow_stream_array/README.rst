Dataflow Using Array of HLS Stream (C)
======================================

This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.

**KEY CONCEPTS:** Array of Stream

**KEYWORDS:** `dataflow <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#bmx1539734225930>`__, `hls::stream <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_stream_library.html>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/N_stage_adders.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./dataflow_stream_array <N_stage_Adders XCLBIN>

DETAILS
-------

This example demontrates the use of an array of ``HLS streams`` in
kernels.

Kernel performs a number of vector additions. Initial vector is taken
from the global memory and is written into a stream. Operator ``<<`` is
overloaded to perform a ``blocking write`` to a stream from a variable.

.. code:: cpp

    mem_rd:
       for (int i = 0; i < size; i++) {
          #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           inStream << input[i];
       }

Multiple additions are performed using the ``adder`` function which take
the input from a stream and provide the output to another stream.

.. code:: cpp

   compute_loop:
       for (int i = 0; i < STAGES; i++) {
          #pragma HLS UNROLL
           adder(streamArray[i], streamArray[i + 1], incr, size);
       }

Finally, result is written back from stream to global memory buffer.

.. code:: cpp

   static void write_result(int *output, hls::stream<int> &outStream, int size) {
   mem_wr:
       for (int i = 0; i < size; i++) {
          #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           output[i] = outStream.read();
       }
   }

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
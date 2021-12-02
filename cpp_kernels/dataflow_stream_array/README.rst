Dataflow Using Array of HLS Stream (C)
======================================

This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.

**KEY CONCEPTS:** Array of Stream

**KEYWORDS:** `dataflow <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Optimization-Techniques-in-Vitis-HLS>`__, `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

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

This example demonstrates the use of an array of ``HLS streams`` in
kernels, with different depth set for each of the streams.

.. code:: cpp
   
   static hls::stream<int> streamArray[STAGES + 1];
   #pragma HLS STREAM variable=streamArray[0] depth=4
   #pragma HLS STREAM variable=streamArray[1] depth=16
   #pragma HLS STREAM variable=streamArray[2] depth=8


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
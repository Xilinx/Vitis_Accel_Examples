Dataflow Stream Array
=====================

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

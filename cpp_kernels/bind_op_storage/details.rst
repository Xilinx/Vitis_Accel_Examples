BIND OP and STORAGE
===================

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

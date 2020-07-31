Streaming Custom Datatype
=========================

This is simple streaming example of RGB to HSV conversion to demonstrate
how streams of custom data types needs to be handled on the kernel side.
It is recommended that custom datatype is a power of 2 and minimum 32
bits to allow ``burst transfer`` thus using the AXI master bandwidth
efficiently. Extra ``padding`` can be added in case not a multiple of 32
bits as shown below.

In a streaming case, the compiler defined struct only accepts apint as
the datatype. This example showcases a workaround to extract information
from a stream of type pkt to RGBcolor and vice-versa using functions
init and func respectively.

.. code:: cpp

   struct RGBcolor {
     unsigned char r;
     unsigned char g;
     unsigned char b;

     RGBcolor() = default;

     RGBcolor(ap_int<32> d) {
       r = d.range(7, 0);
       g = d.range(15, 8);
       b = d.range(23, 16);
     }

   } __attribute__((packed, aligned(4)));


   struct HSVcolor {
     unsigned char h;
     unsigned char s;
     unsigned char v;

     ap_int<32> packed() { return ap_int<8>(h), ap_int<8>(s), ap_int<8>(v); }

   } __attribute__((packed, aligned(4)));

Kernel in this example uses the above structures as datatypes for its
input and output ports.

::

   using pkt = qdma_axis<DWIDTH, 0, 0, 0>;
   void rgb_to_hsv(hls::stream<pkt>& in,  // Access global memory as RGBcolor struct-wise
                   hls::stream<pkt>& out, // Access Global Memory as HSVcolor struct-wise
                   )

Custom datatypes can be used to reduce the number of
``kernel arguments`` thus reducing the number of interfaces between
kernels and host. It can also help to reduce execution time to set
kernel arguments if number of kernel arguments is large.

Please refer ``streaming_simple`` example to know more about Streaming
kernel usage.

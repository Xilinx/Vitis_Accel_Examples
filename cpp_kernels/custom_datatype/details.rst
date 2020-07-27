Custom Datatype
===============

Kernel ports can have custom datatypes.It is recommended that custom
datatype is a power of 2 and minimum 32 bits to allow ``burst transfer``
thus using the AXI master bandwidth efficiently. Extra ``padding`` can
be added in case not a multiple of 32 bits as shown below.

.. code:: cpp

   typedef struct RGBcolor_struct
   {
     unsigned char r;
     unsigned char g;
     unsigned char b;
     unsigned char pad;
   } RGBcolor;

   typedef struct HSVcolor_struct
   {
     unsigned char h;
     unsigned char s;
     unsigned char v;
     unsigned char pad;
   }HSVcolor;

Kernel in this example uses the above structures as datatypes for its
input and output ports.

::

   void rgb_to_hsv(RGBcolor *in,  // Access global memory as RGBcolor struct-wise
                   HSVcolor *out, // Access Global Memory as HSVcolor struct-wise
                   int size) 

Custom datatypes can be used to reduce the number of
``kernel arguments`` thus reducing the number of interfaces between
kernels and memory. It can also help to reduce execution time to set
kernel arguments if number of kernel arguments is large.

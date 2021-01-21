Advanced Configuration
======================

This is a simple example to describe how user can provide various advanced options in Vitis flow. The same can be achieved by the inclusion of cfg or tcl files.

User can customize the clock_uncertainity using ‘set_clock_uncertainty’. Following is the content of my_directives.tcl file

.. code:: cpp

   set_clock_uncertainty 0.370

The clock uncertainty setting needs to be specified in my_directives.tcl file. We include this tcl file in our vadd_vadd.cfg file and specify the kernel_frequency and clock uncertainity.

Following is the content of cfg file

.. code:: cpp

   kernel_frequency=0:280
   [hls]
   pre_tcl=my_directives.tcl

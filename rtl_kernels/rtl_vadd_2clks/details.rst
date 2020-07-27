Vadd 2 Kernel Clocks (RTL Kernel)
=================================

This example demonstrates the use of ``two kernel clocks`` for a simple
case of vector addition.

An RTL kernel can have up to two external clock interfaces; a primary
clock, ``ap_clk``, and an optional secondary clock, ``ap_clk_2``. Both
clocks can be used for clocking internal logic. However, all external
RTL kernel interfaces must be clocked on the primary clock. Both primary
and secondary clocks support independent automatic frequency scaling.

These clock frequencies are specified at V++ linking stage in the
following manner.

::

   kernel_frequency=0:150|1:250 

Port Width Widening
===================

This example introduces the capability of how Vitis HLS can configure
the size of kernel interface ports.

User can customize the interface ports size using ‘config_interface
-m_axi_max_widen_bitwidth size’ Following is the content of
hls_config.tcl file

.. code:: cpp

   config_interface -m_axi_max_widen_bitwidth 256

The interface size setting need to be specified in hls_config.tcl file.
We include this tcl file in our krnl_port_widen.ini file and by using
the ``--config`` tag in the kernel compile stage we specify the m_axi
interface size.

Following is the content of ini file

.. code:: cpp

   [advanced]
   prop=solution.hls_pre_tcl=hls_config.tcl

Below are the resource numbers while running the design on U200 platform
:

========== ========= ==== === ==== ====
Design     port_size BRAM DSP FF   LUT
========== ========= ==== === ==== ====
krnl_base  512       32   3   6331 4205
krnl_widen 256       18   3   4006 3452
========== ========= ==== === ==== ====

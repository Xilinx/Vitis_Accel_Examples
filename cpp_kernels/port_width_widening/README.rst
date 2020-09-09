Port Width Widening
===================

This example shows how HLS introduces the capability of resizing the port width of the kernel interface ports for better resource utilization maintaining the performance.

**KEY CONCEPTS:** Interface port width auto widening

**KEYWORDS:** m_axi_max_widen_bitwidth 

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_base.cpp
   src/krnl_widen.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./port_width_widening <krnl_port_widen XCLBIN>


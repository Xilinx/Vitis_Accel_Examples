PLRAM Config
=============

This example demonstrates how size and type of each ``PLRAM`` can
be configured on the target platform before kernels or Compute Units 
are linked into the system.

We can use a Tcl script to configure the PLRAM before system linking occurs.
The use of the Tcl script can be enabled on the v++ command line as follows:

.. code:: cpp

     # Setup PLRAM 
     sdx_memory_subsystem::update_plram_specification [get_bd_cells /memory_subsystem] PLRAM_MEM00 { SIZE 2M AXI_DATA_WIDTH 512 
     SLR_ASSIGNMENT SLR0 READ_LATENCY 10 MEMORY_PRIMITIVE URAM} 
     
     sdx_memory_subsystem::update_plram_specification [get_bd_cells /memory_subsystem] PLRAM_MEM01 { SIZE 2M AXI_DATA_WIDTH 512 
     SLR_ASSIGNMENT SLR1 READ_LATENCY 10 MEMORY_PRIMITIVE URAM} 
     
     validate_bd_design -force
     save_bd_design

In the example above, PLRAM_MEM00 is changed to be 2 MB in size and composed of UltraRAM; 
PLRAM_MEM01 is changed to be 2 MB in size and composed of UltraRAM. 

PLRAM can be assigned to a buffer by using ``sp`` tags in the vadd.cfg file along with the port name and kernel name.

::

   [connectivity]
   sp=vadd_1.in1:PLRAM[0]
   sp=vadd_1.in2:PLRAM[1]
   sp=vadd_1.out:PLRAM[1]

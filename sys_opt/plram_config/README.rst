PLRAM Config (C)
================

This example showcases how to configure various PLRAM parameters like Memory Size, Memory Type(BRAM/URAM) etc.

**KEY CONCEPTS:** Vitis Memory Hierarchy, `PLRAMs <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#wsi1614020986106>`__

**KEYWORDS:** `PLRAM <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#wsi1614020986106>`__, BRAM, URAM

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   zc
   vck
   samsung
   _u2_
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./plram_config <vadd XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
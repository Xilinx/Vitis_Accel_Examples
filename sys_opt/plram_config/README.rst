PLRAM Config (C)
================

This example showcases how to configure various PLRAM parameters like Memory Size, Memory Type(BRAM/URAM) etc.

**KEY CONCEPTS:** Vitis Memory Hierarchy, `PLRAMs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-AXI-Interfaces-to-PLRAM>`__

**KEYWORDS:** `PLRAM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-AXI-Interfaces-to-PLRAM>`__, BRAM, URAM

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Versal V70
 - Alveo U280
 - Alveo U55c
 - u50_gen3x16
 - Alveo U55n
 - Alveo U200
 - Samsung U.2 SmartSSD

.. raw:: html

 </details>

.. raw:: html

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

To visit github.io of this repository, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
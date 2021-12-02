HLS Wrapper XO Generator (C)
============================

This is an example of systemC TLM wrapper and XO generation for a given HLS kernel function.

**KEY CONCEPTS:** `SystemC <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-SystemC-Models>`__

**KEYWORDS:** `create_sc_xo <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-SystemC-Models>`__, `ap_ctrl_none <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Block-Level-Control-Protocols>`__

**EXCLUDED PLATFORMS:** 

 - All Platforms with 2018 Version
 - AWS VU9P F1
 - Samsung U.2 SmartSSD
 - Embedded ZC702
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   src/increment.h
   src/mem_read.cpp
   src/mem_write.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hls_wrapper_xo_gen <krnl_incr XCLBIN>

DETAILS
-------

The HLS kernel systemC wrapper enables TLM emulation using an existing HLS kernel. This utility generates the XO file (containing TLM wrappers) for the HLS kernel code.

Wrapper XO generation flow
--------------------------

1. The user provides HLS code and corresponding JSON data for the HLS kernel
2. During XO generation

   a. based on the kernel JSON data, hls2tlm.py util generates the TLM source files (which have TLM based implementation of HLS stream class)
   b. systemcwrapper2json util generates JSON data for packaging the wrapper
   c. gen_ip_package and gen_xo package the XO
3. The packaged XO is generated

Input from the user
-------------------

HLS Kernel Code - The user can provide the kernel code which follows XSC xtlm_ap_ctrl_none protocol and stream-type arguments (currently supports hls::stream, support for others to be added)

HLS Kernel JSON data - To define the HLS kernel in JSON spec fields like name of the kernel, argument name, argument size, argument type, etc are required.

Generating XO and TLM wrappers
------------------------------

To generate the TLM wrapper source and its XO do:

.. code:: c++

        #create_sc_xo command in Makefile
        cd xo_build
        create_sc_xo <path-to-hls-kernel.cpp> <path-to-hls-kernel.json>

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
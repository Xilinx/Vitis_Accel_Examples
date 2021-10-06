HLS Wrapper XO Generator
========================

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

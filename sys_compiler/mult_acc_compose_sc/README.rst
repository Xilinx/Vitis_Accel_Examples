Multiple Accelerators System Compiler
=====================================

In this example we have multiple accelerators in one xclbin, and compose them into a pipeline, with or without CPU processing in-between the PEs.

**KEY CONCEPTS:** Multiple Accelerators

**KEYWORDS:** sync_outputs, custom_sync_outputs

**EXCLUDED PLATFORMS:** 

 - zcu104
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/filter.cpp
   src/filter.hpp
   src/main.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

This example demonstrates the usage of multile accelerators into single xclbin and compose them into a pipeline with CPU processing in between the PE's.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
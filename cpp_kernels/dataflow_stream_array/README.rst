Dataflow Using Array of HLS Stream (C)
======================================

This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.

**KEY CONCEPTS:** Array of Stream

**KEYWORDS:** dataflow, hls::stream

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/N_stage_adders.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./dataflow_stream_array <N_stage_Adders XCLBIN>


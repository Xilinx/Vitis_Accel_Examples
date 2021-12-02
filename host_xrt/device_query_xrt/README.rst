Device Query XRT (XRT Native API's)
===================================

This Example prints device properties using XRT Native APIs. It also displays the limits and capabilities of the hardware.

**KEY CONCEPTS:** Querying device properties

**KEYWORDS:** `xrt::device <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-the-Device-ID-and-Loading-the-XCLBIN>`__

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1 NoDMA

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./device_query_xrt

DETAILS
-------

This example is to get the details of all the devices connected to a system. . The example prints all device properties like device bdf, kdma etc. It reports the details as below:

::

   Device 0 : xilinx_u250_gen3x16_xdma_shell_3_1
     device bdf      : 0000:04:00.1
     device kdma     : 1
     device max freq : 500
     device m2m      : true
     device nodma    : false
   TEST PASSED


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
Advanced Configuration
======================

This is simple example to describe how user can provide various advanced options in Vitis flow.

**KEY CONCEPTS:** `Using custom Tcl <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__, `Setting Kernel Clock Frequency <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Vitis-Compiler-Command>`__, `Clock Uncertainity <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__

**KEYWORDS:** `config <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Vitis-Compiler-General-Options>`__, set_clock_uncertainity, `clock <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__, `freqHz <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__, `hls.pre_tcl <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

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

   ./advanced_config <vadd XCLBIN>

DETAILS
-------

This is a simple example to describe how user can provide various advanced options in Vitis flow. The same can be achieved by the inclusion of cfg or tcl files.

User can customize the clock_uncertainity using ‘set_clock_uncertainty’. Following is the content of my_directives.tcl file

.. code:: cpp

   set_clock_uncertainty 0.370

The clock uncertainty setting needs to be specified in my_directives.tcl file. We include this tcl file in our vadd_vadd.cfg file and specify the kernel_frequency and clock uncertainity.

Following are the contents of cfg file for SoC Platforms -

.. code:: cpp

   [clock]
   freqHz=280000000:vadd_1
   
   [hls]
   pre_tcl=my_directives.tcl

Following are the contents of cfg file for PCIe Platforms -

.. code:: cpp

   kernel_frequency=0:280
   
   [hls]
   pre_tcl=my_directives.tcl

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
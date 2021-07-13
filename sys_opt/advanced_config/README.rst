Advanced Configuration
======================

This is simple example to describe how user can provide various advanced options in Vitis flow.

**KEY CONCEPTS:** `Using custom Tcl <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#mcj1568640526180__section_emy_hbk_dnb>`__, `Setting Kernel Clock Frequency <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#qcm1528577331870__section_frk_>`__, `Clock Uncertainity <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#mcj1568640526180__section_bh5_dg4_bjb>`__

**KEYWORDS:** `config <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#qcm1528577331870__config>`__, set_clock_uncertainity, `kernel_frequency <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#qcm1528577331870__section_frk_xtr_t3b>`__, `hls.pre_tcl <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#mcj1568640526180__section_emy_hbk_dnb>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
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

   ./advanced_config <vadd XCLBIN>

DETAILS
-------

This is a simple example to describe how user can provide various advanced options in Vitis flow. The same can be achieved by the inclusion of cfg or tcl files.

User can customize the clock_uncertainity using ‘set_clock_uncertainty’. Following is the content of my_directives.tcl file

.. code:: cpp

   set_clock_uncertainty 0.370

The clock uncertainty setting needs to be specified in my_directives.tcl file. We include this tcl file in our vadd_vadd.cfg file and specify the kernel_frequency and clock uncertainity.

Following is the content of cfg file

.. code:: cpp

   kernel_frequency=0:280
   [hls]
   pre_tcl=my_directives.tcl

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
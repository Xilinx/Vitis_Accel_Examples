External IO (HLS C/C++ Kernel)
==============================

This example will demonstrate emulation of Xilinx Device's IO with an external process.

**KEY CONCEPTS:** `HLS C Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Interfaces>`__, `OpenCL Host APIs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Programming>`__

**KEYWORDS:** `sim_ipc_axis_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__, `sim_ipc_axis_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__, `gt_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__, `gt_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__

**EXCLUDED PLATFORMS:** 

 - All Xilinx DMA Platforms
 - Versal VCK5000
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./external_io <increment XCLBIN>

DETAILS
-------

Some user applications make use of IO ports in Vitis flow to drive streaming traffic in and out of the platform . This example emulates such designs in HW-Emulation by mimicing the IO port behavior as in HW.

In this example, Master and Slave Stream IO models will be available as prepacked xo's with different data width configuration. In order to instantiate the IP's corresponding to the provided xo, the xosare specified in the v++ command line 

::

   XOS = $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_master_32.xo  $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_slave_32.xo
   $(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) $(XOS) --temp_dir $(BUILD_DIR)  -o'$(BUILD_DIR)/increment.link.xclbin' $(+)

Connections to the Custom IP's specified in xo's are made using ``krnl_incr.cfg`` file as below:

::
   
   [connectivity]
   nk=sim_ipc_axis_master:1:gt_master
   nk=sim_ipc_axis_slave:1:gt_slave
   stream_connect=gt_master.M00_AXIS:increment_1.a
   stream_connect=increment_1.output:gt_slave.S00_AXIS


Note : xo instance name specified on v++ command line  becomes a key which will be used by the external process to refer to a particular stream port 

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
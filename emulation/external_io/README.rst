External IO (HLS C/C++ Kernel)
==============================

This example will demonstrate emulation of Xilinx Device's IO with an external process.

**KEY CONCEPTS:** `HLS C Kernel <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devckernels.html#hxx1556235054362>`__, `OpenCL Host APIs <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html>`__

**KEYWORDS:** `sim_ipc_axis_master <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/runemulation1.html#sbn1601597712644__section_g3t_2s2_nnb>`__, `sim_ipc_axis_slave <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/runemulation1.html#sbn1601597712644__section_g3t_2s2_nnb>`__, `gt_master <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/runemulation1.html#sbn1601597712644__section_gsg_11m_nnb>`__, `gt_slave <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/runemulation1.html#sbn1601597712644__section_c3g_pzl_nnb>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   dma
   vck5000
   aws-vu9p-f1
   samsung
   _u2_

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
External IO System Verilog based generator(HLS C/C++ Kernel)
============================================================

This example will demonstrate emulation of Xilinx Device's IO with a System Verilog based external process.

**KEY CONCEPTS:** `Traffic Generator <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__, `Traffic generation using SystemV <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Traffic-Generators-in-SV/Verilog>`__

**KEYWORDS:** `ipc_axis_master_mirror <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Manually-Creating-Sources-for-Mirror-Modules>`__, `ipc_axis_slave_mirror <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Manually-Creating-Sources-for-Mirror-Modules>`__

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

   src/axis_vip/axi4stream_vip_axi4streampc.sv
   src/axis_vip/axi4stream_vip_if.sv
   src/axis_vip/axi4stream_vip_pkg.sv
   src/axis_vip/axi4stream_vip_v1_1_vl_rfs.sv
   src/axis_vip/ex_sim_axi4stream_vip_mst_0.sv
   src/axis_vip/ex_sim_axi4stream_vip_mst_0_pkg.sv
   src/axis_vip/ex_sim_axi4stream_vip_slv_0.sv
   src/axis_vip/ex_sim_axi4stream_vip_slv_0_pkg.sv
   src/axis_vip/include/xil_common_vip_macros.svh
   src/axis_vip/infra/axis_infrastructure_v1_1_0.vh
   src/axis_vip/infra/axis_infrastructure_v1_1_vl_rfs.v
   src/axis_vip/xil_common_vip_pkg.sv
   src/des.v
   src/host.cpp
   src/increment.cpp
   src/sim_ipc_axis_master/sim_ipc_axis_master.cpp
   src/sim_ipc_axis_master/sim_ipc_axis_master.h
   src/sim_ipc_axis_master/sim_ipc_axis_master_mirror.cpp
   src/sim_ipc_axis_master/sim_ipc_axis_master_mirror.h
   src/sim_ipc_axis_master/sim_ipc_axis_master_mirror.sv
   src/sim_ipc_axis_slave/sim_ipc_axis_slave.cpp
   src/sim_ipc_axis_slave/sim_ipc_axis_slave.h
   src/sim_ipc_axis_slave/sim_ipc_axis_slave_mirror.cpp
   src/sim_ipc_axis_slave/sim_ipc_axis_slave_mirror.h
   src/sim_ipc_axis_slave/sim_ipc_axis_slave_mirror.sv
   src/top.sv
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./external_io_sv <increment XCLBIN>

DETAILS
-------

Some user applications make use of IO ports in Vitis flow to drive streaming traffic in and out of the platform . This example emulates such designs in HW-Emulation by mimicing the IO port behavior as in HW.

In this example, Master and Slave Stream IO models will be available as prepacked xo's with different data width configuration. In order to instantiate the IP's corresponding to the provided xo, the xos are specified in the v++ command line 

::

   XOS = $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_master_32.xo  $(XILINX_VITIS)/data/emulation/XO/sim_ipc_axis_slave_32.xo
   $(VPP) $(VPP_FLAGS) -l $(VPP_LDFLAGS) $(XOS) --temp_dir $(BUILD_DIR)  -o'$(BUILD_DIR)/increment.link.xclbin' $(+)

Connections to the Custom IP's specified in xo's are made using ``krnl_incr.cfg`` file as below:

::
   
   [connectivity]
   nk=sim_ipc_axis_master_32:1:gt_master
   nk=sim_ipc_axis_slave_32:1:gt_slave
   stream_connect=gt_master.M00_AXIS:increment_1.a
   stream_connect=increment_1.output:gt_slave.S00_AXIS

The example essentially contains two parts i.e. the HDL Traffic Generator Top Module and the Vitis Emulation Process. 

The Traffic Generator Top Module injects AXIStream Traffic to the Emulation process. It consists of the design top (in file des.v) which instantiates the AXI Stream master/slave VIP agents and the ipc_axis_master/slave_mirror modules. The AXI Stream master VIP agent is connected to the ipc_axis_master_mirror module via RTL interface whereas the AXI Stream slave VIP is connected to ipc_axis_slave_mirror module.

The ipc_axis_master_mirror converts the pin level AXI transactions to generic_payload and sends through the IPC sockets to the ipc_axis_master module in the Vitis emulation design. Similarly, the ipc_axis_slave_mirror module is instantiated in the Traffic Generator top to connect to the ipc_axis_slave module in the Vitis emulation design.

In the vitis emulation process the IP's ipc_axis_master converts back the generic payload transaction to pin level RTL transaction and drive the traffic to the kernel and the ipc_axis_slave converts the pin level RTL transactions from the kernel back to generic payload transactions. Hence, ipc_axis_master/slave modules and mirror modules act as an IPC bridge between the external TG module and the kernel.

Traffic generator sources are in directory - *src* . It contains following files

::

  top.sv - top level file instatiating desing and contains traffic generator/checker methods
  des.v - top level design.
  axis_vip/* - all files for axi_vip user underneath for traffic generation
  sim_ipc_axis_master_mirror/* - example source code for the mirror module to the sim_ipc_axis_master ip intantiated in Vitis Platform
  sim_ipc_axis_slave_mirror/* - example source code for the mirror module to the sim_ipc_axis_slave ip intantiated in Vitis Platform

Note : xo instance name specified on v++ command line  becomes a key which will be used by the external process to refer to a particular stream port 

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
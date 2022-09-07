Xilinx AIE2 Validate Test   
=========================

Xilinx AIE2 Validate example. 

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - Versal V70

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   pl_controller/kernel/ctrl_pkt_utils.hpp
   pl_controller/kernel/dma_ctrl.hpp
   pl_controller/kernel/enums.hpp
   pl_controller/kernel/graph_ctrl.hpp
   pl_controller/kernel/pl_controller_kernel.hpp
   pl_controller/kernel/pl_controller_top.cpp
   pl_controller/kernel/sender_receiver.cpp
   pl_controller/kernel/sender_receiver.hpp
   pl_controller/kernel/utils.hpp
   aie/graph.h
   aie/graph.cpp
   aie/kernels.h
   aie/kernels/include.h
   aie/kernels/kernels.cc
   sw/host.cpp
   sw/pl_controller.cpp
   sw/pl_controller.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./main.exe final.xclbin ./Work/ps/c_rts/aie_control_config.json ./Work/reports/dma_lock_report.json

If application can be executed successfully, you will get the list just showing below,

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>Example output:</b>

.. raw:: html

 </summary>
|
..

.. code::

   kernel set argument done
   bo sync to device done
   kernel started
   Waiting PL controller done.....
   PL controller done!
   Graph wait done
   INFO::[ Vitis-EM 22 ] [Time elapsed: 21 minute(s) 16 seconds, Emulation time: 0.378542 ms]
   Data transfer between kernel(s) and global memory(s)
   controller_1:m_axi_gmem_aie_ctrl-S_AXI_AIE          RD = 14.344 KB              WR = 1.852 KB        
   controller_1:m_axi_gmem_aie_ctrl2-DDR          RD = 0.000 KB               WR = 0.000 KB        
   controller_1:m_axi_gmem_cfg-DDR          RD = 0.055 KB               WR = 0.000 KB        
   controller_1:m_axi_gmem_out-DDR          RD = 0.000 KB               WR = 0.004 KB        
   controller_1:m_axi_gmem_ucode-DDR          RD = 3.285 KB               WR = 0.000 KB        
   
   check: nerr=0
   qemu-system-microblazeel: /ps_pmc_rp@0: Disconnected clk=1264875165098 ns
   
   Test passed

.. raw:: html

 </details>

.. raw:: html

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.

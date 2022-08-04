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

   pl_controller/kernel/access_axi_mb.hpp
   pl_controller/kernel/maxi.hpp
   pl_controller/kernel/pl_controller_hw.cpp
   pl_controller/kernel/pl_controller_hw.hpp
   pl_controller/kernel/ucode_process.hpp
   pl_controller/ps_helper/pl_controller_sw.cpp
   pl_controller/ps_helper/pl_controller_sw.hpp
   pl_controller/ps_helper/pl_controller_sw_xrt.cpp
   pl_controller/ps_helper/pl_controller_sw_xrt.hpp
   pl_controller/ucode_gen/aie_runtime_control_impl.cpp
   pl_controller/ucode_gen/aie_runtime_control_impl.hpp
   pl_controller/ucode_gen/aie_runtime_control_wrap.cpp
   pl_controller/ucode_gen/aie_runtime_control_wrap.hpp
   pl_controller/ucode_gen/gen_micro_code.cpp
   pl_controller/ucode_gen/ucode_utils.hpp
   src/graph.h
   src/graph.cpp
   src/kernels.h
   src/kernels.cpp
   sw/golden.hpp
   sw/main.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./main.exe final.xclbin ucode.bin

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

   loadMicroCode: name=g.ifm_ddr, id=0
   loadMicroCode: name=g.wts_ddr, id=1
   loadMicroCode: name=g.ofm_ddr, id=2
   core_iter_size=8
   core_control_size=8
   ucode_size=3364, cfg_buf_size=40
   group_id=2
   setAddress: name=g.ifm_ddr, id=0, addr=826781212672
   setAddress: name=g.wts_ddr, id=1, addr=826781425664
   setAddress: name=g.ofm_ddr, id=2, addr=826781507584
   high_addr = 512, low_addr = 0
   kernel start setting argument
   kernel set argument done
   bo sync to device done
   kernel started
   Waiting PL controller done.....
   INFO::[ Vitis-EM 22 ] [Time elapsed: 5 minute(s) 10 seconds, Emulation time: 0.093771 ms]
   Data transfer between kernel(s) and global memory(s)
   controller_1:m_axi_gmem_aie_ctrl-S_AXI_AIE          RD = 2.562 KB               WR = 0.234 KB        
   controller_1:m_axi_gmem_aie_ctrl2-DDR          RD = 0.000 KB               WR = 0.000 KB        
   controller_1:m_axi_gmem_cfg-DDR          RD = 0.055 KB               WR = 0.000 KB        
   controller_1:m_axi_gmem_out-DDR          RD = 0.000 KB               WR = 0.004 KB        
   controller_1:m_axi_gmem_ucode-DDR          RD = 3.285 KB               WR = 0.000 KB
   
   INFO::[ Vitis-EM 22 ] [Time elapsed: 21 minute(s) 5 seconds, Emulation time: 0.375197 ms]
   Data transfer between kernel(s) and global memory(s)
   controller_1:m_axi_gmem_aie_ctrl-S_AXI_AIE          RD = 14.324 KB              WR = 1.820 KB        
   controller_1:m_axi_gmem_aie_ctrl2-DDR          RD = 0.000 KB               WR = 0.000 KB        
   controller_1:m_axi_gmem_cfg-DDR          RD = 0.055 KB               WR = 0.000 KB        
   controller_1:m_axi_gmem_out-DDR          RD = 0.000 KB               WR = 0.004 KB        
   controller_1:m_axi_gmem_ucode-DDR          RD = 3.285 KB               WR = 0.000 KB
   
   out_buf=841
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

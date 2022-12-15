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

   make xclbin TARGET=hw PLATFORM=<path to local xpfm>
   ./host.exe final.xclbin ./Work/ps/c_rts/aie_control_config.json ./Work/reports/dma_lock_report.json

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

   Xclbin Load successful!
   UUID Load successful!
   memory allocation complete
   sync pm buffer complete
   start sender-receiver kernel
   start pl controller kernel
   host_out1[0]=1
   host_out1[1]=2
   host_out1[2]=3
   host_out1[3]=4
   host_out1[4]=5
   host_out1[5]=6
   host_out1[6]=7
   host_out1[7]=8
   host_out1[8]=9
   host_out1[9]=10
   host_out1[10]=11
   host_out1[11]=12
   host_out1[12]=13
   host_out1[13]=14
   host_out1[14]=15
   host_out1[15]=16
   host_out1[16]=17
   host_out1[17]=18
   host_out1[18]=19
   host_out1[19]=20
   host_out1[20]=21
   host_out1[21]=22
   host_out1[22]=23
   host_out1[23]=24
   host_out1[24]=25
   host_out1[25]=26
   host_out1[26]=27
   host_out1[27]=28
   host_out1[28]=29
   host_out1[29]=30
   host_out1[30]=31
   host_out1[31]=32
   qemu-system-microblazeel: /ps_pmc_rp@0: Disconnected clk=26532985404 ns
   
   INFO: [HW-EMU 06-0] Waiting for the simulator process to exit
   INFO: [HW-EMU 06-1] All the simulator processes exited successfully
   INFO: [HW-EMU 07-0] Please refer the path "*/validate/validate_aie2_pl/build_dir.hw_emu.xilinx_v70_gen5x8_qdma_2_202220_1/.run/78892/hw_em/device0/binary_0/behav_waveform/xsim/simulate.log" for more detailed simulation infos, errors and warnings.
   TEST PASSEDTest passed

.. raw:: html

 </details>

.. raw:: html

To visit github.io of this repository , `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.

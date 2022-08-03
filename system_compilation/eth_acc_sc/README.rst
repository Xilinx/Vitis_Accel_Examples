Ethernet System Compiler
========================

This is simple example showcasing custom AXI-streams to PE interfaces in system compiler environment.

**KEY CONCEPTS:** `Asyncronous host interactions <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Asynchronous-Host-Control-of-Accelerator>`__, Custom AXI-Streams

**KEYWORDS:** `compute_async <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Asynchronous-Host-Control-of-Accelerator>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - zcu104
 - Embedded ZCU102
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1
 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/eth.cpp
   src/eth.hpp
   src/main.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

This example demonstrates hardware pipeline with asynchronous host interactions to modify behavior at run time. This design also showcases custom AXI-streams to PE interfaces. 

.. code:: cpp
   

                                           Custom Ethernet
                                            :    +GTY   :    
                                         Ax :+--------+ :
       +-------------------------------------| eth_rx |--(((
       |                                    :+--------+ :   
       |   +---------+  Bx  +---------+  Cx :+--------+ :
       +-->| fsk_mod |----->| fsk_smp |----->| eth_tx |--)))
           +---------+      +---------+     :+--------+ :
              ^  ^             ^  |             ^  |
          drop|  |add       req|  |smp       get|  |cnt
              |  |             |  v             |  v
            +----------------------------------------+
            |                   control              |
            +----------------------------------------+
                  ^      ^     |
               cmd|   dIn|     |dOut
                  |      |     |
                  |      |     v
                        DDR   DDR

  
- A system of pipelined tasks
- AXI stream connections
   - Internal k2k
   - External IO (like GTY kernel modules)
- Uses free running kernels
   - Self executing after start
   - Entirely data driven kernels
- Host interactions
   - Asynchronous control from host
   - Configuration setup
   - Currently through XDMA
   - If latency is important we should use slave-bridge (NYI)
   - Complex control layer over axi-lite

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
Ethernet System Compiler
========================

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

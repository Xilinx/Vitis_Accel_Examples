Xilinx RX Exerciser Loop Design
===============================

Xilinx RX Exerciser Loop Design

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Samsung SmartSSD Computation Storage Drive
 - All NoDMA Platforms, i.e u50 nodma etc
 - All Platforms with 2018 Version
 - qep

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/exerciser_config.hpp
   src/exerciser_config_rx.cpp
   src/exerciser_rx.cpp
   src/general_def.hpp
   src/host.cpp
   src/packet_passthrough_tx_v1.cpp
   src/pkt_fwd_rx.hpp
   src/pkt_fwd_utils.hpp
   src/x3_specs.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./x3_exerciser_loop_rx <exerciser_loop_rx XCLBIN> ./packet_gen_python/packetGen.py

DETAILS
-------

This design demonstrates passthrough of data from  NIC to MAC (Tx) path whereas from MAC to NIC path data is exercised by processing or dropping of packets. This designs only  process only IPv4 UDP packets without VLANs (other packets go through the design untouched). Default UDP Destination port should be 0x1234 to take some action on packet based command in payload. first 2 bytes of payload will be considered as command words. Format of these two bytes are as below:

1 byte: action_id : Determines the operation to be performed on the packet. 

    0 - No operation (NOP), Packet is passed through design unmodified
    
    4 - Drop the packet
    
    5 - Modify the first 16bit of payload with value from the table with table_key key
    
    6 - Insert 16bit value from the table with table_key key are inserted after the command word header. Insert operation   means the length fields of UDP and IPv4 will be updated. 
    
2 byte: table_key (applicable only for 5 and 6 action IDs)



KEY DESIGN FILES
------------

`<src>`_ - Containing host and kernels sources and headers
    
    `src/exerciser_rx.cpp <src/exerciser_rx.cpp>`_ - exerciser vitis kernel which can process the UDP packet as per command word. Default destination port is 0x1234, and pre-initialized table.
    
    `<src/exerciser_config.hpp>`_ - exerciser config Kernel which can modify the default configuration of execiser kernel using XRT Host API command.
    
    `<src/passthrough_tx_v1.cpp>`_ - this is passthrough kernel in tx path of X3.
    
    `<src/pkt_fwd_rx.hpp>`_ - Low level HLS APIs to process UDP frame.
    
    `<src/host.cpp>`_ - XRT Hots application to change the default kernel configuration. Design can modify host code and re-compile and run to make changes in exerciser.
    
`<packet_gen_python>`_ - Containing Python scripts to drive and recieve network traffic in emulation
    
    `<packet_gen_python/packetGen.py>`_ - Network traffic python script to generate and recieve network traffic using Frame level API    
    
    `<packet_gen_python/pcapRun.py>`_ - Network traffic python script to generate and recieve network traffic using PCAP level API    
    
    `<test.pcap>`_ Sample PCAP file which will be used as input pcap file to run PCAP level API flow testing. 


BUILD INSTRUCTIONS
------------------

Instruction below can be used to generate XCLBIN for Hardware (Board) flow.

::

    make all TARGET=hw  DEVICE=<path to X3 xpfm file> 

Instruction below can be used to generate XCLBIN for Hardware Emulation Flow.

:: 

    make all TARGET=hw_emu DEVICE=<path to X3 xpfm emulation platform>

   
Run Instructions
-----------------

In this design,  both type of python scripts are provided 1) Frame based NTG API Flow and 2) PCAP file based Emulation

Please run following command for Frame Based NTG API Flow run

::

   ./<build directory>/x3_exerciser_loop_rx ./<build directory>/exerciser_loop_rx.xclbin ./packet_gen_python/packetGen.py


Please run following command for PCAP Based NTG API Flow run

::

   ./<build directory>/x3_exerciser_loop_rx ./<build directory>/exerciser_loop_rx.xclbin ./packet_gen_python/pcapRun.py

Connectivity Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

Following is the connectivity configuration of this design.

::

    [connectivity]
    stream_connect=MNicTx0:packet_passthrough_tx_v1_1.tx_in
    stream_connect=MMacTxMetaRsp0:packet_passthrough_tx_v1_1.tx_meta_res_in
    stream_connect=MNicTxPause0:packet_passthrough_tx_v1_1.tx_pause_in
    stream_connect=packet_passthrough_tx_v1_1.tx_meta_res_out:SNicTxMetaRsp0
    stream_connect=packet_passthrough_tx_v1_1.tx_out:SMacTx0
    stream_connect=packet_passthrough_tx_v1_1.tx_pause_out:SMacTxPause0
    stream_connect=MMacRx0:exerciser_rx_1.rx_in:512
    stream_connect=exerciser_rx_1.rx_out:SNicRx0
    stream_connect=exerciser_config_rx_1.table_data:exerciser_rx_1.table_in
    stream_connect=exerciser_config_rx_1.udp_dest:exerciser_rx_1.udp_dest
    sp=exerciser_config_rx_1.m_axi_gmem:HOST[0]

    [clock]
    id=3:packet_passthrough_tx_v1_1

To visit github.io of this repository, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.

Xilinx IPTables RX Filter
=========================

Xilinx IPTables RX Filter offload engine.

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/iptfiltersc.cpp
   src/iptparser.hpp
   src/kernel/iptfilterRxsc.cpp
   src/kernel/iptfilterRxsc.hpp
   src/meta_handler_rx.cpp
   src/meta_handler_rx.hpp
   src/net_utils.hpp
   src/packet_filter_sc.hpp
   src/x3_specs.hpp
   src/x3host.py
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

Device
-------
xilinx_x3522p_gen4x8_hn10g_1_202220_1

Follow the instructions below to run IP Tables Vitis application in
hardware emulation flow.

Run Instructions
----------------

Following instructions are used to run application.

::
    $ setenv PLATFORM_PATH <path to platform>
    $ setenv XCL_EMULATION_MODE hw_emu
    $ ./host-hw_emu.exe -n ./src/x3host.py -p test.pcap -f 0 -r ./iptable.rules


Arguments
~~~~~~~~~~~~~~~~~~~~~~~~~~

Following is the arguments expected by the host executable to run the application

=====  ================  ========================================
Args   Depiction         Meaning
-----  ----------------  ----------------------------------------
-f     --flow Run        test(0) or interactive flow (>= 1)
-----  ----------------  ----------------------------------------
-p     --pcap            input file to enable pcap flow
-----  ----------------  ----------------------------------------
-r     --iptable_rules   IPTableV4 rules file
-----  ----------------  ----------------------------------------
-n     --ntg             python script to send/rcv network data
=====  ================  ========================================

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.

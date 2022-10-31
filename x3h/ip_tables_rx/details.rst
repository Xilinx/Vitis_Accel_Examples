X3 NTG: IP Tables RX Vitis System Compiler
===========================================

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

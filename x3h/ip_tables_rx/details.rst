X3 NTG: IP Tables RX Vitis System Compiler
===========================================

This design showcase an application for the X3 hybrid NIC based on providing IPTables-like functionality on the NIC using System Compiler.

We hope that customers will be able to use this example to make it easy for them to begin using the programmable capabilities of the X3 NIC. They may use it as a starting point and customize it to their needs, or write their own from scratch with a similar structure. 

IPTables is a Linux firewall system to apply packet filters to network traffic. It consists of the iptables utility that configures the current rules and the netfilter kernel module(s) that implement the rules in the kernel. Although the IPTables system has numerous tables and filter points, this example will only be concerned with the INPUT filter chain. Each chain contains a list of rules. The packet is compared to each rule in sequence until a rule matches and a decision is made (e.g. to accept or drop the packet). 


Device
~~~~~~

::

   xilinx_x3522p_gen4x8_hn10g_1_202220_1

   Follow the instructions below to run IP Tables Vitis application in
   hardware emulation flow.

Run Instructions
~~~~~~~~~~~~~~~~

Following instructions are used to run application.

::

    $ setenv PLATFORM_PATH <path to platform>
    $ setenv XCL_EMULATION_MODE hw_emu
    $ ./host-hw_emu.exe -n ./src/x3host.py -p test.pcap -f 0 -r ./iptable.rules


Arguments
~~~~~~~~~

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

 Printing Profile Data and Dumping Waveform file (CL)
======================

This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in sdaccel.ini file to see the live waveform while running application.

***KEY CONCEPTS:*** Use of Profile API, Waveform Dumping and loading


For more information, please refer: [details.md][]

[details.md]: details.md


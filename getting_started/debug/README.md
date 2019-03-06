Debug/Profile Examples
==================================
Debugging and Profiling of Kernel.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[debug_printf_ocl/][]|This is simple example of vector addition and printing of data that is computational result (addition). It is based on vectored addition that demonstrates printing of work item data (integer product in this case)|__Key__ __Concepts__<br> - Use of print statements for debugging<br>__Keywords__<br> - printf<br> - param:compiler.enableAutoPipelining=false
[debug_profile_ocl/][]|This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in sdaccel.ini file to see the live waveform while running application.|__Key__ __Concepts__<br> - Use of Profile API<br> - Waveform Dumping and loading<br>

[.]:.
[debug_printf_ocl/]:debug_printf_ocl/
[debug_profile_ocl/]:debug_profile_ocl/

Debug Profile (C)
======================

This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in xrt.ini file to see the live waveform while running application.

***KEY CONCEPTS:*** Use of Profile API, Waveform Dumping and loading

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/host.h
src/vadd.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
```


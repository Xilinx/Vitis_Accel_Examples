Scout Example Repository
===========================

Welcome to the Scout example repository. This repository contains the latest examples to get you started with application optimization targeting Xilinx PCIe FPGA acceleration boards. All examples are ready to be compiled and executed on Scout supported boards and accelerated cloud service partners. The repository is organized as follows:

1. PREREQUISITE
2. SUPPORTED PLATFORMS
3. COMPILATION AND EXECUTION
4. DIRECTORY STRUCTURE
5. EXECUTION IN CLOUD ENVIRONMENTS
6. SUPPORT


## 1. PREREQUISITE

Scout Git Examples Assumes that user is familiar with Basic Scout Environment, Setup, Programming and Debugging Flow. If not, it is recommended to cover these topics from [Scout User Guides][]:
 - UG#### - Scout Development Environment
 - UG#### - Scout Programming Guide
 - UG#### - Scout Debugging Guide

## 2. SUPPORTED PLATFORMS

Board | Device Name | Software Version
------|-------------|-----------------
Xilinx Alveo U250|xilinx_u250_xdma_201820_1|SDx 2018.3
Xilinx Virtex UltraScale+ VCU1525|xilinx_vcu1525_dynamic|SDx 2018.3
Xilinx Alveo U200|xilinx_u200_xdma_201820_1|SDx 2018.3

## 3. COMPILATION AND EXECUTION

It is primarily recommended to start with [Hello World][] example which makes the new users aware about the basic structure of an Scout based Application.

### Compiling for Application Emulation
As part of the capabilities available to an application developer, Scout includes environments to test the correctness of an application at both a software functional level and a hardware emulated level.

These modes, which are named sw_emu and hw_emu, allow the developer to profile and evaluate the performance of a design before compiling for board execution.
It is recommended that all applications are executed in at least the sw_emu mode before being compiled and executed on an FPGA board.

```
    cd <PATH TO SAMPLE APPLICATION>
    make all TARGET=<sw_emu|hw_emu> DEVICE=<FPGA Platform>
```
where
```
	sw_emu = software emulation
	hw_emu = hardware emulation
```

*NOTE:* The software emulation flow is a functional correctness check only. It does not estimate the performance of the application in hardware.

The hardware emulation flow is a cycle accurate simulation of the hardware generated for the application. As such, it is expected for this simulation to take a long time.
It is recommended that for this example the user skips running hardware emulation or modifies the example to work on a reduced data set.

### Executing Emulated Application 
***Recommended Execution Flow for Example Applications in Emulation*** 

The makefile for the application can directly executed the application with the following command:
```
    cd <PATH TO SAMPLE APPLICATION>
    make check TARGET=<sw_emu|hw_emu> DEVICE=<FPGA Platform>

```
where
```
	sw_emu = software emulation
	hw_emu = hardware emulation
```
If the application has not been previously compiled, the check makefile rule will compile and execute the application in the emulation mode selected by the user.

### Compiling for Application Execution in the FPGA Accelerator Card
The command to compile the application for execution on the FPGA acceleration board is
```
    cd <PATH TO SAMPLE APPLICATION>
    make all DEVICE=<FPGA Platform>
```
The default target for the makefile is to compile for hardware. Therefore, setting the TARGETS option is not required.
*NOTE:* Compilation for application execution in hardware generates custom logic to implement the functionality of the kernels in an application.
It is typical for hardware compile times to range from 30 minutes to a couple of hours.

## 4. DIRECTORY STRUCTURE

- ### HELLO WORLD
Hello World example for new users.

- ### HOST
Collection of examples geared at OpenCL host code for optimized interfacing with Xilinx Devices

- ### CPP KERNELS
This page contains examples for users who are new to Xilinx SDx OpenCL Flows. The focus of the examples is towards code optimization using C++ kernels for Xilinx devices.

- ### RTL KERNELS
Collection of RTL Kernel Based Examples

- ### OCL KERNELS
This page contains examples for users who are new to Xilinx SDx OpenCL Flows. The focus of the examples is towards code optimization using OpenCL kernels for Xilinx devices.

- ### APPLICATIONS
This page contains collection of examples in processor offloading to FPGA based compute units.

- ### COMMON
This folder contains the common utilities and includes for application
execution. Collection of common includes used across all examples to assist in the quick development of application host code. Collection of utility functions used as part of the Makefiles in all of the examples. This set includes Makefile rules and scripts to launch Scout compiled applications onto boards hosted by Nimbix directly from the developers terminal shell.

## 5. Execution in Cloud Environments

FPGA acceleration boards have been deployed to the cloud. For information on how to execute the example within a specific cloud, take a look at the following guides.
* [AWS F1 Application Execution on Xilinx Devices]
* [Nimbix Application Execution on Xilinx Devices] 

## 6. SUPPORT

For questions and to get help on any project in this repository or your own projects, visit the [Scout Forums][].

To execute these example using the Scout GUI, follow the setup instructions in [Scout GUI README][]



[AWS F1 Application Execution on Xilinx Devices]: https://github.com/aws/aws-fpga/blob/master/SDAccel/README.md
[Nimbix Application Execution on Xilinx Devices]: ./common/utility/nimbix/README.md
[Hello World]: 100_hello_world/101_hello_world/
[Scout User Guides]: http://www.xilinx.com/support/documentation-navigation/development-tools/software-development/sdaccel.html?resultsTablePreSelect=documenttype:SeeAll#documentation
[Scout Forums]: https://forums.xilinx.com/t5/Scout/bd-p/SDx
[Scout GUI README]: ./GUIREADME.md

# ScoutExamples

S.No.   | Category  | Description 
--------|-----------|-----------------------------------------
1 | hello_world      |Hello World example for new users
2 | host      |OpenCL host code for optimized interfacing with Xilinx Devices
3 | cpp_kernel     |HLS C/C++ Based Examples.
4 | rtl_kernels     |RTL Kernel based Examples.
5 | ocl_kernels      |OCL Kernel based Examples
6 | applications     |Application which shows acceleration on FPGA.

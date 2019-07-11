Device Query CPP
================
The role of this example to get the details of all the OpenCL Devices connected to system. Platform IDs and Device IDs can be extracted using Open CL APIs which are subsequently needed to configure the context and command queues for communication between host and device.
Example first searches the all connected Platforms using clGetPlatformIDs() API. Then it traverses to each Platform and finds all the connected devices using clGetDeviceIDs(). Then it traverses all the devices and print all device properties like device type, vendor id etc.

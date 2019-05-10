Device Query
================
The role of this example to get the details of all the OpenCL Devices connected to system.
Example first search the all connected Platforms using clGetPlatformIDs() API. Then it traverse to each Platform and find all the connected devices using clGetDeviceIDs(). Then it traverse all the devices and print all device properties like device type, vendor id, etc.

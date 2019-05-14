Device Query CPP
================
The role of this example is to get the details of all the OpenCL Devices connected to system. Example first searches all the connected Platforms using cl::platform::get() API. Then it traverses to each Platform and finds all the connected devices using cl::platform::getDevices(). Then it traverses all the devices and prints all device properties like device type, vendor id, etc.



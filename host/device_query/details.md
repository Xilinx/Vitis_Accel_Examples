Device Query CPP
================
This example is to get the details of all the OpenCL Devices connected to system. Platform IDs and Device IDs can be extracted using Open CL APIs which are subsequently needed to configure the context and command queues for communication between host and device.
Example first searches all the connected Platforms using `clGetPlatformIDs()` API. Then it traverses to each Platform and finds all the connected devices using `clGetDeviceIDs()`. Then it traverses all the devices and print all device properties like device type, vendor id etc.
It reports the details as below:
```
platform profile    : EMBEDDED_PROFILE
platform version    : OpenCL 1.0
platform name       : Xilinx
platform vendor     : Xilinx
platform extensions : cl_khr_icd
Device 0: 
  device type                            : CL_DEVICE_TYPE_ACCELERATOR
  device vendor id                       : 0
  device max compute units               : 0
  device max work item dimensions        : 3
  device max work group size             : 4294967295
  device max work item sizes             : [  4294967295  4294967295  4294967295  ]
  device preferred vector width char     : 1
  device preferred vector width short    : 1
  device preferred vector width int      : 1
  device preferred vector width long     : 1
  device preferred vector width float    : 1
  device preferred vector width double   : 0
  device max clock frequency             : 509
  device address bits                    : 64
  device max read image args             : 128
  device max write image args            : 8
  device max mem alloc size              : 536870912
  device image2d max width               : 8192
  device image2d max height              : 8192
  device image3d max width               : 2048
  device image3d max height              : 2048
  device image3d max depth               : 2048
  device image support                   : true
  device max parameter size              : 2048
  device max samplers                    : 0
  device mem base addr align             : 32768
  device min data type align size        : 128
  device single fp config                : [ CL_FP_INF_NAN ]
  device global mem cache type           : CL_NONE
  device global mem cacheline size       : 64
  device global mem cache size           : 0
  device global mem size                 : 0
  device max constant buffer size        : 4194304
  device max constant args               : 8
  device local mem type                  : CL_LOCAL
  device local mem size                  : 16384
  device error correction support        : true
  device profiling timer resolution      : 1
  device endian little                   : true
  device available                       : false
  device compiler available              : false
  device execution capabilities          : [ CL_EXEC_KERNEL ]
  device queue properties                : [ CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE CL_QUEUE_PROFILING_ENABLE ]
  device name                            : 
  device vendor                          : Xilinx
  device version                         : 1.0
  device profile                         : EMBEDDED_PROFILE
  device version                         : OpenCL 1.0
  device extensions                      : 
  device platform                        : Xilinx
  device double fp config                : [ ]
  device preferred vector width half     : 1
  device host unified memory             : true
  device native vector width char        : 1
  device native vector width short       : 1
  device native vector width int         : 1
  device native vector width long        : 1
  device native vector width float       : 1
  device native vector width double      : 1
  device native vector width half        : 1
  device opencl c version                : OpenCL C 1.0
  device linker available                : true
  device built in kernels                : 
  device image max buffer size           : 65536
  device image max array size            : 2048
  device parent device                   : N/A 
  device partition max sub devices       : 0
  device partition properties            : NONE
  device partition affinity domain       : [ ]
  device partition type                  : N/A 
  device reference countplatforms[p].getDevices(CL_DEVICE_TYPE_ALL, &devices): 1
  device preferred interop user sync     : true
  device printf buffer size              : 0
TEST PASSED
```

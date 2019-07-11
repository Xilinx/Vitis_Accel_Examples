OpenCL Subdevices
==================
This example illustrates how to create `OpenCL subdevices` which use a single kernel multiple times.
Each of these subdevices can act as a seperate device and can have its own command queue, buffers, sequencing and program (However, same program can also be used). Any OpenCL API with device as a parameter can have subdevice as a parameter as well.

```c++
sub_q[i] = cl::CommandQueue(sub_context, device, 0, &err);
sub_kernel[i] = cl::Kernel(program, "krnl_vadd", &err);
```

Creating subdevices gives the user more control over the allocation of compute resources in the device and high priority tasks can have dedicated resources. Creating subdevices can also prove useful when some portion of the device may be used for other tasks than OpenCL.

Subdevices can be divided further into more subdevices if needed. Each subdevice has a parent device from which it is derived. Creating subdevices doesn't destroy the parent device and they can be used together if needed.


OpenCL API `clcreateSubDevices` is used to create subdevices and  device partition properties are used to specify how the resources will be divided among subdevices. In this example,  `CL_DEVICE_PARTITION_EQUALLY` is used which will partition the device into as many subdevices as possible each with the number of compute units as mentioned in the API.

```c++
const cl_device_partition_property device_part_properties[3] = {
            CL_DEVICE_PARTITION_EQUALLY  ,
            1, // Use only one compute unit
            0 //CL_DEVICE_PARTITION_BY_COUNTS_LIST_END
    };
err = device.createSubDevices(device_part_properties, &sub_devices);
```
Device can also be partitioned with unequal compute units for each subdevice with `CL_DEVICE_PARTITION_BY_COUNTS` which allows specific number of compute units to be assigned for each subdevice.


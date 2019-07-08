Device Query CPP
================
This example prints devices available on this machine and their corresponding capabilities.Platform IDs and Device IDs can be extracted using Open CL APIs which are subsequently needed to configure the context and command queues for communication between host and device.

OpenCL CPP API `cl::Platform::get` is used to get the `platform_count` and other information pertaining to platforms in the `platforms vector`.
```C
std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    platform_count = platforms.size();
```
Platforms' info is then printed using `print_platform_info` function which calls `platform.getInfo` API with different flags in `platforms_info[]`.
```c
for (int p = 0; p < (int)platform_count; ++p) {
        print_platform_info(platforms[p]);
```        

Similarly, `platform[p].getDevices` is used to get the `device_count` and corresponding IDs for the devices available for all platforms returned by the `cl::Platform::get`. Bitfield `device_type` in arguments is set to `CL_DEVICE_TYPE_ALL` which instructs the API to return all types of devices on the platform. Other bitfields can be used to return specific types of devices(CL_DEVICE_TYPE_CPU,CL_DEVICE_TYPE_ACCELERATOR etc).
```c
        platforms[p].getDevices(CL_DEVICE_TYPE_ALL, &devices);
        device_count = devices.size();
  ```                              


`getInfo` API is once used to display device name and other properties are printed by `print_device_info` function which calls `devices.getInfo` with different flags in device_info[].

```c++
for (int d = 0; d < (int)device_count; ++d) {
           devices[d].getInfo(CL_DEVICE_NAME, &field);
           printf("Device %d: %s\n", d, field.c_str());
           print_device_info(devices[d]);
           continue;
       }

```

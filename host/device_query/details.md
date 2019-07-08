Device Query
================
This example prints devices available on this machine and their corresponding capabilities. Platform IDs and Device IDs can be extracted using Open CL APIs which are subsequently needed to configure the context and command queues on the device.

`clGetPlatformIDs` API is used to get the information about the platforms available. It returns a list of OpenCL platforms found on the machine and their corresponding IDs.

First the `platform_count` is determined using `clGetPlatformIDs` and in the second call to the same API, first argument(number of platform ID entries) is set to that count and the IDs returned are stored in `platforms` vector.
```C
cl_uint platform_count;
    OCL_CHECK(clGetPlatformIDs(0, nullptr, &platform_count));
    vector<cl_platform_id> platforms(platform_count);
    OCL_CHECK(clGetPlatformIDs(platform_count, platforms.data(), nullptr));
```
These IDs and other information pertaining to these platforms is then printed using `print_platform_info` function which calls clGetPlatformInfo with different flags.
```c
for (int p = 0; p < (int)platform_count; ++p) {
        print_platform_info(platforms[p]);
```        

Similarly, `clGetDeviceIDs` is used to get the `device_count` and corresponding IDs for the devices available for all platforms returned by the `clGetPlatformIDs API`. Bitfield `device_type` in arguments is set to `CL_DEVICE_TYPE_ALL` which instructs the API to return all types of devices on the platform. Other bitfields can be used to return specific types of devices.(CL_DEVICE_TYPE_CPU,CL_DEVICE_TYPE_ACCELERATOR etc).
```c
OCL_CHECK(clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, nullptr,
                                &device_count));
       vector<cl_device_id> devices(device_count);
       OCL_CHECK(clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, device_count,
                                devices.data(), nullptr));
  ```                              


`clGetDeviceInfo` API is once used to display device name and other properties are printed by `print_device_info` function which calls `clGetDeviceInfo` with different flags.

```c++
for (int d = 0; d < (int)device_count; ++d) {
           clGetDeviceInfo(devices[d], CL_DEVICE_NAME, field.size(),
                           (void*)field.data(), nullptr);
           printf("Device %d: %s\n", d, field.c_str());
           print_device_info(devices[d]);
           continue;
       }

```

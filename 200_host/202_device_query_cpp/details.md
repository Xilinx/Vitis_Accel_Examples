Device Query CPP
================
This example prints devices available on this machine and their corresponding capabilities.

`OCL_CHECK` is used to wrap any OPENCL API calls. It returns an error code from the API called inside it enabling a quick error check.
```C
#define OCL_CHECK(error,call)                                       \
    call;                                                           \
    if (error != CL_SUCCESS) {                                      \
      printf("%s:%d Error calling " #call ", error code is: %d\n",  \
              __FILE__,__LINE__, error);                            \
      exit(EXIT_FAILURE);                                           \
    }       

```

OpenCL CPP API `cl::Platform::get` is used to get the `platform_count` and other information pertaining to platforms in the `platforms vector`.
```C
std::vector<cl::Platform> platforms;
    OCL_CHECK(err, err = cl::Platform::get(&platforms));
    platform_count = platforms.size();
```
Platforms' info is then printed using `print_platform_info` function which calls `platform.getInfo` API with different flags in `platforms_info[]`.
```c
for (int p = 0; p < (int)platform_count; ++p) {
        print_platform_info(platforms[p]);
```        

Similarly, `platform[p].getDevices` is used to get the `device_count` and corresponding IDs for the devices available for all platforms returned by the `cl::Platform::get`. Bitfield `device_type` in arguments is set to `CL_DEVICE_TYPE_ALL` which instructs the API to return all types of devices on the platform. Other bitfields can be used to return specific types of devices(CL_DEVICE_TYPE_CPU,CL_DEVICE_TYPE_ACCELERATOR etc).
```c
        OCL_CHECK(err, err = platforms[p].getDevices(CL_DEVICE_TYPE_ALL, &devices));
        device_count = devices.size();
  ```                              


`getInfo` API is once used to display device name and other properties are printed by `print_device_info` function which calls `devices.getInfo` with different flags in device_info[].

```c++
for (int d = 0; d < (int)device_count; ++d) {
           OCL_CHECK(err, err = devices[d].getInfo(CL_DEVICE_NAME, &field));
           printf("Device %d: %s\n", d, field.c_str());
           print_device_info(devices[d]);
           continue;
       }

```


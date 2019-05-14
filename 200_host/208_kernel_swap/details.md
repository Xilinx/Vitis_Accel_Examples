Kernel Swap
============

This example illustrates how kernels in different binaries can share the resources allocated by host. Two kernels `krnl_vadd` and `krnl_vmult` are created in seperate binaries.



Pointer `h_temp` is shared by both these kernels.

`krnl_vmult` uses it to store the multiplication result back from kernel to host.
```c++
OCL_CHECK(err, cl::Buffer d_mul(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                   sizeof(int) * LENGTH, h_temp.data(), &err));
```                   

`krnl_vadd` uses it to get the elements for addition operation.
```c++
cl::Buffer d_temp(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,  
                sizeof(int) * LENGTH, h_temp.data());
```

_Note :_  The temporary pointer(h_temp) is created mainly for the dynamic platforms,
since in the dynamic platforms we will not be able to load a second xclbin
unless all the cl buffers are released before calling cl::Program a second
time in the same process.


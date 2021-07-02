P2P Simple Example
==================

This is simple example of vector increment to describe P2P between FPGA and NVMe SSD.

**KEY CONCEPTS:** `P2P <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/Chunk938767849.html#qex1558551641915>`__, NVMe SSD, SmartSSD

**KEYWORDS:** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite, O_DIRECT, O_RDWR

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   _u25_
   u30
   vck
   aws-vu9p-f1
   201910
   u50_gen3x16_xdma_2019

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/sample.txt
   src/adder.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_simple -x <adder XCLBIN> -f ./data/sample.txt

DETAILS
-------

PCIe peer-to-peer communication (P2P) is a PCIe feature which enables
two PCIe devices to directly transfer data between each other without
using host RAM as a temporary storage. The latest version of Vitis PCIe
platforms support P2P feature via PCIe Resizeable BAR Capability

Using the P2P enabled device the data can be transferred between the
FPGA device and another NVMe Device, such as SMART SSD, without
migrating the data via host memory space

.. code:: cpp

   // Creating P2P Buffer
     cl_mem_ext_ptr_t outExt;
     outExt = {XCL_MEM_EXT_P2P_BUFFER, nullptr, 0};

     cl_mem p2pBo =
         clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX,
                        vector_size_bytes, &outExt, &err);
     clSetKernelArg(krnl_adder, narg++, sizeof(cl_mem), &p2pBo);

   // Map P2P buffer to host access pointers
     p2pPtr = (uint32_t *)clEnqueueMapBuffer(
           q, p2pBo, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, vector_size_bytes, 0, nullptr, nullptr, nullptr);

     filename = <full path to SSD>
     fd = open(filename, O_RDWR | O_DIRECT);

   // Read chunk_size bytes starting at offset 0 from fd into p2pPtr
     pread(fd, p2pPtr, chunk_size, 0);

   // Wrtie chunk_size bytes starting at offset 0 from p2pPtr into fd
     pwrite(fd, p2pPtr, chunk_size, 0);

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
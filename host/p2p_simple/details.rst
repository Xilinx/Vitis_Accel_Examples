P2P Simple
==========

PCIe peer-to-peer communication (P2P) is a PCIe feature which enables
two PCIe devices to directly transfer data between each other without
using host RAM as a temporary storage. The latest version of SDx PCIe
platforms support P2P feature via PCIe Resizeable BAR Capability

Using the P2P enabled device the data can be transferred between the
FPGA device and another NVMe Device, such as SMART SSD, without
migrating the data via host memory space

.. code:: cpp

   // Creating P2P Buffer
     cl_mem_ext_ptr_t outExt;
     outExt = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};

     cl_mem p2pBo =
         clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX,
                        vector_size_bytes, &outExt, &err);
     clSetKernelArg(krnl_adder, narg++, sizeof(cl_mem), &p2pBo);

   // Map P2P buffer to host access pointers
     p2pPtr = (uint32_t *)clEnqueueMapBuffer(
           q, p2pBo, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, vector_size_bytes, 0, NULL, NULL, NULL);

     filename = <full path to SSD>
     fd = open(filename, O_RDWR | O_DIRECT);

   // Read chunk_size bytes starting at offset 0 from fd into p2pPtr
     pread(fd, p2pPtr, chunk_size, 0);

   // Wrtie chunk_size bytes starting at offset 0 from p2pPtr into fd
     pwrite(fd, p2pPtr, chunk_size, 0);

User Managed IP (RTL Kernel)
============================

This example demonstrates how a user can create a User-Managed RTL IP. The RTL IP here does simple vector addition where two vectors are transferred from host to kernel, added and the result is written back to the host and verified. The IP's control protocol is mentioned as user_managed by adding ``-ctrl_protocol user_managed`` to the package command as below: 

::

   package_xo -ctrl_protocol user_managed -xo_path ${xoname} -kernel_name krnl_vadd_rtl -ip_directory ./packaged_kernel_${suffix}

The IP is first created and the memory index and register offset determined as below:  

.. code:: cpp

   int reg_offset[4];
   auto ip = xrt::ip(device, uuid, "krnl_vadd_rtl");
   auto xclbin = xrt::xclbin(binaryFile);
   std::cout << "Determine buffer offsets\n";
   for (auto& kernel : xclbin.get_kernels()) {
       for (auto& cu : kernel.get_cus()) {
           int i = 0;
           for (auto& arg : cu.get_args()) {
               reg_offset[i] = arg.get_offset();
               i++;
           }
       }
   }
   
   std::cout << "Determine memory index\n";
   for (auto& mem : xclbin.get_mems()) {
   if (mem.get_used()) {
      mem_used = mem;
      break;
   }
   }

All the IP settings are achieved using the ``write_register`` and ``read_register`` calls as below:

.. code:: cpp

   std::cout << "INFO: Setting IP Data" << std::endl;
   
   for (int i = 0; i < 3; i++) {
   ip.write_register(reg_offset[i], buf_addr[i]);
   ip.write_register(reg_offset[i] + 4, buf_addr[i] >> 32);
   }
   
   ip.write_register(reg_offset[3], DATA_SIZE);
   
   uint32_t axi_ctrl = 0;
   
   std::cout << "INFO: IP Start" << std::endl;
   axi_ctrl = IP_START;
   ip.write_register(CSR_OFFSET, axi_ctrl);
   
   // Wait until the IP is DONE
   
   axi_ctrl = 0;
   while ((axi_ctrl & IP_IDLE) != IP_IDLE) {
       axi_ctrl = ip.read_register(CSR_OFFSET);
   }
   
   std::cout << "INFO: IP Done" << std::endl;

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``.
These kernels have the same software interface model as OpenCL and C/C++
kernels. That is, they are seen by the host application as functions
with a void return value, scalar arguments, and pointer arguments.

The RTL Kernel Wizard automates some of the steps that need to be taken
to ensure that the RTL IP is packaged into a kernel that can be
integrated into a system in Vitis environment.

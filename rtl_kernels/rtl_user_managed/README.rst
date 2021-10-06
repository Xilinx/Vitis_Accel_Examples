User Managed IP (RTL Kernel)
============================

Simple example of user managed RTL Kernel.

**KEY CONCEPTS:** `User-Managed RTL Kernel <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html#lvg1620349851355>`__

**KEYWORDS:** `package_xo <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/package_xo.html#fsi1542298725587__section_zzf_f5y_q3b>`__, `ctrl_protocol <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/package_xo.html#fsi1542298725587__section_mhz_2p5_5fb>`__, `user_managed <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html#lvg1620349851355>`__, `xrt::ip <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#jln1620691667890>`__, `xrt::xclbin <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#zja1524097906844>`__, xrt::kernel::get_kernels, xrt::kernel::get_cus, xrt::kernel::get_args, xrt::arg::get_offset, xrt::ip::write_register, xrt::ip::read_register

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/hdl/krnl_vadd_rtl.v
   src/hdl/krnl_vadd_rtl_adder.sv
   src/hdl/krnl_vadd_rtl_axi_read_master.sv
   src/hdl/krnl_vadd_rtl_axi_write_master.sv
   src/hdl/krnl_vadd_rtl_control_s_axi.v
   src/hdl/krnl_vadd_rtl_counter.sv
   src/hdl/krnl_vadd_rtl_int.sv
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_user_ip -x <vadd XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.